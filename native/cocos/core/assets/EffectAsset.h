/****************************************************************************
 Copyright (c) 2021 Xiamen Yaji Software Co., Ltd.

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

#include <tuple>
#include "base/std/container/string.h"
#include "base/std/container/unordered_map.h"
#include "base/std/optional.h"

#include "base/Value.h"
#include "core/Types.h"
#include "core/assets/Asset.h"
#include "renderer/core/PassUtils.h"
#include "renderer/gfx-base/GFXDef.h"
#include "renderer/pipeline/Define.h"

#include "serialization/SerializationTrait.h"

namespace cc {

using IPropertyHandleInfo = std::tuple<ccstd::string, uint32_t, gfx::Type>;

using IPropertyValueInternal = ccstd::variant<ccstd::vector<float>, ccstd::string>;
using IPropertyValue = ccstd::optional<IPropertyValueInternal>;

struct IPropertyInfo {
    int32_t type{0};                                 // auto-extracted from shader
    ccstd::optional<IPropertyHandleInfo> handleInfo; // auto-generated from 'target'
    ccstd::optional<ccstd::hash_t> samplerHash;      // auto-generated from 'sampler'
    IPropertyValue value;                            // default value
    ccstd::optional<bool> linear;                    // whether to convert the input to linear space first before applying

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(type);
        CC_SERIALIZE(handleInfo);
        CC_SERIALIZE(samplerHash);
        CC_SERIALIZE(value);
        CC_SERIALIZE(linear);
    }
};

struct IPassInfoFull;

struct RasterizerStateInfo {
    ccstd::optional<bool> isDiscard;
    ccstd::optional<bool> isFrontFaceCCW;
    ccstd::optional<bool> depthBiasEnabled;
    ccstd::optional<bool> isDepthClip;
    ccstd::optional<bool> isMultisample;

    ccstd::optional<gfx::PolygonMode> polygonMode;
    ccstd::optional<gfx::ShadeModel> shadeModel;
    ccstd::optional<gfx::CullMode> cullMode;

    ccstd::optional<float> depthBias;
    ccstd::optional<float> depthBiasClamp;
    ccstd::optional<float> depthBiasSlop;
    ccstd::optional<float> lineWidth;

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(isDiscard);
        CC_SERIALIZE(isFrontFaceCCW);
        CC_SERIALIZE(depthBiasEnabled);
        CC_SERIALIZE(isDepthClip);
        CC_SERIALIZE(isMultisample);

        CC_SERIALIZE(polygonMode);
        CC_SERIALIZE(shadeModel);
        CC_SERIALIZE(cullMode);

        CC_SERIALIZE(depthBias);
        CC_SERIALIZE(depthBiasClamp);
        CC_SERIALIZE(depthBiasSlop);
        CC_SERIALIZE(lineWidth);
    }

    void fromGFXRasterizerState(const gfx::RasterizerState &rs) {
        isDiscard = rs.isDiscard;
        isFrontFaceCCW = rs.isFrontFaceCCW;
        depthBiasEnabled = rs.depthBiasEnabled;
        isDepthClip = rs.isDepthClip;
        isMultisample = rs.isMultisample;

        polygonMode = rs.polygonMode;
        shadeModel = rs.shadeModel;
        cullMode = rs.cullMode;

        depthBias = rs.depthBias;
        depthBiasClamp = rs.depthBiasClamp;
        depthBiasSlop = rs.depthBiasSlop;
        lineWidth = rs.lineWidth;
    }

    void assignToGFXRasterizerState(gfx::RasterizerState &rs) const {
        if (isDiscard.has_value()) {
            rs.isDiscard = isDiscard.value();
        }
        if (isFrontFaceCCW.has_value()) {
            rs.isFrontFaceCCW = isFrontFaceCCW.value();
        }
        if (depthBiasEnabled.has_value()) {
            rs.depthBiasEnabled = depthBiasEnabled.value();
        }
        if (isDepthClip.has_value()) {
            rs.isDepthClip = isDepthClip.value();
        }
        if (isMultisample.has_value()) {
            rs.isMultisample = isMultisample.value();
        }
        if (polygonMode.has_value()) {
            rs.polygonMode = polygonMode.value();
        }
        if (shadeModel.has_value()) {
            rs.shadeModel = shadeModel.value();
        }
        if (cullMode.has_value()) {
            rs.cullMode = cullMode.value();
        }
        if (depthBias.has_value()) {
            rs.depthBias = depthBias.value();
        }
        if (depthBiasClamp.has_value()) {
            rs.depthBiasClamp = depthBiasClamp.value();
        }
        if (depthBiasSlop.has_value()) {
            rs.depthBiasSlop = depthBiasSlop.value();
        }
        if (lineWidth.has_value()) {
            rs.lineWidth = lineWidth.value();
        }
    }
};

struct DepthStencilStateInfo {
    ccstd::optional<bool> depthTest;
    ccstd::optional<bool> depthWrite;
    ccstd::optional<bool> stencilTestFront;
    ccstd::optional<bool> stencilTestBack;

    ccstd::optional<gfx::ComparisonFunc> depthFunc;
    ccstd::optional<gfx::ComparisonFunc> stencilFuncFront;
    ccstd::optional<uint32_t> stencilReadMaskFront;
    ccstd::optional<uint32_t> stencilWriteMaskFront;
    ccstd::optional<gfx::StencilOp> stencilFailOpFront;
    ccstd::optional<gfx::StencilOp> stencilZFailOpFront;
    ccstd::optional<gfx::StencilOp> stencilPassOpFront;
    ccstd::optional<uint32_t> stencilRefFront;

    ccstd::optional<gfx::ComparisonFunc> stencilFuncBack;
    ccstd::optional<uint32_t> stencilReadMaskBack;
    ccstd::optional<uint32_t> stencilWriteMaskBack;
    ccstd::optional<gfx::StencilOp> stencilFailOpBack;
    ccstd::optional<gfx::StencilOp> stencilZFailOpBack;
    ccstd::optional<gfx::StencilOp> stencilPassOpBack;
    ccstd::optional<uint32_t> stencilRefBack;

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(depthTest);
        CC_SERIALIZE(depthWrite);
        CC_SERIALIZE(stencilTestFront);
        CC_SERIALIZE(stencilTestBack);

        CC_SERIALIZE(depthFunc);
        CC_SERIALIZE(stencilFuncFront);
        CC_SERIALIZE(stencilReadMaskFront);
        CC_SERIALIZE(stencilWriteMaskFront);
        CC_SERIALIZE(stencilFailOpFront);
        CC_SERIALIZE(stencilZFailOpFront);
        CC_SERIALIZE(stencilPassOpFront);
        CC_SERIALIZE(stencilRefFront);

        CC_SERIALIZE(stencilFuncBack);
        CC_SERIALIZE(stencilReadMaskBack);
        CC_SERIALIZE(stencilWriteMaskBack);
        CC_SERIALIZE(stencilFailOpBack);
        CC_SERIALIZE(stencilZFailOpBack);
        CC_SERIALIZE(stencilPassOpBack);
        CC_SERIALIZE(stencilRefBack);
    }

    void fromGFXDepthStencilState(const gfx::DepthStencilState &ds) {
        depthTest = ds.depthTest;
        depthWrite = ds.depthWrite;
        stencilTestFront = ds.stencilTestFront;
        stencilTestBack = ds.stencilTestBack;

        depthFunc = ds.depthFunc;
        stencilFuncFront = ds.stencilFuncFront;
        stencilReadMaskFront = ds.stencilReadMaskFront;
        stencilWriteMaskFront = ds.stencilWriteMaskFront;
        stencilFailOpFront = ds.stencilFailOpFront;
        stencilZFailOpFront = ds.stencilZFailOpFront;
        stencilPassOpFront = ds.stencilPassOpFront;
        stencilRefFront = ds.stencilRefFront;

        stencilFuncBack = ds.stencilFuncBack;
        stencilReadMaskBack = ds.stencilReadMaskBack;
        stencilWriteMaskBack = ds.stencilWriteMaskBack;
        stencilFailOpBack = ds.stencilFailOpBack;
        stencilZFailOpBack = ds.stencilZFailOpBack;
        stencilPassOpBack = ds.stencilPassOpBack;
        stencilRefBack = ds.stencilRefBack;
    }

    void assignToGFXDepthStencilState(gfx::DepthStencilState &ds) const {
        if (depthTest.has_value()) {
            ds.depthTest = depthTest.value();
        }
        if (depthWrite.has_value()) {
            ds.depthWrite = depthWrite.value();
        }
        if (stencilTestFront.has_value()) {
            ds.stencilTestFront = stencilTestFront.value();
        }
        if (stencilTestBack.has_value()) {
            ds.stencilTestBack = stencilTestBack.value();
        }
        if (depthFunc.has_value()) {
            ds.depthFunc = depthFunc.value();
        }
        if (stencilFuncFront.has_value()) {
            ds.stencilFuncFront = stencilFuncFront.value();
        }
        if (stencilReadMaskFront.has_value()) {
            ds.stencilReadMaskFront = stencilReadMaskFront.value();
        }
        if (stencilWriteMaskFront.has_value()) {
            ds.stencilWriteMaskFront = stencilWriteMaskFront.value();
        }
        if (stencilFailOpFront.has_value()) {
            ds.stencilFailOpFront = stencilFailOpFront.value();
        }
        if (stencilZFailOpFront.has_value()) {
            ds.stencilZFailOpFront = stencilZFailOpFront.value();
        }
        if (stencilPassOpFront.has_value()) {
            ds.stencilPassOpFront = stencilPassOpFront.value();
        }
        if (stencilRefFront.has_value()) {
            ds.stencilRefFront = stencilRefFront.value();
        }
        if (stencilFuncBack.has_value()) {
            ds.stencilFuncBack = stencilFuncBack.value();
        }
        if (stencilReadMaskBack.has_value()) {
            ds.stencilReadMaskBack = stencilReadMaskBack.value();
        }
        if (stencilWriteMaskBack.has_value()) {
            ds.stencilWriteMaskBack = stencilWriteMaskBack.value();
        }
        if (stencilFailOpBack.has_value()) {
            ds.stencilFailOpBack = stencilFailOpBack.value();
        }
        if (stencilZFailOpBack.has_value()) {
            ds.stencilZFailOpBack = stencilZFailOpBack.value();
        }
        if (stencilPassOpBack.has_value()) {
            ds.stencilPassOpBack = stencilPassOpBack.value();
        }
        if (stencilRefBack.has_value()) {
            ds.stencilRefBack = stencilRefBack.value();
        }
    }
};

struct BlendTargetInfo {
    ccstd::optional<bool> blend;
    ccstd::optional<gfx::BlendFactor> blendSrc;
    ccstd::optional<gfx::BlendFactor> blendDst;
    ccstd::optional<gfx::BlendOp> blendEq;
    ccstd::optional<gfx::BlendFactor> blendSrcAlpha;
    ccstd::optional<gfx::BlendFactor> blendDstAlpha;
    ccstd::optional<gfx::BlendOp> blendAlphaEq;
    ccstd::optional<gfx::ColorMask> blendColorMask;

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(blend);
        CC_SERIALIZE(blendSrc);
        CC_SERIALIZE(blendDst);
        CC_SERIALIZE(blendEq);
        CC_SERIALIZE(blendSrcAlpha);
        CC_SERIALIZE(blendDstAlpha);
        CC_SERIALIZE(blendAlphaEq);
        CC_SERIALIZE(blendColorMask);
    }

    void fromGFXBlendTarget(const gfx::BlendTarget &target) {
        blend = target.blend;
        blendSrc = target.blendSrc;
        blendDst = target.blendDst;
        blendEq = target.blendEq;
        blendSrcAlpha = target.blendSrcAlpha;
        blendDstAlpha = target.blendDstAlpha;
        blendAlphaEq = target.blendAlphaEq;
        blendColorMask = target.blendColorMask;
    }

    void assignToGFXBlendTarget(gfx::BlendTarget &target) const {
        if (blend.has_value()) {
            target.blend = blend.value();
        }
        if (blendSrc.has_value()) {
            target.blendSrc = blendSrc.value();
        }
        if (blendDst.has_value()) {
            target.blendDst = blendDst.value();
        }
        if (blendEq.has_value()) {
            target.blendEq = blendEq.value();
        }
        if (blendSrcAlpha.has_value()) {
            target.blendSrcAlpha = blendSrcAlpha.value();
        }
        if (blendDstAlpha.has_value()) {
            target.blendDstAlpha = blendDstAlpha.value();
        }
        if (blendAlphaEq.has_value()) {
            target.blendAlphaEq = blendAlphaEq.value();
        }
        if (blendColorMask.has_value()) {
            target.blendColorMask = blendColorMask.value();
        }
    }
};

using BlendTargetInfoList = ccstd::vector<BlendTargetInfo>;

struct BlendStateInfo {
    ccstd::optional<bool> isA2C;
    ccstd::optional<bool> isIndepend;
    ccstd::optional<gfx::Color> blendColor;
    ccstd::optional<BlendTargetInfoList> targets;

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(isA2C);
        CC_SERIALIZE(isIndepend);
        CC_SERIALIZE(blendColor);
        CC_SERIALIZE(targets);
    }

    void fromGFXBlendState(const gfx::BlendState &bs) {
        isA2C = bs.isA2C;
        isIndepend = bs.isIndepend;
        blendColor = bs.blendColor;
        size_t len = bs.targets.size();
        if (len > 0) {
            BlendTargetInfoList targetsList(len);
            for (size_t i = 0; i < len; ++i) {
                targetsList[i].fromGFXBlendTarget(bs.targets[i]);
            }
            targets = targetsList;
        }
    }

    void assignToGFXBlendState(gfx::BlendState &bs) const {
        if (targets.has_value()) {
            const auto &targetsVal = targets.value();
            bs.targets.resize(targetsVal.size());
            for (size_t i = 0, len = targetsVal.size(); i < len; ++i) {
                targetsVal[i].assignToGFXBlendTarget(bs.targets[i]);
            }
        }

        if (isA2C.has_value()) {
            bs.isA2C = isA2C.value();
        }

        if (isIndepend.has_value()) {
            bs.isIndepend = isIndepend.value();
        }

        if (blendColor.has_value()) {
            bs.blendColor = blendColor.value();
        }
    }
};

// Pass instance itself are compliant to IPassStates too
struct IPassStates {
    ccstd::optional<int32_t> priority;
    ccstd::optional<gfx::PrimitiveMode> primitive;
    ccstd::optional<pipeline::RenderPassStage> stage;
    ccstd::optional<RasterizerStateInfo> rasterizerState;
    ccstd::optional<DepthStencilStateInfo> depthStencilState;
    ccstd::optional<BlendStateInfo> blendState;
    ccstd::optional<gfx::DynamicStateFlags> dynamicStates;
    ccstd::optional<ccstd::string> phase;

    IPassStates() = default;
    explicit IPassStates(const IPassInfoFull &o);
    IPassStates &operator=(const IPassInfoFull &o);
    void overrides(const IPassInfoFull &o);

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(priority);
        CC_SERIALIZE(primitive);
        CC_SERIALIZE(stage);
        CC_SERIALIZE(rasterizerState);
        CC_SERIALIZE(depthStencilState);
        CC_SERIALIZE(blendState);
        CC_SERIALIZE(dynamicStates);
        CC_SERIALIZE(phase);
    }
};
using PassOverrides = IPassStates;

using PassPropertyInfoMap = ccstd::unordered_map<ccstd::string, IPropertyInfo>;

struct IPassInfoFull final { // cjh } : public IPassInfo {
    // IPassStates
    ccstd::optional<int32_t> priority;
    ccstd::optional<gfx::PrimitiveMode> primitive;
    ccstd::optional<pipeline::RenderPassStage> stage;
    ccstd::optional<RasterizerStateInfo> rasterizerState;
    ccstd::optional<DepthStencilStateInfo> depthStencilState;
    ccstd::optional<BlendStateInfo> blendState;
    ccstd::optional<gfx::DynamicStateFlags> dynamicStates;
    ccstd::optional<ccstd::string> phase;
    // IPassInfo
    ccstd::string program; // auto-generated from 'vert' and 'frag'
    ccstd::optional<MacroRecord> embeddedMacros;
    index_t propertyIndex{CC_INVALID_INDEX};
    ccstd::optional<ccstd::string> switch_;
    ccstd::optional<PassPropertyInfoMap> properties;

    // IPassInfoFull
    // generated part
    index_t passIndex{0};
    MacroRecord defines;
    ccstd::optional<PassOverrides> stateOverrides;

    IPassInfoFull() = default;
    explicit IPassInfoFull(const IPassStates &o) {
        *this = o;
    }
    IPassInfoFull &operator=(const IPassStates &o) {
        priority = o.priority;
        primitive = o.primitive;
        stage = o.stage;
        rasterizerState = o.rasterizerState;
        depthStencilState = o.depthStencilState;
        blendState = o.blendState;
        dynamicStates = o.dynamicStates;
        phase = o.phase;
        return *this;
    }

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(priority);
        CC_SERIALIZE(primitive);
        CC_SERIALIZE(stage);
        CC_SERIALIZE(rasterizerState);
        CC_SERIALIZE(depthStencilState);
        CC_SERIALIZE(blendState);
        CC_SERIALIZE(dynamicStates);
        CC_SERIALIZE(phase);
        CC_SERIALIZE(program);
        CC_SERIALIZE(embeddedMacros);
        CC_SERIALIZE(propertyIndex);
        ar.serialize(switch_, "switch");
        CC_SERIALIZE(properties);
        CC_SERIALIZE(passIndex);
        CC_SERIALIZE(defines);
        CC_SERIALIZE(stateOverrides);
    }
};

using IPassInfo = IPassInfoFull;

struct ITechniqueInfo {
    ccstd::vector<IPassInfoFull> passes;
    ccstd::optional<ccstd::string> name;

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(passes);
        CC_SERIALIZE(name);
    }
};

struct IBlockInfo {
    uint32_t binding{UINT32_MAX};
    ccstd::string name;
    ccstd::vector<gfx::Uniform> members;
    gfx::ShaderStageFlags stageFlags{gfx::ShaderStageFlags::NONE};

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(binding);
        CC_SERIALIZE(name);
        CC_SERIALIZE(members);
        CC_SERIALIZE(stageFlags);
    }
};

struct ISamplerTextureInfo {
    uint32_t binding{UINT32_MAX};
    ccstd::string name;
    gfx::Type type{gfx::Type::UNKNOWN};
    uint32_t count{0};
    gfx::ShaderStageFlags stageFlags{gfx::ShaderStageFlags::NONE};

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(binding);
        CC_SERIALIZE(name);
        CC_SERIALIZE(type);
        CC_SERIALIZE(count);
        CC_SERIALIZE(stageFlags);
    }
};

struct ITextureInfo {
    uint32_t set{0};
    uint32_t binding{UINT32_MAX};
    ccstd::string name;
    gfx::Type type{gfx::Type::UNKNOWN};
    uint32_t count{0};
    gfx::ShaderStageFlags stageFlags{gfx::ShaderStageFlags::NONE};

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(set);
        CC_SERIALIZE(binding);
        CC_SERIALIZE(name);
        CC_SERIALIZE(type);
        CC_SERIALIZE(count);
        CC_SERIALIZE(stageFlags);
    }
};

struct ISamplerInfo {
    uint32_t set{0};
    uint32_t binding{UINT32_MAX};
    ccstd::string name;
    uint32_t count{0};
    gfx::ShaderStageFlags stageFlags{gfx::ShaderStageFlags::NONE};

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(set);
        CC_SERIALIZE(binding);
        CC_SERIALIZE(name);
        CC_SERIALIZE(count);
        CC_SERIALIZE(stageFlags);
    }
};

struct IBufferInfo {
    uint32_t binding{UINT32_MAX};
    ccstd::string name;
    gfx::MemoryAccess memoryAccess{gfx::MemoryAccess::NONE};
    gfx::ShaderStageFlags stageFlags{gfx::ShaderStageFlags::NONE};

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(binding);
        CC_SERIALIZE(name);
        CC_SERIALIZE(memoryAccess);
        CC_SERIALIZE(stageFlags);
    }
};

struct IImageInfo {
    uint32_t binding{UINT32_MAX};
    ccstd::string name;
    gfx::Type type{gfx::Type::UNKNOWN};
    uint32_t count{0};
    gfx::MemoryAccess memoryAccess{gfx::MemoryAccess::NONE};
    gfx::ShaderStageFlags stageFlags{gfx::ShaderStageFlags::NONE};

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(binding);
        CC_SERIALIZE(name);
        CC_SERIALIZE(type);
        CC_SERIALIZE(count);
        CC_SERIALIZE(memoryAccess);
        CC_SERIALIZE(stageFlags);
    }
};

struct IInputAttachmentInfo {
    uint32_t set{0};
    uint32_t binding{UINT32_MAX};
    ccstd::string name;
    uint32_t count{0};
    gfx::ShaderStageFlags stageFlags{gfx::ShaderStageFlags::NONE};

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(set);
        CC_SERIALIZE(binding);
        CC_SERIALIZE(name);
        CC_SERIALIZE(count);
        CC_SERIALIZE(stageFlags);
    }
};

struct IAttributeInfo {
    ccstd::string name;
    gfx::Format format{gfx::Format::UNKNOWN};
    bool isNormalized{false};
    uint32_t stream{0U};
    bool isInstanced{false};
    uint32_t location{0U};

    ccstd::vector<ccstd::string> defines;

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(name);
        CC_SERIALIZE(format);
        CC_SERIALIZE(isNormalized);
        CC_SERIALIZE(stream);
        CC_SERIALIZE(isInstanced);
        CC_SERIALIZE(location);
        CC_SERIALIZE(defines);
    }
};

struct IDefineInfo {
    ccstd::string name;
    ccstd::string type;
    ccstd::optional<ccstd::vector<int32_t>> range; // cjh number is float?  ?: number[];
    ccstd::optional<ccstd::vector<ccstd::string>> options;
    ccstd::optional<ccstd::string> defaultVal;

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(name);
        CC_SERIALIZE(type);
        CC_SERIALIZE(range);
        CC_SERIALIZE(options);
        CC_SERIALIZE(defaultVal);
    }
};

struct IBuiltin {
    ccstd::string name;
    ccstd::vector<ccstd::string> defines;

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(name);
        CC_SERIALIZE(defines);
    }
};

struct IBuiltinInfo {
    ccstd::vector<IBuiltin> buffers;
    ccstd::vector<IBuiltin> blocks;
    ccstd::vector<IBuiltin> samplerTextures;
    ccstd::vector<IBuiltin> images;

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(buffers);
        CC_SERIALIZE(blocks);
        CC_SERIALIZE(samplerTextures);
        CC_SERIALIZE(images);
    }
};

using BuiltinsStatisticsType = ccstd::unordered_map<ccstd::string, int32_t>;

struct IBuiltins {
    IBuiltinInfo globals;
    IBuiltinInfo locals;
    BuiltinsStatisticsType statistics;

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(globals);
        CC_SERIALIZE(locals);
        CC_SERIALIZE(statistics);
    }
};

struct IShaderSource {
    ccstd::string vert;
    ccstd::string frag;

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(vert);
        CC_SERIALIZE(frag);
    }
};

struct IShaderInfo {
    ccstd::string name;
    ccstd::hash_t hash{0xFFFFFFFFU};
    IShaderSource glsl4;
    IShaderSource glsl3;
    IShaderSource glsl1;
    IBuiltins builtins;
    ccstd::vector<IDefineInfo> defines;
    ccstd::vector<IAttributeInfo> attributes;
    ccstd::vector<IBlockInfo> blocks;
    ccstd::vector<ISamplerTextureInfo> samplerTextures;
    ccstd::vector<ISamplerInfo> samplers;
    ccstd::vector<ITextureInfo> textures;
    ccstd::vector<IBufferInfo> buffers;
    ccstd::vector<IImageInfo> images;
    ccstd::vector<IInputAttachmentInfo> subpassInputs;

    const IShaderSource *getSource(const ccstd::string &version) const {
        if (version == "glsl1") return &glsl1;
        if (version == "glsl3") return &glsl3;
        if (version == "glsl4") return &glsl4;
        return nullptr;
    }

    template <class Archive>
    void serializeInlineData(Archive &ar) {
        CC_SERIALIZE(name);
        CC_SERIALIZE(hash);
        CC_SERIALIZE(glsl4);
        CC_SERIALIZE(glsl3);
        CC_SERIALIZE(glsl1);
        CC_SERIALIZE(builtins);
        CC_SERIALIZE(defines);
        CC_SERIALIZE(attributes);
        CC_SERIALIZE(blocks);
        CC_SERIALIZE(samplerTextures);
        CC_SERIALIZE(samplers);
        CC_SERIALIZE(textures);
        CC_SERIALIZE(buffers);
        CC_SERIALIZE(images);
        CC_SERIALIZE(subpassInputs);
    }
};

using IPreCompileInfoValueType = ccstd::variant<ccstd::vector<bool>, ccstd::vector<int32_t>, ccstd::vector<ccstd::string>>;
using IPreCompileInfo = ccstd::unordered_map<ccstd::string, IPreCompileInfoValueType>;

class EffectAsset final : public Asset {
    CC_DECLARE_SERIALIZE()
public:
    using Super = Asset;

    EffectAsset() = default;
    ~EffectAsset() override = default;
    /**
     * @en Register the effect asset to the static map
     * @zh 将指定 effect 注册到全局管理器。
     */
    static void registerAsset(EffectAsset *asset);

    /**
     * @en Unregister the effect asset from the static map
     * @zh 将指定 effect 从全局管理器移除。
     */
    static void remove(const ccstd::string &name);
    static void remove(EffectAsset *asset);

    /**
     * @en Get the effect asset by the given name.
     * @zh 获取指定名字的 effect 资源。
     */
    static EffectAsset *get(const ccstd::string &name);

    using RegisteredEffectAssetMap = ccstd::unordered_map<ccstd::string, IntrusivePtr<EffectAsset>>;
    /**
     * @en Get all registered effect assets.
     * @zh 获取所有已注册的 effect 资源。
     */
    static RegisteredEffectAssetMap &getAll() { return EffectAsset::effects; }

    static bool isLayoutValid() { return layoutValid; }
    static void setLayoutValid() { layoutValid = true; }

    inline void setTechniques(const ccstd::vector<ITechniqueInfo> &val) { _techniques = val; }
    inline void setShaders(const ccstd::vector<IShaderInfo> &val) { _shaders = val; }
    inline void setCombinations(const ccstd::vector<IPreCompileInfo> &val) { _combinations = val; }

    inline const ccstd::vector<ITechniqueInfo> &getTechniques() const { return _techniques; }
    inline const ccstd::vector<IShaderInfo> &getShaders() const { return _shaders; }
    inline const ccstd::vector<IPreCompileInfo> &getCombinations() const { return _combinations; }

    /*
    @serializable
    @editorOnly
     */
    bool hideInEditor = false;

    /**
     * @en The loaded callback which should be invoked by the [[Loader]], will automatically register the effect.
     * @zh 通过 [[Loader]] 加载完成时的回调，将自动注册 effect 资源。
     */
    void onLoaded() override;
    bool destroy() override;
    void initDefault(const ccstd::optional<ccstd::string> &uuid) override;
    bool validate() const override;

