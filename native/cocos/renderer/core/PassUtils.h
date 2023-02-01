/****************************************************************************
 Copyright (c) 2021-2023 Xiamen Yaji Software Co., Ltd.
 
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
#include "base/std/container/string.h"
#include "base/std/container/vector.h"
#include "base/std/variant.h"

#include "math/Color.h"
#include "math/Mat3.h"
#include "math/Mat4.h"
#include "math/Quaternion.h"
#include "math/Vec2.h"
#include "math/Vec3.h"
#include "math/Vec4.h"

#include "renderer/gfx-base/GFXDef.h"
#include "renderer/gfx-base/GFXTexture.h"

#include "core/assets/TextureBase.h"

#include "serialization/JsonInputArchive.h"
#include "serialization/SerializationTrait.h"

namespace cc {

class TextureBase;

constexpr uint32_t TYPE_MASK = 0xfc000000;    //  6 bits => 64 types
constexpr uint32_t BINDING_MASK = 0x03f00000; //  6 bits => 64 bindings
constexpr uint32_t COUNT_MASK = 0x000ff000;   //  8 bits => 256 vectors
constexpr uint32_t OFFSET_MASK = 0x00000fff;  // 12 bits => 1024 vectors

constexpr uint32_t genHandle(uint32_t binding, gfx::Type type, uint32_t count, uint32_t offset = 0) {
    return ((static_cast<uint32_t>(type) << 26) & TYPE_MASK) |
           ((binding << 20) & BINDING_MASK) |
           ((count << 12) & COUNT_MASK) |
           (offset & OFFSET_MASK);
}

constexpr gfx::Type getTypeFromHandle(uint32_t handle) { return static_cast<gfx::Type>((handle & TYPE_MASK) >> 26); }
constexpr uint32_t getBindingFromHandle(uint32_t handle) { return (handle & BINDING_MASK) >> 20; }
constexpr uint32_t getCountFromHandle(uint32_t handle) { return (handle & COUNT_MASK) >> 12; }
constexpr uint32_t getOffsetFromHandle(uint32_t handle) { return (handle & OFFSET_MASK); }
constexpr uint32_t customizeType(uint32_t handle, gfx::Type type) {
    return (handle & ~TYPE_MASK) | ((static_cast<uint32_t>(type) << 26) & TYPE_MASK);
}

using MacroValue = ccstd::variant<ccstd::monostate, int32_t, bool, ccstd::string>;

/**
 * @en Combination of preprocess macros
 * @zh 预处理宏组合
 */
using MacroRecord = Record<ccstd::string, MacroValue>;

using MaterialProperty = ccstd::variant<ccstd::monostate /*0*/, float /*1*/, int32_t /*2*/, Vec2 /*3*/, Vec3 /*4*/, Vec4 /*5*/, Color, /*6*/ Mat3 /*7*/, Mat4 /*8*/, Quaternion /*9*/, IntrusivePtr<TextureBase> /*10*/, IntrusivePtr<gfx::Texture> /*11*/>;

using MaterialPropertyList = ccstd::vector<MaterialProperty>;

using MaterialPropertyVariant = ccstd::variant<ccstd::monostate /*0*/, MaterialProperty /*1*/, MaterialPropertyList /*2*/>;

template <>
struct SerializationTrait<MacroValue> : SerializationTraitBase<MacroValue> {
    static void serialize(MacroValue& data, JsonInputArchive& ar) {
        const auto* currentNode = ar.getCurrentNode();
        if (currentNode == nullptr) {
            return;
        }

        if (currentNode->IsNumber()) {
            int32_t v{0};
            SerializationTrait<int32_t>::serialize(v, ar);
            data = v;
        } else if (currentNode->IsBool()) {
            bool v{false};
            SerializationTrait<bool>::serialize(v, ar);
            data = v;
        } else if (currentNode->IsString()) {
            ccstd::string v;
            SerializationTrait<ccstd::string>::serialize(v, ar);
            data = std::move(v);
        } else {
            assert(false);
        }
    }

    static void serialize(MacroValue& data, BinaryInputArchive& ar) {
        assert(false);
    }
};

