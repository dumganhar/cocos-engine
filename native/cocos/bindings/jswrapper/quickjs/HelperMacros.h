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

#include "../ValueArrayPool.h"
#include "../config.h"

#include "Base.h"

//#define RECORD_JSB_INVOKING

#ifndef CC_DEBUG
    #undef RECORD_JSB_INVOKING
#endif

#if SCRIPT_ENGINE_TYPE == SCRIPT_ENGINE_QUICKJS

    #if defined(RECORD_JSB_INVOKING)

class JsbInvokeScopeT {
public:
    JsbInvokeScopeT(const char *functionName);
    ~JsbInvokeScopeT();

private:
    const char *                                                _functionName;
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;
};
        #define JsbInvokeScope(arg) JsbInvokeScopeT invokeScope(arg); // NOLINT(readability-identifier-naming)

    #else
        // NOLINTNEXTLINE(readability-identifier-naming)
        #define JsbInvokeScope(arg) \
            do {                    \
            } while (0)

    #endif

template <typename T, typename STATE>
constexpr inline T *SE_THIS_OBJECT(STATE &s) { // NOLINT(readability-identifier-naming)
    return reinterpret_cast<T *>(s.nativeThisObject());
}

template <typename T>
constexpr typename std::enable_if<std::is_enum<T>::value, char *>::type SE_UNDERLYING_TYPE_NAME() { // NOLINT(readability-identifier-naming)
    return typeid(std::underlying_type_t<T>).name();
}

template <typename T>
constexpr typename std::enable_if<!std::is_enum<T>::value, char *>::type SE_UNDERLYING_TYPE_NAME() { // NOLINT(readability-identifier-naming)
    return typeid(T).name();
}

void clearRecordJSBInvoke();

void printJSBInvoke();

void printJSBInvokeAtFrame(int n);

namespace se {
class Class;
class Object;
class State;
} // namespace se

using se_function_ptr = bool (*)(se::State &state);
using se_finalize_ptr = void (*)(JSRuntime *_rt, JSValue _thisVal);

JSValue jsbFunctionWrapper(JSContext *_ctx, JSValueConst _thisVal, int argc, JSValueConst *argv,
                        se_function_ptr,
                        const char *);
void jsbFinalizeWrapper(JSRuntime *_rt, JSValue _thisVal,
                        se_function_ptr,
                        const char *);
JSValue jsbConstructorWrapper(JSContext *_ctx, JSValueConst new_target, int argc, JSValueConst *argv,
                           se_function_ptr,
                           se_finalize_ptr finalizeCb,
                           se::Class *,
                           const char *);
JSValue jsbGetterWrapper(JSContext *_ctx, JSValueConst _thizObj,
                      se_function_ptr,
                      const char *);
JSValue jsbSetterWrapper(JSContext *_ctx, JSValueConst _thizObj, JSValueConst _jsval,
                      se_function_ptr,
                      const char *);

#ifdef __GNUC__
    #define SE_UNUSED __attribute__((unused))
    #define SE_HOT    __attribute__((hot))
#else
    #define SE_UNUSED
    #define SE_HOT
