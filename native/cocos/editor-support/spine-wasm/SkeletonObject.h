#ifndef _SKELETON_OBJECT_H__
#define _SKELETON_OBJECT_H__

#include "spine/spine.h"
#include <stdint.h>
#include "SkMeshData.h"
#include "MeshDataDef.h"
#include <vector>

class SkeletonObject {
struct UserData
{
    bool doScale = false;
    bool doFillZ = true;
    float scale = 1.0F;
    Color4F color = Color4F(1.0F, 1.0F, 1.0F, 1.0F);
};
public:
    SkeletonObject();
    ~SkeletonObject();
    uint32_t ObjectID();
    uint32_t initWithSkeletonData(bool isJson, uint32_t start, uint32_t length);
    uint32_t updateRenderData();
    uint32_t queryRenderDataInfo();
    uint32_t setSkin(uint32_t start, uint32_t length);
    uint32_t setAnimation(uint32_t trackIndex,uint32_t start, uint32_t length, bool loop);
    void     clearTrack(uint32_t trackIndex);
    void     clearTracks();
    void     setToSetupPose();
    uint32_t setTimeScale(float timeScale);
    uint32_t updateAnimation(float dltTime);
    void     setMix(uint32_t start, uint32_t fromLength, uint32_t toLength, float duration);
    uint32_t getDrawOrderSize();
    uint32_t getSlotNameByOrder(uint32_t index);
    uint32_t getBoneMatrix(uint32_t boneIdx);
    bool     setDefualtScale(float scale);
    void     setVertexEffect(spine::VertexEffect *effect);

    uint32_t testFunc(uint32_t start, uint32_t length);
private:
    void processVertices(std::vector<SkMeshData> &meshes);
    void mergeMeshes(std::vector<SkMeshData> &meshes);
    void resetMeshArray();
private:
    uint32_t _objID;
    spine::Skeleton *_skeleton = nullptr;
    spine::SkeletonData *_skeletonData = nullptr;
    spine::AnimationStateData* _animStateData = nullptr;
    spine::AnimationState* _animState = nullptr;
    spine::SkeletonClipping* _clipper = nullptr;
    spine::VertexEffect *_effect = nullptr;

    std::vector<SkMeshData*> _meshArray{};
    UserData userData;
};

typedef SkeletonObject* SkeletonHandle;
SkeletonHandle getSkeletonHandle(uint32_t objID);

#endif