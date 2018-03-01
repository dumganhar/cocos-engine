const Enums = require('../platform/CCEnum');
const renderEngine = require('./render-engine');
const gfx = renderEngine.gfx;
const renderer = renderEngine.renderer;
const RecyclePool = renderEngine.RecyclePool;

// Stage types
var Stage = cc.Enum({
    // Stencil disabled
    DISABLED: 0,
    // Entering a new level, should handle new stencil
    ENTER_LEVEL: 1,
    // In stencil
    ENABLED: 2,
    // Exiting a level, should restore old stencil or disable
    EXIT_LEVEL: 3,
});

let _passPool = new RecyclePool(function () {
    return new renderer.Pass("");
}, 16);
let _techPool = new RecyclePool(function () {
    return new renderer.Technique([], null, []);
}, 16);
let _effectPool = new RecyclePool(function () {
    return new renderer.Effect([], {}, []);
}, 16);

function clonePass (from) {
    let to = _passPool.add();
    for (var name in from) {
        to[name] = from[name];
    }
    return to;
}

function cloneEffect (effect) {
    let clone = _effectPool.add();
    // properties and defines won't change
    clone._properties = effect._properties;
    clone._defines = effect._defines;
    // only deal with transparent stage
    let technique = effect.getTechnique('transparent');
    let cloneTech = _techPool.add();
    cloneTech._parameters = technique._parameters;
    cloneTech._stageIDs = technique._stageIDs;
    cloneTech._layer = technique._layer;
    // clone passes
    let l = cloneTech._passes.length = technique._passes.length;
    for (let i = 0; i < l; i++) {
        let pass = technique._passes[i];
        cloneTech._passes[i] = clonePass(pass);
    }
    clone._techniques.push(cloneTech);
    return clone;
}

function StencilManager () {
    // todo: 8 is least Stencil depth supported by webGL device, it could be adjusted to vendor implementation value
    this._maxLevel = 8;
    // Current mask
    this._maskStack = [];
    // Current stage of process, DISABLED, ENTER_LEVEL, ENABLED, EXIT_LEVEL
    this.stage = Stage.DISABLED;
}

StencilManager.prototype = {
    constructor: StencilManager,

    reset () {
        // Reset pools
        _effectPool.reset();
        _techPool.reset();
        _passPool.reset();
        
        // reset stack and stage
        this._maskStack.length = 0;
        this.stage = Stage.DISABLED;
    },

    handleEffect (effect) {
        let technique = effect.getTechnique('transparent');
        if (this.stage === Stage.DISABLED) {
            this.stage = Stage.DISABLED;
            for (let i = 0; i < technique._passes.length; ++i) {
                let pass = technique._passes[i];
                pass._stencilTest = false;
            }
            return effect;
        }

        let mask, func, ref, stencilMask, writeMask, failOp,
            zFailOp = gfx.STENCIL_OP_KEEP,
            zPassOp = gfx.STENCIL_OP_KEEP;
        
        if (this.stage === Stage.ENABLED) {
            func = gfx.DS_FUNC_EQUAL;
            mask = this._maskStack[this._maskStack.length - 1];
            if (mask.inverted) {
                ref = this.getInvertedRef();
                stencilMask = this.getStencilRef();
            }
            else {
                ref = this.getStencilRef();
                stencilMask = ref;
            }
            failOp = gfx.STENCIL_OP_KEEP;
            writeMask = 0;

            // clone effect to avoid stencil state pollution
            effect = cloneEffect(effect);
            technique = effect.getTechnique('transparent');
        }
        else {
            func = gfx.DS_FUNC_NEVER;
            failOp = gfx.STENCIL_OP_REPLACE;

            if (this.stage === Stage.ENTER_LEVEL) {
                this.stage = Stage.ENABLED;
                // Fill stencil mask
                ref = this.getStencilRef();
                stencilMask = this.getWriteMask();
                writeMask = stencilMask;
            }
            else if (this.stage === Stage.EXIT_LEVEL) {
                // Pop mask after getting the correct stencil mask
                if (this._maskStack.length === 0) {
                    this.stage = Stage.DISABLED;
                }
                else {
                    this.stage = Stage.ENABLED;
                }
                // Clear stencil mask
                ref = 0;
                stencilMask = this.getExitWriteMask();
                writeMask = stencilMask;
            }
        }
        
        for (let i = 0; i < technique._passes.length; ++i) {
            let pass = technique._passes[i];
            pass.setStencilFront(func, ref, stencilMask, failOp, zFailOp, zPassOp, writeMask);
            pass.setStencilBack(func, ref, stencilMask, failOp, zFailOp, zPassOp, writeMask);
        }
        return effect;
    },

    pushMask (mask) {
        if (this._maskStack.length + 1 > this._maxLevel) {
            cc.errorID(9000, this._maxLevel);
        }
        this._maskStack.push(mask);
        this.stage = Stage.ENTER_LEVEL;
    },

    popMask () {
        if (this._maskStack.length === 0) {
            cc.errorID(9001);
        }
        this._maskStack.pop();
        this.stage = Stage.EXIT_LEVEL;
    },
  
    getWriteMask () {
        return 0x01 << (this._maskStack.length - 1);
    },

    getExitWriteMask () {
        return 0x01 << this._maskStack.length;
    },
  
    getStencilRef () {
        let result = 0;
        for (let i = 0; i < this._maskStack.length; ++i) {
            result += (0x01 << i);
        }
        return result;
    },

    getInvertedRef () {
        let result = 0;
        for (let i = 0; i < this._maskStack.length - 1; ++i) {
            result += (0x01 << i);
        }
        return result;
    }
};

StencilManager.sharedManager = new StencilManager();
StencilManager.Stage = Stage;

module.exports = StencilManager;