protected:
    static ccstd::vector<MacroRecord> doCombine(const ccstd::vector<MacroRecord> &cur, const IPreCompileInfo &info, IPreCompileInfo::iterator iter);
    static ccstd::vector<MacroRecord> generateRecords(const ccstd::string &key, const IPreCompileInfoValueType &value);
    static ccstd::vector<MacroRecord> insertInfoValue(const ccstd::vector<MacroRecord> &records,
                                                      const ccstd::string &key,
                                                      const IPreCompileInfoValueType &value);

    void precompile();

    // We need make it to public for deserialization
public:
    /**
     * @en The techniques used by the current effect.
     * @zh 当前 effect 的所有可用 technique。

    @serializable
    @editable*/
    ccstd::vector<ITechniqueInfo> _techniques;

    /**
     * @en The shaders used by the current effect.
     * @zh 当前 effect 使用的所有 shader。

    @serializable
    @editable*/
    ccstd::vector<IShaderInfo> _shaders;

    /**
     * @en The preprocess macro combinations for the shader
     * @zh 每个 shader 需要预编译的宏定义组合。

    @serializable
    @editable*/
    ccstd::vector<IPreCompileInfo> _combinations;
    //
protected:
    static RegisteredEffectAssetMap effects; // cjh TODO: how to clear when game exits.
    static bool layoutValid;

    CC_DISALLOW_COPY_MOVE_ASSIGN(EffectAsset);

    friend class EffectAssetDeserializer;
    friend class Material;
    friend class ProgramLib;
    friend class MaterialInstance;
    friend class BuiltinResMgr;
};

