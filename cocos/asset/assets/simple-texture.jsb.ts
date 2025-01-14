/*
 Copyright (c) 2021-2023 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/
import { TextureFilter, PixelFormat, WrapMode } from './asset-enum';
import dependUtil from '../asset-manager/depend-util';
import { js, macro, cclegacy, ccenum } from '../../core';
import { Format, TextureInfo, TextureViewInfo } from '../../gfx';
import { patch_cc_SimpleTexture } from '../../native-binding/decorators';
import type { SimpleTexture as JsbSimpleTexture } from './simple-texture';

declare const jsb: any;

export type PresumedGFXTextureInfo = Pick<TextureInfo, 'usage' | 'flags' | 'format' | 'levelCount'>;
export type PresumedGFXTextureViewInfo = Pick<TextureViewInfo, 'texture' | 'format' | 'baseLevel' | 'levelCount'>;

export type SimpleTexture = JsbSimpleTexture;
export const SimpleTexture: typeof JsbSimpleTexture = jsb.SimpleTexture;

const jsbWindow = jsb.window;

ccenum(Format);

SimpleTexture.Filter = TextureFilter;
SimpleTexture.PixelFormat = PixelFormat;
SimpleTexture.WrapMode = WrapMode;

const simpleTextureProto = SimpleTexture.prototype;
const oldUpdateDataFunc = simpleTextureProto.uploadData;
simpleTextureProto.uploadData = function (source, level = 0, arrayIndex = 0) {
    let data;
    if (source instanceof jsbWindow.HTMLCanvasElement) {
        // @ts-ignore
        data = source.data;
    } else if (source instanceof jsbWindow.HTMLImageElement) {
        // @ts-ignore
        data = source._data;
    } else if (ArrayBuffer.isView(source)) {
        data = source.buffer;
    }
    oldUpdateDataFunc.call(this, data, level, arrayIndex);
};

// @ts-ignore
simpleTextureProto._ctor = function () {
    jsb.TextureBase.prototype._ctor.apply(this, arguments);
    this._gfxTexture = null;
    this._registerListeners();
};

const oldGetGFXTexture = simpleTextureProto.getGFXTexture;
simpleTextureProto.getGFXTexture = function () {
    if (!this._gfxTexture) {
        this._gfxTexture = oldGetGFXTexture.call(this);
    }
    return this._gfxTexture;
};

// @ts-ignore
simpleTextureProto._onGFXTextureUpdated = function (gfxTexture) {
    this._gfxTexture = gfxTexture;
};

// @ts-ignore
simpleTextureProto._onAfterAssignImage = function (image) {
    if (macro.CLEANUP_IMAGE_CACHE) {
        const deps = dependUtil.getDeps(this._uuid);
        const index = deps.indexOf(image._uuid);
        if (index !== -1) {
            js.array.fastRemoveAt(deps, index);
            image.decRef();
        }
    }
};

patch_cc_SimpleTexture({SimpleTexture});

cclegacy.SimpleTexture = jsb.SimpleTexture;

