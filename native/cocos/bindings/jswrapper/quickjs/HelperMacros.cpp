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

#include "HelperMacros.h"
#include "../State.h"
#include "../ValueArrayPool.h"
#include "../MappingUtils.h"
#include "Class.h"
#include "Object.h"
#include "Utils.h"

#if defined(RECORD_JSB_INVOKING)

namespace {
bool cmp(const std::pair<const char *, std::tuple<int, uint64_t>> &a, const std::pair<const char *, std::tuple<int, uint64_t>> &b) {
    return std::get<1>(a.second) > std::get<1>(b.second);
}
unsigned int                                      __jsbInvocationCount;        // NOLINT(readability-identifier-naming)
std::map<char const *, std::tuple<int, uint64_t>> __jsbFunctionInvokedRecords; // NOLINT(readability-identifier-naming)
} // namespace

JsbInvokeScopeT::JsbInvokeScopeT(const char *functionName) : _functionName(functionName) {
    _start = std::chrono::high_resolution_clock::now();
    __jsbInvocationCount++;
}
JsbInvokeScopeT::~JsbInvokeScopeT() {
    auto &ref = __jsbFunctionInvokedRecords[_functionName];
    std::get<0>(ref) += 1;
    std::get<1>(ref) += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - _start).count();
}

#endif

void printJSBInvokeAtFrame(int n) {
#if defined(RECORD_JSB_INVOKING)
    static int cnt = 0;
    cnt += 1;
    if (cnt % n == 0) {
        printJSBInvoke();
    }
#endif
}

void clearRecordJSBInvoke() {
#if defined(RECORD_JSB_INVOKING)
    __jsbInvocationCount = 0;
    __jsbFunctionInvokedRecords.clear();
#endif
}

void printJSBInvoke() {
#if defined(RECORD_JSB_INVOKING)
    static std::vector<std::pair<const char *, std::tuple<int, uint64_t>>> pairs;
    for (const auto &it : __jsbFunctionInvokedRecords) {
        pairs.emplace_back(it); //NOLINT
    }

    std::sort(pairs.begin(), pairs.end(), cmp);
    cc::Log::logMessage(cc::LogType::KERNEL, cc::LogLevel::LEVEL_DEBUG, "Start print JSB function record info....... %d times", __jsbInvocationCount);
    for (const auto &pair : pairs) {
        cc::Log::logMessage(cc::LogType::KERNEL, cc::LogLevel::LEVEL_DEBUG, "\t%s takes %.3lf ms, invoked %u times,", pair.first, std::get<1>(pair.second) / 1000000.0, std::get<0>(pair.second));
    }
    pairs.clear();
    cc::Log::logMessage(cc::LogType::KERNEL, cc::LogLevel::LEVEL_DEBUG, "End print JSB function record info.......\n");
#endif
}


SE_HOT JSValue jsbFunctionWrapper(JSContext *_ctx, JSValueConst _thisVal, int argc, JSValueConst *argv, se_function_ptr func,
                                  const char *funcName) {
    JSValue                _jsRet = JS_UNDEFINED;
    bool needDeleteValueArray{false};
    se::ValueArray &       args   = se::gValueArrayPool.get(argc, needDeleteValueArray);
    se::CallbackDepthGuard depthGuard{args, se::gValueArrayPool._depth, needDeleteValueArray};
    se::internal::jsToSeArgs(_ctx, argc, argv, args);

    se::Value seThisVal;
    se::internal::jsObjectToSeObject(_thisVal, &seThisVal);
    se::Object *thisObject = seThisVal.toObject();

    se::State state(thisObject, args);
    bool      ret = func(state);
    if (!ret) {
        SE_LOGE("[ERROR] Failed to invoke %s, location: %s:%d\n", funcName, __FILE__, __LINE__);
    } else {
        se::internal::seToJsValue(_ctx, state.rval(), &_jsRet);
    }
    return _jsRet;
}

