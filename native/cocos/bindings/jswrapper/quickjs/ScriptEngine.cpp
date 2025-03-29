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

#include "ScriptEngine.h"

#if SCRIPT_ENGINE_TYPE == SCRIPT_ENGINE_QUICKJS

    #include "../MappingUtils.h"
    #include "../State.h"
    #include "Class.h"
    #include "Object.h"
    #include "Utils.h"

namespace se {

namespace {

const char *BYTE_CODE_FILE_EXT = ".jsc";

    #define countof(x) (sizeof(x) / sizeof((x)[0]))

ScriptEngine *__instance = nullptr;

JSValue __forceGC(JSContext *ctx, JSValueConst thisVal, int argc, JSValueConst *argv) {
    JS_RunGC(JS_GetRuntime(ctx));
    return JS_UNDEFINED;
}

JSValue __log(JSContext *ctx, JSValueConst thisVal, int argc, JSValueConst *argv) {
    if (argc > 0) {
        const char *string = JS_ToCString(ctx, argv[0]);
        if (string != nullptr) {
            SE_LOGD("JS: %s\n", string);
            JS_FreeCString(ctx, string);
        }
    }
    return JS_UNDEFINED;
}

void js_dump_obj(JSContext *ctx, FILE *f, JSValueConst val) {
    const char *str;

    str = JS_ToCString(ctx, val);
    if (str) {
        fprintf(f, ">> %s\n", str);
        JS_FreeCString(ctx, str);
    } else {
        fprintf(f, "[exception]\n");
    }
}

// ------------------------------------------------------- ScriptEngine

// For console stuff
bool JSB_console_format_log(State &s, const char *prefix, int msgIndex = 0) {
    if (msgIndex < 0)
        return false;

    const auto &args = s.args();
    int         argc = (int)args.size();
    if ((argc - msgIndex) == 1) {
        std::string msg = args[msgIndex].toStringForce();
        SE_LOGD("JS: %s%s\n", prefix, msg.c_str());
    } else if (argc > 1) {
        std::string msg = args[msgIndex].toStringForce();
        size_t      pos;
        for (int i = (msgIndex + 1); i < argc; ++i) {
            pos = msg.find("%");
            if (pos != std::string::npos && pos != (msg.length() - 1) && (msg[pos + 1] == 'd' || msg[pos + 1] == 's' || msg[pos + 1] == 'f')) {
                msg.replace(pos, 2, args[i].toStringForce());
            } else {
                msg += " " + args[i].toStringForce();
            }
        }

        SE_LOGD("JS: %s%s\n", prefix, msg.c_str());
    }

    return true;
}

bool JSB_console_log(State &s) {
    JSB_console_format_log(s, "");
    return true;
}
SE_BIND_FUNC(JSB_console_log)

bool JSB_console_debug(State &s) {
    JSB_console_format_log(s, "[DEBUG]: ");
    return true;
}
SE_BIND_FUNC(JSB_console_debug)

bool JSB_console_info(State &s) {
    JSB_console_format_log(s, "[INFO]: ");
    return true;
}
SE_BIND_FUNC(JSB_console_info)

bool JSB_console_warn(State &s) {
    JSB_console_format_log(s, "[WARN]: ");
    return true;
}
SE_BIND_FUNC(JSB_console_warn)

bool JSB_console_error(State &s) {
    JSB_console_format_log(s, "[ERROR]: ");
    return true;
}
SE_BIND_FUNC(JSB_console_error)

bool JSB_console_assert(State &s) {
    const auto &args = s.args();
    if (!args.empty()) {
        if (args[0].isBoolean() && !args[0].toBoolean()) {
            JSB_console_format_log(s, "[ASSERT]: ", 1);
        }
    }
    return true;
}
SE_BIND_FUNC(JSB_console_assert)

bool JSB_console_time(State &s) {
    return true; //TODO(cjh)
}
SE_BIND_FUNC(JSB_console_time)

bool JSB_console_timeEnd(State &s) {
    return true; //TODO(cjh)
}
SE_BIND_FUNC(JSB_console_timeEnd)

void test() {
    auto runtime = JS_NewRuntime();
    JS_SetDumpFlags(runtime, JS_DUMP_LEAKS);
    auto context = JS_NewContext(runtime);

    std::string source =
        "function foo() {}\n"
        "foo.prototype.bar = function() {};\n"
        "globalThis.helloworld = function(a, b, obj) { return a + b + obj.prop; }  \n"
        "globalThis.ownvalue = function(obj) { globalThis.aaa = obj; return {a: 1}; } \n"
    ;
    auto ret = JS_Eval(context, source.c_str(), source.size(), "", JS_EVAL_TYPE_GLOBAL);
    
    JSValue global = JS_GetGlobalObject(context);
    JSValue obj = JS_NewObject(context);
    int ref = JS_ValueRefCount(context, obj);
    JS_SetPropertyStr(context, global, "world_key", obj);
    JSValue val = JS_NewString(context, "hello");
    JS_SetPropertyStr(context, obj, "prop", val);
    ref = JS_ValueRefCount(context, obj);
    
    {
        JSValue val1 = JS_NewString(context, "hello111");
        JSValue val2 = JS_NewString(context, "world222");
        JSValue func = JS_GetPropertyStr(context, global, "helloworld");
        JSValue argv[3] = {val1, val2, obj};;
        JSValue ret = JS_Call(context, func, global, 3, argv);
        ref = JS_ValueRefCount(context, obj);
        const char * c = JS_ToCString(context, ret);
        printf("result: %s\n", c);
        JS_FreeCString(context, c);
        JS_FreeValue(context, func);
        JS_FreeValue(context, ret);
        JS_FreeValue(context, val1);
//        JS_FreeValue(context, val2);
    }
    
    {
        JSValue func = JS_GetPropertyStr(context, global, "ownvalue");
        JSValue argv[1] = {obj};;
        ref = JS_ValueRefCount(context, obj);
        JSValue ret = JS_Call(context, func, global, 1, argv);
        ref = JS_ValueRefCount(context, obj);
        JS_FreeValue(context, func);
        //JS_FreeValue(context, ret);
    }
    
    JS_FreeValue(context, ret);
    JS_FreeValue(context, global);
    JS_FreeContext(context);
    JS_FreeRuntime(runtime);
    int a = 0;
    printf("a: %d\n", a);
}

} // namespace

static std::stack<AutoHandleScope*> __scopeStack;

AutoHandleScope::AutoHandleScope() {
    __scopeStack.push(this);
    _jsValuesInScope.reserve(1024);
}

AutoHandleScope::~AutoHandleScope() {
    for (auto &e : _jsValuesInScope) {
        JS_FreeValue(ScriptEngine::getInstance()->_getContext(), e);
    }
    
    __scopeStack.pop();
}

void AutoHandleScope::push(JSValue v) {
    if (_inCleanup) return;
    JS_DupValue(ScriptEngine::getInstance()->_getContext(), v);
    _jsValuesInScope.emplace_back(v);
}

AutoHandleScope* AutoHandleScope::getCurrent() {
    if (!__scopeStack.empty()) {
        return __scopeStack.top();
    }
    return nullptr;
}

ScriptEngine *ScriptEngine::getInstance() {
    return __instance;
}

void ScriptEngine::destroyInstance() {
}

ScriptEngine::ScriptEngine() {
    __instance = this;
}

bool ScriptEngine::init() {
//    test();
    cleanup();
    SE_LOGD("Initializing QuickJS, version: %s\n", "2021-03-27");
    ++_vmId;
    
    _globalHandleScope = new AutoHandleScope();

    for (const auto &hook : _beforeInitHookArray) {
        hook();
    }
    _beforeInitHookArray.clear();

    _rt = JS_NewRuntime();
    if (nullptr == _rt)
        return false;
    
    JS_SetDumpFlags(_rt, JS_DUMP_LEAKS);

    _cx = JS_NewContext(_rt);
    if (nullptr == _cx) {
        JS_FreeRuntime(_rt);
        return false;
    }

    JS_SetMaxStackSize(_rt, 0xFFFFFFFF);

//    JS_AddIntrinsicBigFloat(_cx);
//    JS_AddIntrinsicBigDecimal(_cx);
//    JS_AddIntrinsicOperators(_cx);
//    JS_EnableBignumExt(_cx, 1);

    NativePtrToObjectMap::init();

    Class::setContext(_cx);
    Object::setContext(_cx);

    JSValue globalObj = JS_GetGlobalObject(_cx);
    _globalObj        = Object::_createJSObject(nullptr, globalObj);
    _globalObj->root();

    _globalObj->setProperty("window", Value(_globalObj));

    // QuickJS isn't shipped with a console variable. Make a fake one.
    Value consoleVal;
    bool  hasConsole = _globalObj->getProperty("console", &consoleVal) && consoleVal.isObject();
    assert(!hasConsole);

    HandleObject consoleObj(Object::createPlainObject());
    consoleObj->defineFunction("log", _SE(JSB_console_log));
    consoleObj->defineFunction("debug", _SE(JSB_console_debug));
    consoleObj->defineFunction("info", _SE(JSB_console_info));
    consoleObj->defineFunction("warn", _SE(JSB_console_warn));
    consoleObj->defineFunction("error", _SE(JSB_console_error));
    consoleObj->defineFunction("assert", _SE(JSB_console_assert));
    consoleObj->defineFunction("time", _SE(JSB_console_info));    //TODO(cjh)
    consoleObj->defineFunction("timeEnd", _SE(JSB_console_info)); //TODO(cjh)

    _globalObj->setProperty("console", Value(consoleObj));

    _globalObj->setProperty("scriptEngineType", Value("quickjs"));

    static const JSCFunctionListEntry funcs[] = {
        JS_CFUNC_DEF("log", 0, __log),
        JS_CFUNC_DEF("forceGC", 0, __forceGC),
    };
    JS_SetPropertyFunctionList(_cx, globalObj, funcs, countof(funcs));

    JS_FreeValue(_cx, globalObj);
    globalObj = JS_UNDEFINED;

    _isValid = true;

    for (const auto &hook : _afterInitHookArray) {
        hook();
    }
    _afterInitHookArray.clear();
    
    return true;
}

ScriptEngine::~ScriptEngine() {
    cleanup();
    __instance = nullptr;
}

void ScriptEngine::cleanup() {
    if (!_isValid)
        return;

    _isInCleanup = true;
    for (const auto &hook : _beforeCleanupHookArray) {
        hook();
    }
    _beforeCleanupHookArray.clear();
    
//    _globalObj->setProperty("window", Value::Undefined);
//    _globalObj->setProperty("jsb", Value::Undefined);

    mainLoopUpdate();
    
    
    auto jsobj = _globalObj->_getJSObject();
    int ref = JS_ValueRefCount(_cx, _globalObj->_getJSObject());
    
    Class::cleanup();
    ref = JS_ValueRefCount(_cx, _globalObj->_getJSObject());
    Object::cleanup();
    ref = JS_ValueRefCount(_cx, _globalObj->_getJSObject());
    
    _globalObj->unroot();
    ref = JS_ValueRefCount(_cx, _globalObj->_getJSObject());
    
//    JS_FreeValue(_cx, jsobj);
//    JS_FreeValue(_cx, jsobj);
//    JS_FreeValue(_cx, jsobj);
//    JS_FreeValue(_cx, jsobj);
    
    _globalObj->decRef();
    _globalObj = nullptr;
    
    ref = JS_ValueRefCount(_cx, jsobj);
    
    _globalHandleScope->_inCleanup = true;
    
    JS_FreeContext(_cx);
    JS_FreeRuntime(_rt);
    
    _globalHandleScope->_jsValuesInScope.clear();
    delete _globalHandleScope;
    _globalHandleScope = nullptr;

    _rt        = nullptr;
    _cx        = nullptr;
    _isValid   = false;

    _registerCallbackArray.clear();

    for (const auto &hook : _afterCleanupHookArray) {
        hook();
    }
    _afterCleanupHookArray.clear();
    _isInCleanup = false;

    NativePtrToObjectMap::destroy();
}

void ScriptEngine::addBeforeCleanupHook(const std::function<void()> &hook) {
    _beforeCleanupHookArray.push_back(hook);
}

void ScriptEngine::addAfterCleanupHook(const std::function<void()> &hook) {
    _afterCleanupHookArray.push_back(hook);
}

void ScriptEngine::addBeforeInitHook(const std::function<void()> &hook) {
    _beforeInitHookArray.push_back(hook);
}

void ScriptEngine::addAfterInitHook(const std::function<void()> &hook) {
    _afterInitHookArray.push_back(hook);
}

bool ScriptEngine::isGarbageCollecting() {
    return _isGarbageCollecting;
}

void ScriptEngine::_setGarbageCollecting(bool isGarbageCollecting) {
    _isGarbageCollecting = isGarbageCollecting;
}

Object *ScriptEngine::getGlobalObject() {
    return _globalObj;
}

void ScriptEngine::addRegisterCallback(RegisterCallback cb) {
    assert(std::find(_registerCallbackArray.begin(), _registerCallbackArray.end(), cb) == _registerCallbackArray.end());
    _registerCallbackArray.push_back(cb);
}

void ScriptEngine::addPermanentRegisterCallback(RegisterCallback cb) {
    if (std::find(_permRegisterCallbackArray.begin(), _permRegisterCallbackArray.end(), cb) == _permRegisterCallbackArray.end()) {
        _permRegisterCallbackArray.push_back(cb);
    }
}

bool ScriptEngine::start() {
    if (!init())
        return false;

    if (isDebuggerEnabled() && _debugGlobalObj == nullptr) {
    }
       
    bool ok    = false;
    _startTime = std::chrono::steady_clock::now();

    for (auto cb : _permRegisterCallbackArray) {
        ok = cb(_globalObj);
        assert(ok);
        if (!ok) {
            break;
        }
    }

    for (auto cb : _registerCallbackArray) {
        ok = cb(_globalObj);
        assert(ok);
        if (!ok)
            break;
    }

    // After ScriptEngine is started, _registerCallbackArray isn't needed. Therefore, clear it here.
    _registerCallbackArray.clear();
    return ok;
}

bool ScriptEngine::evalString(const char *script, ssize_t length /* = -1 */, Value *ret /* = nullptr */, const char *fileName /* = nullptr */) {
    assert(script != nullptr);

    if (length < 0)
        length = strlen(script);

    if (fileName == nullptr)
        fileName = "(no filename)";

    JSValue jsRet = JS_Eval(_cx, script, length, fileName, JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(jsRet)) {
        clearException();
        return false;
    }

    if (ret != nullptr) {
        internal::jsToSeValue(_cx, jsRet, ret);
    }
    
    JS_FreeValue(_cx, jsRet);
    return true;
}

void ScriptEngine::setFileOperationDelegate(const FileOperationDelegate &delegate) {
    _fileOperationDelegate = delegate;
}

const ScriptEngine::FileOperationDelegate &ScriptEngine::getFileOperationDelegate() const {
    return _fileOperationDelegate;
}

bool ScriptEngine::runScript(const std::string &path, Value *ret /* = nullptr */) {
    assert(_fileOperationDelegate.isValid());
    printf("cjh runScript: %s\n", path.c_str());
    std::string scriptBuffer = _fileOperationDelegate.onGetStringFromFile(path);
    if (!scriptBuffer.empty()) {
        return evalString(scriptBuffer.c_str(), static_cast<ssize_t>(scriptBuffer.length()), ret, path.c_str());
    }

    SE_LOGE("ScriptEngine::runScript script %s, buffer is empty!\n", path.c_str());
    return false;
}

void ScriptEngine::clearException() {
    if (_cx == nullptr)
        return;

    JSValue exception_val = JS_GetException(_cx);
    if (JS_IsUndefined(exception_val) || JS_IsNull(exception_val) || JS_IsUninitialized(exception_val)) {
        return;
    }

    JSValue val      = JS_UNDEFINED;
    int     is_error = JS_IsError(_cx, exception_val);
    js_dump_obj(_cx, stderr, exception_val);
    if (is_error) {
        val = JS_GetPropertyStr(_cx, exception_val, "stack");
        if (!JS_IsUndefined(val)) {
            js_dump_obj(_cx, stderr, val);
        }
        JS_FreeValue(_cx, val);
    }

    JS_FreeValue(_cx, exception_val);
}

void ScriptEngine::setExceptionCallback(const ExceptionCallback &cb) {
    _exceptionCallback = cb;
}

void ScriptEngine::setJSExceptionCallback(const ExceptionCallback &cb) { //TODO(cjh)
}

void ScriptEngine::enableDebugger(const std::string &serverAddr, uint32_t port, bool isWait) {
}

bool ScriptEngine::isDebuggerEnabled() const {
    return false;
}

void ScriptEngine::executePendingJobs() {
    JSContext* cx = nullptr;
    int ret = 0;
    while (true) {
        ret = JS_ExecutePendingJob(_rt, &cx);
        if (ret == -1) {
            clearException();
            break;
        } else if (ret == 0) {
            break;
        }
    }
}

void ScriptEngine::mainLoopUpdate() {
    executePendingJobs();
    
    auto* scope = AutoHandleScope::getCurrent();
    if (scope && !scope->_jsValuesInScope.empty()) {
        for (auto &e : scope->_jsValuesInScope) {
            JS_FreeValue(_cx, e);
        }
        scope->_jsValuesInScope.clear();
    }
}

std::string ScriptEngine::getCurrentStackTrace() const {
    JSValue error = JS_NewError(_cx);
    JSValue stack = JS_GetPropertyStr(_cx, error, "stack");
    const char* stackCStr = JS_ToCString(_cx, stack);
    std::string ret = stackCStr;
    JS_FreeCString(_cx, stackCStr);
    JS_FreeValue(_cx, stack);
    JS_FreeValue(_cx, error);
    return ret;
}

bool ScriptEngine::callFunction(Object *targetObj, const char *funcName, uint32_t argc, Value *args, Value *rval /* = nullptr*/) {
    ValueArray argv;
    argv.reserve(argc);
    for (uint32_t i = 0; i < argc; ++i) {
        argv.emplace_back(args[i]);
    }
    
    Value funcVal;
    if (targetObj->getProperty(funcName, &funcVal)) {
        return funcVal.toObject()->call(argv, targetObj, rval);
    }

    return false;
}

void ScriptEngine::handlePromiseExceptions() {
    clearException();
} //TODO(cjh)

} // namespace se

#endif // #if SCRIPT_ENGINE_TYPE == SCRIPT_ENGINE_QUICKJS
