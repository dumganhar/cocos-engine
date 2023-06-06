#ifndef _SPINE_SKELETON_INSTANCE_H_
#define _SPINE_SKELETON_INSTANCE_H_
#include <spine/spine.h>
#include "mesh-type-define.h"
#include "spine-model.h"
#include <string>
#include <memory>
#include <functional>
using namespace spine;

typedef std::function<void(TrackEntry *entry)> StartListener;
typedef std::function<void(TrackEntry *entry)> InterruptListener;
typedef std::function<void(TrackEntry *entry)> EndListener;
typedef std::function<void(TrackEntry *entry)> DisposeListener;
typedef std::function<void(TrackEntry *entry)> CompleteListener;
typedef std::function<void(TrackEntry *entry, Event *event)> EventListener;

class SpineSkeletonInstance {
    struct UserData {
        bool useTint = false;
        bool premultipliedAlpha = false;
        Color4F color = Color4F(1.0F, 1.0F, 1.0F, 1.0F);
    };

public:
    SpineSkeletonInstance();
    ~SpineSkeletonInstance();
    Skeleton *initSkeleton(SkeletonData* data);
    void setAnimation(float trackIndex, const std::string& name, bool loop);
    void setSkin(const std::string& name);
    void updateAnimation(float dltTime);
    SpineModel* updateRenderData();
    void setPremultipliedAlpha(bool val);
    void setUseTint(bool useTint);
    void setColor(float r, float g, float b, float a);
    void setJitterEffect(JitterVertexEffect *effect);
    void setSwirlEffect(SwirlVertexEffect *effect);
    void clearEffect();
    AnimationState* getAnimationState();
    void setMix(const std::string& from, const std::string& to, float duration);
    void setStartListener();
    virtual void onAnimationStateEvent(TrackEntry *entry, EventType type, Event *event);

private:
    void collectMeshData();
private:
    Skeleton *_skeleton = nullptr;
    SkeletonData *_skeletonData = nullptr;
    AnimationStateData* _animStateData = nullptr;
    AnimationState* _animState = nullptr;
    SkeletonClipping* _clipper = nullptr;
    VertexEffect *_effect = nullptr;
    SpineModel* _model = nullptr;
    StartListener _startListener = nullptr;
    InterruptListener _interruptListener = nullptr;
    EndListener _endListener = nullptr;
    DisposeListener _disposeListener = nullptr;
    CompleteListener _completeListener = nullptr;
    EventListener _eventListener = nullptr;
    
    UserData _userData;
};

#endif