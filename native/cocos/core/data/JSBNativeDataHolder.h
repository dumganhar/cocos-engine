/****************************************************************************
 Copyright (c) 2022 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

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
****************************************************************************/

#pragma once

#include <cstdint>
#include "base/Utils.h"

namespace cc {
class JSBNativeDataHolder final {
public:
    JSBNativeDataHolder() = default;
    explicit JSBNativeDataHolder(uint8_t* data, uint32_t byteLength) : _data(data), _byteLength(byteLength){};

    ~JSBNativeDataHolder() {
        destroy();
    }

    inline void setData(uint8_t* data) { _data = data; }
    inline uint8_t* getData() const { return _data; }

    inline void destroy() { // Also support to invoke destroy method to free memory before garbage collection
        if (_data != nullptr) {
            static uint32_t releasedBytes = 0;
            releasedBytes += _byteLength;
            printf("cjh data released: %u\n", releasedBytes);
            free(_data);
            _data = nullptr;
            _byteLength = 0;
        }
    }

private:
    uint8_t* _data{nullptr};
    uint32_t _byteLength{0};
};
} // namespace cc