template <>
struct SerializationTrait<MaterialProperty> : SerializationTraitBase<MaterialProperty> {
    static void serialize(MaterialProperty& data, JsonInputArchive& ar) {
        const auto* currentNode = ar.getCurrentNode();
        if (currentNode == nullptr) {
            return;
        }

        if (currentNode->IsInt() || currentNode->IsUint()) {
            int32_t v{0};
            SerializationTrait<int32_t>::serialize(v, ar);
            data = v;
        } else if (currentNode->IsNumber()) {
            float v{0.F};
            SerializationTrait<float>::serialize(v, ar);
            data = v;
        } else if (currentNode->IsObject()) {
            const auto typeIter = currentNode->FindMember("__type__");
            if (typeIter != currentNode->MemberEnd()) {
                const auto* type = typeIter->value.GetString();
                if (0 == strcmp(type, "cc.Color")) {
                    cc::Color v;
                    SerializationTrait<cc::Color>::serialize(v, ar);
                    data = v;
                } else if (0 == strcmp(type, "cc.Vec4")) {
                    Vec4 v;
                    SerializationTrait<Vec4>::serialize(v, ar);
                    data = v;
                } else if (0 == strcmp(type, "cc.Mat4")) {
                    Mat4 v;
                    SerializationTrait<Mat4>::serialize(v, ar);
                    data = v;
                } else if (0 == strcmp(type, "cc.Vec3")) {
                    Vec3 v;
                    SerializationTrait<Vec3>::serialize(v, ar);
                    data = v;
                } else if (0 == strcmp(type, "cc.Vec2")) {
                    Vec2 v;
                    SerializationTrait<Vec2>::serialize(v, ar);
                    data = v;
                } else if (0 == strcmp(type, "cc.Quat")) {
                    Quaternion v;
                    SerializationTrait<Quaternion>::serialize(v, ar);
                    data = v;
                } else if (0 == strcmp(type, "cc.Mat3")) {
                    Mat3 v;
                    SerializationTrait<Mat3>::serialize(v, ar);
                    data = v;
                }
            } else {
                assert(false); // TODO(cjh):
            }

        } else {
            assert(false);
        }
    }

    static void serialize(MaterialProperty& data, BinaryInputArchive& ar) {
        assert(false);
    }
};

template <>
struct SerializationTrait<MaterialPropertyVariant> : SerializationTraitBase<MaterialPropertyVariant> {
    static void serialize(MaterialPropertyVariant& data, JsonInputArchive& ar) {
        const auto* currentNode = ar.getCurrentNode();
        if (currentNode == nullptr) {
            return;
        }

        if (currentNode->IsArray()) {
            MaterialPropertyList v;
            SerializationTrait<MaterialPropertyList>::serialize(v, ar);
            data = std::move(v);
        } else {
            MaterialProperty v;
            SerializationTrait<MaterialProperty>::serialize(v, ar);
            data = std::move(v);
        }
    }

    static void serialize(MaterialPropertyVariant& data, BinaryInputArchive& ar) {
        assert(false);
    }
};

#define MATERIAL_PROPERTY_INDEX_SINGLE 1
#define MATERIAL_PROPERTY_INDEX_LIST   2

using GFXTypeReaderCallback = void (*)(const float*, MaterialProperty&, index_t);
using GFXTypeWriterCallback = void (*)(float*, const MaterialProperty&, index_t);
using GFXTypeValidatorCallback = bool (*)(const MaterialProperty &);

extern const ccstd::unordered_map<gfx::Type, GFXTypeReaderCallback> type2reader;       //NOLINT(readability-identifier-naming)
extern const ccstd::unordered_map<gfx::Type, GFXTypeWriterCallback> type2writer;       //NOLINT(readability-identifier-naming)
extern const ccstd::unordered_map<gfx::Type, GFXTypeValidatorCallback> type2validator; //NOLINT(readability-identifier-naming)

/**
 * @en Gets the default values for the given type of uniform
 * @zh 根据指定的 Uniform 类型来获取默认值
 * @param type The type of the uniform
 */
const ccstd::vector<float>& getDefaultFloatArrayFromType(gfx::Type type);
const ccstd::string& getDefaultStringFromType(gfx::Type type);
const ccstd::string& getStringFromType(gfx::Type type);

/**
 * @en Combination of preprocess macros
 * @zh 预处理宏组合
 */
/**
 * @en Override the preprocess macros
 * @zh 覆写预处理宏
 * @param target Target preprocess macros to be overridden
 * @param source Preprocess macros used for override
 */
bool overrideMacros(MacroRecord& target, const MacroRecord& source);

MaterialProperty toMaterialProperty(gfx::Type type, const ccstd::vector<float>& vec);

bool macroRecordAsBool(const MacroRecord::mapped_type &v);
ccstd::string macroRecordAsString(const MacroRecord::mapped_type &v);

} // namespace cc