#endif

    #define SAFE_INC_REF(obj) \
        if (obj != nullptr) obj->incRef()
    #define SAFE_DEC_REF(obj)   \
        if ((obj) != nullptr) { \
            (obj)->decRef();    \
            (obj) = nullptr;    \
        }

    #define _SE(name) name##Registry

    #define SE_DECLARE_FUNC(funcName) \
        JSValue funcName##Registry(JSContext *_ctx, JSValueConst _thisVal, int argc, JSValueConst *argv)

    #define SE_BIND_FUNC(funcName)                                                                         \
        JSValue funcName##Registry(JSContext *_ctx, JSValueConst _thisVal, int argc, JSValueConst *argv) { \
            JsbInvokeScope(#funcName);                                                                     \
            return jsbFunctionWrapper(_ctx, _thisVal, argc, argv, funcName, #funcName); \
        }

    #define SE_BIND_FUNC_FAST(funcName)                                                                    \
        JSValue funcName##Registry(JSContext *_ctx, JSValueConst _thisVal, int argc, JSValueConst *argv) { \
            JsbInvokeScope(#funcName);                                                                     \
            se::Object *seObj = (se::Object *)se::internal::getPrivate(_thisVal);                          \
            if (seObj) {                                                                                   \
                funcName(seObj->getPrivateData());                                                         \
            } else {                                                                                       \
                funcName(nullptr);                                                                         \
            }                                                                                              \
            return JS_UNDEFINED;                                                                           \
        }

    #define SE_DECLARE_FINALIZE_FUNC(funcName) \
        void funcName##Registry(JSRuntime *_rt, JSValue _thisVal);

    #define SE_BIND_FINALIZE_FUNC(funcName)                                                               \
        void funcName##Registry(JSRuntime *_rt, JSValue _thisVal) {                                       \
            JsbInvokeScope(#funcName);                                                                    \
            jsbFinalizeWrapper(_rt, _thisVal, funcName, #funcName); \
        }

    #define SE_BIND_CTOR(funcName, cls, finalizeCb)                                                          \
        JSValue funcName##Registry(JSContext *_ctx, JSValueConst new_target, int argc, JSValueConst *argv) { \
            JsbInvokeScope(#funcName);                                                                       \
            return jsbConstructorWrapper(_ctx, new_target, argc, argv, funcName, _SE(finalizeCb), cls, #funcName); \
        }

    #define SE_BIND_PROP_GET_IMPL(funcName, postFix)                                                      \
        JSValue funcName##postFix##Registry(JSContext *_ctx, JSValueConst _thizObj) {                     \
            JsbInvokeScope(#funcName);                                                                    \
            return jsbGetterWrapper(_ctx, _thizObj, funcName, #funcName); \
        }

    #define SE_BIND_PROP_GET(funcName)         SE_BIND_PROP_GET_IMPL(funcName, )
    #define SE_BIND_FUNC_AS_PROP_GET(funcName) SE_BIND_PROP_GET_IMPL(funcName, _asGetter)

    #define SE_BIND_PROP_SET_IMPL(funcName, postFix)                                                       \
        JSValue funcName##postFix##Registry(JSContext *_ctx, JSValueConst _thizObj, JSValueConst _jsval) { \
            JsbInvokeScope(#funcName);                                                                     \
            return jsbSetterWrapper(_ctx, _thizObj, _jsval, funcName, #funcName); \
        }

    #define SE_BIND_PROP_SET(funcName)         SE_BIND_PROP_SET_IMPL(funcName, )
    #define SE_BIND_FUNC_AS_PROP_SET(funcName) SE_BIND_PROP_SET_IMPL(funcName, _asSetter)

    #define SE_TYPE_NAME(t) typeid(t).name()

    #define SE_QUOTEME_(x) #x
    #define SE_QUOTEME(x)  SE_QUOTEME_(x)

    #define SE_REPORT_ERROR(fmt, ...)                                                        \
        SE_LOGD("ERROR (" __FILE__ ", " SE_QUOTEME(__LINE__) "): " fmt "\n", ##__VA_ARGS__); \
        JS_ThrowSyntaxError(se::ScriptEngine::getInstance()->_getContext(), fmt, ##__VA_ARGS__)

    #if CC_DEBUG > 0

        #define SE_ASSERT(cond, fmt, ...)                                                                 \
            do {                                                                                          \
                if (!(cond)) {                                                                            \
                    SE_LOGE("ASSERT (" __FILE__ ", " SE_QUOTEME(__LINE__) "): " fmt "\n", ##__VA_ARGS__); \
                    assert(false);                                                                        \
                }                                                                                         \
            } while (false)

    #else

        #define SE_ASSERT(cond, fmt, ...)

    #endif // #if CC_DEBUG > 0

#endif // #if SCRIPT_ENGINE_TYPE == SCRIPT_ENGINE_QUICKJS
