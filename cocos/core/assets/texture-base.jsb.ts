/*
 Copyright (c) 2021 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
  worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
  not use Cocos Creator software for developing other software or tools that's
  used for developing games. You are not granted to publish, distribute,
  sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/
import { ccclass, serializable } from 'cc.decorator';
import {
    _applyDecoratedDescriptor,
    _assertThisInitialized,
    _initializerDefineProperty,
} from '../data/utils/decorator-jsb-utils';
import { legacyCC } from '../global-exports';
import { Filter, PixelFormat, WrapMode } from './asset-enum';

const textureBaseProto: any = jsb.TextureBase.prototype;

textureBaseProto.createNode = null!;

export type TextureBase = jsb.TextureBase;
export const TextureBase = jsb.TextureBase;

const clsDecorator = ccclass('cc.TextureBase');

const _class2$b = TextureBase;
const _descriptor$9 = _applyDecoratedDescriptor(_class2$b.prototype, '_format', [serializable], {
    configurable: true,
    enumerable: true,
    writable: true,
    initializer: function initializer () {
        return PixelFormat.RGBA8888;
    },
});
const _descriptor2$6 = _applyDecoratedDescriptor(_class2$b.prototype, '_minFilter', [serializable], {
    configurable: true,
    enumerable: true,
    writable: true,
    initializer: function initializer () {
        return Filter.LINEAR;
    },
});
const _descriptor3$5 = _applyDecoratedDescriptor(_class2$b.prototype, '_magFilter', [serializable], {
    configurable: true,
    enumerable: true,
    writable: true,
    initializer: function initializer () {
        return Filter.LINEAR;
    },
});
const _descriptor4$4 = _applyDecoratedDescriptor(_class2$b.prototype, '_mipFilter', [serializable], {
    configurable: true,
    enumerable: true,
    writable: true,
    initializer: function initializer () {
        return Filter.NONE;
    },
});
const _descriptor5$3 = _applyDecoratedDescriptor(_class2$b.prototype, '_wrapS', [serializable], {
    configurable: true,
    enumerable: true,
    writable: true,
    initializer: function initializer () {
        return WrapMode.REPEAT;
    },
});
const _descriptor6$1 = _applyDecoratedDescriptor(_class2$b.prototype, '_wrapT', [serializable], {
    configurable: true,
    enumerable: true,
    writable: true,
    initializer: function initializer () {
        return WrapMode.REPEAT;
    },
});
const _descriptor7$1 = _applyDecoratedDescriptor(_class2$b.prototype, '_wrapR', [serializable], {
    configurable: true,
    enumerable: true,
    writable: true,
    initializer: function initializer () {
        return WrapMode.REPEAT;
    },
});
const  _descriptor8$1 = _applyDecoratedDescriptor(_class2$b.prototype, '_anisotropy', [serializable], {
    configurable: true,
    enumerable: true,
    writable: true,
    initializer: function initializer () {
        return 0;
    },
});
textureBaseProto._ctor = function () {
    // for deserialization
    // _initializerDefineProperty(_this, "_format", _descriptor$9, _assertThisInitialized(_this));
    // _initializerDefineProperty(_this, "_minFilter", _descriptor2$6, _assertThisInitialized(_this));
    // _initializerDefineProperty(_this, "_magFilter", _descriptor3$5, _assertThisInitialized(_this));
    // _initializerDefineProperty(_this, "_mipFilter", _descriptor4$4, _assertThisInitialized(_this));
    // _initializerDefineProperty(_this, "_wrapS", _descriptor5$3, _assertThisInitialized(_this));
    // _initializerDefineProperty(_this, "_wrapT", _descriptor6$1, _assertThisInitialized(_this));
    // _initializerDefineProperty(_this, "_wrapR", _descriptor7$1, _assertThisInitialized(_this));
    // _initializerDefineProperty(_this, "_anisotropy", _descriptor8$1, _assertThisInitialized(_this));
};

clsDecorator(TextureBase);

legacyCC.TextureBase = jsb.TextureBase;
