/****************************************************************************
 Copyright (c) 2019-2023 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

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
****************************************************************************/

#pragma once
#include "base/Ptr.h"
#include "base/Macros.h"
#include "base/TypeDef.h"
#include "renderer/gfx-base/GFXInputAssembler.h"
#include "renderer/gfx-base/GFXDef-common.h"
#include "renderer/gfx-base/GFXBuffer.h"
#include "bindings/utils/BindingUtils.h"

namespace cc {

struct MeshBufferLayout {
    uint32_t byteOffset{0};
    uint32_t vertexOffset{0};
    uint32_t indexOffset{0};
    uint32_t dirtyMark{0};
};

class UIMeshBuffer final {
public:
    UIMeshBuffer();
    ~UIMeshBuffer();

    inline float* getVData() const { return _vData; }
    void setVData(float* vData);
    inline uint16_t* getIData() const { return _iData; }
    void setIData(uint16_t* iData);

    void initialize(ccstd::vector<gfx::Attribute>&& attrs);
    void reset();
    void destroy();
    void setDirty();
    void uploadBuffers();
    void resetIA();
    void recycleIA(gfx::InputAssembler* ia);

    gfx::InputAssembler* requireFreeIA(gfx::Device* device);
    gfx::InputAssembler* createNewIA(gfx::Device* device);

    inline uint32_t getByteOffset() const { return _meshBufferLayout.byteOffset; }
    void setByteOffset(uint32_t byteOffset);
    inline uint32_t getVertexOffset() const { return _meshBufferLayout.vertexOffset; }
    void setVertexOffset(uint32_t vertexOffset);
    inline uint32_t getIndexOffset() const { return _meshBufferLayout.indexOffset; }
    void setIndexOffset(uint32_t indexOffset);
    inline bool getDirty() const { return _meshBufferLayout.dirtyMark != 0; }
    void setDirty(bool dirty);
    inline const ccstd::vector<gfx::Attribute>& getAttributes() const {
        return _attributes;
    }
    
    inline se::Object *_getSharedArrayBufferObject() const { return _sharedMemoryActor.getSharedArrayBufferObject(); } // NOLINT

protected:
    CC_DISALLOW_COPY_MOVE_ASSIGN(UIMeshBuffer);

private:
    float* _vData{nullptr};
    uint16_t* _iData{nullptr};
    cc::bindings::NativeMemorySharedToScriptActor _sharedMemoryActor;

    MeshBufferLayout _meshBufferLayout;

    uint32_t _vertexFormatBytes{0};
    uint32_t _initVDataCount{0};
    uint32_t _initIDataCount{0};

    ccstd::vector<gfx::Attribute> _attributes;
    IntrusivePtr<gfx::InputAssembler> _ia;
    IntrusivePtr<gfx::Buffer> _vb;
    IntrusivePtr<gfx::Buffer> _ib;

    bool _dirty{false};
};
} // namespace cc