template <>
struct SerializationTrait<IPropertyValueInternal> : SerializationTraitBase<IPropertyValueInternal> {
    static void serialize(IPropertyValueInternal &data, JsonInputArchive &ar) {
        const auto *currentNode = ar.getCurrentNode();
        if (currentNode == nullptr) {
            return;
        }

        if (currentNode->IsArray()) {
            ccstd::vector<float> arr;
            SerializationTrait<ccstd::vector<float>>::serialize(arr, ar);
            data = std::move(arr);
        } else if (currentNode->IsString()) {
            ccstd::string str;
            SerializationTrait<ccstd::string>::serialize(str, ar);
            data = std::move(str);
        } else {
            assert(false);
        }
    }

    static void serialize(MacroValue &data, BinaryInputArchive &ar) {
    }
};

template <>
struct SerializationTrait<IPreCompileInfoValueType> : SerializationTraitBase<IPreCompileInfoValueType> {
    static void serialize(IPreCompileInfoValueType &data, JsonInputArchive &ar) {
        const auto *currentNode = ar.getCurrentNode();
        if (currentNode == nullptr) {
            return;
        }

        if (currentNode->IsArray()) {
            const auto &jsonArr = currentNode->GetArray();
            if (jsonArr.Size() > 0) {
                const auto &first = jsonArr[0];
                if (first.IsBool()) {
                    ccstd::vector<bool> arr;
                    SerializationTrait<ccstd::vector<bool>>::serialize(arr, ar);
                    data = std::move(arr);
                } else if (first.IsNumber()) {
                    ccstd::vector<int32_t> arr;
                    SerializationTrait<ccstd::vector<int32_t>>::serialize(arr, ar);
                    data = std::move(arr);
                } else if (first.IsString()) {
                    ccstd::vector<ccstd::string> arr;
                    SerializationTrait<ccstd::vector<ccstd::string>>::serialize(arr, ar);
                    data = std::move(arr);
                } else {
                    assert(false);
                }
            }
        } else {
            assert(false);
        }
    }

    static void serialize(MacroValue &data, BinaryInputArchive &ar) {
    }
};

} // namespace cc