SE_HOT void jsbFinalizeWrapper(JSRuntime *_rt, JSValue _thisVal, se_function_ptr func, const char *funcName) {
    se::Value seThisVal;
    se::internal::jsObjectToSeObject(_thisVal, &seThisVal);
    se::Object *seObj = seThisVal.toObject();

    void *nativeObj = seObj->getPrivateData();
    bool  ret       = false;
    if (seObj == nullptr)
        return;
    se::State state(seObj);
    ret = func(state);
    if (!ret) {
        SE_LOGE("[ERROR] Failed to invoke %s, location: %s:%d\n", funcName, __FILE__, __LINE__);
    }
    if (seObj->isClearMappingInFinalizer() && nativeObj != nullptr) {
        auto iter = se::NativePtrToObjectMap::find(nativeObj);
        if (iter != se::NativePtrToObjectMap::end()) {
            se::NativePtrToObjectMap::erase(iter);
        }
    }
    seObj->decRef();
}

SE_HOT JSValue jsbConstructorWrapper(JSContext *_ctx, JSValueConst new_target, int argc, JSValueConst *argv,
                      se_function_ptr func, se_finalize_ptr finalizeCb, se::Class *cls, const char *funcName) {
    bool needDeleteValueArray{false};
    se::ValueArray &       args = se::gValueArrayPool.get(argc, needDeleteValueArray);
    se::CallbackDepthGuard depthGuard{args, se::gValueArrayPool._depth, needDeleteValueArray};
    se::internal::jsToSeArgs(_ctx, argc, argv, args);
    JSValue proto = JS_GetPropertyStr(_ctx, new_target, "prototype");
    JSValue jsobj = JS_NewObjectProtoClass(_ctx, proto, cls->_getClassID());
    JS_FreeValue(_ctx, proto);
    se::Object *thisObject = se::Object::_createJSObject(cls, jsobj);
    se::State   state(thisObject, args);
    bool        ret = func(state);
    if (ret) {
        se::Value _property;
        bool      _found = false;
        _found           = thisObject->getProperty("_ctor", &_property);
        if (_found) _property.toObject()->call(args, thisObject);
    } else {
        SE_LOGE("[ERROR] Failed to invoke %s, location: %s:%d\n", funcName, __FILE__, __LINE__);
    }
    return jsobj;
}

SE_HOT JSValue jsbGetterWrapper(JSContext *_ctx, JSValueConst _thizObj,
                      se_function_ptr func,
                         const char *funcName) {
    JSValue _jsRet = JS_UNDEFINED;

    se::Value seThisVal;
    se::internal::jsObjectToSeObject(_thizObj, &seThisVal);
    se::Object *thisObject = seThisVal.toObject();

    se::State state(thisObject);
    bool      ret = func(state);
    if (!ret) {
        SE_LOGE("[ERROR] Failed to invoke %s, location: %s:%d\n", funcName, __FILE__, __LINE__);
    } else {
        se::internal::seToJsValue(_ctx, state.rval(), &_jsRet);
    }
    return _jsRet;
}

SE_HOT JSValue jsbSetterWrapper(JSContext *_ctx, JSValueConst _thizObj, JSValueConst _jsval, se_function_ptr func, const char *funcName) {
    bool needDeleteValueArray{false};
    se::Value seThisVal;
    se::internal::jsObjectToSeObject(_thizObj, &seThisVal);
    se::Object *thisObject = seThisVal.toObject();

    se::ValueArray &       args = se::gValueArrayPool.get(1, needDeleteValueArray);
    se::CallbackDepthGuard depthGuard{args, se::gValueArrayPool._depth, needDeleteValueArray};
    se::Value &            data{args[0]};
    se::internal::jsToSeValue(_ctx, _jsval, &data);
    se::State state(thisObject, args);
    bool      ret = func(state);
    if (!ret) {
        SE_LOGE("[ERROR] Failed to invoke %s, location: %s:%d\n", funcName, __FILE__, __LINE__);
    }
    return JS_UNDEFINED;
}
