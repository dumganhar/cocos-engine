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

#include "core/assets/SceneAsset.h"
#include "base/memory/Memory.h"
#include "core/scene-graph/Scene.h"
#include "serialization/BinaryInputArchive.h"
#include "serialization/JsonInputArchive.h"

namespace cc {

CC_IMPL_SERIALIZE(SceneAsset)

SceneAsset::SceneAsset() = default;
SceneAsset::~SceneAsset() = default;

bool SceneAsset::validate() const {
    return _scene.get() != nullptr;
}

void SceneAsset::setScene(Scene *scene) { _scene = scene; };

void SceneAsset::initDefault(const ccstd::optional<ccstd::string> &uuid) {
    Super::initDefault(uuid);
    _scene = ccnew Scene("New Scene");
}

template <class Archive>
void SceneAsset::serialize(Archive &ar) {
    Super::serialize(ar);
    ar.serialize(_scene, "scene");
    CC_LOG_DEBUG("==> SceneAsset: %p, scene: %p", this, _scene.get());
}

} // namespace cc
