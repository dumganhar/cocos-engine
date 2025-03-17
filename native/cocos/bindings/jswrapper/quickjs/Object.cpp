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

#include "Object.h"

#if SCRIPT_ENGINE_TYPE == SCRIPT_ENGINE_QUICKJS

    #include "../MappingUtils.h"
    #include "Class.h"
    #include "ScriptEngine.h"
    #include "Utils.h"

#include "base/UTF8.h"

namespace se {

std::unordered_map<Object *, void *> __objectMap; // Currently, the value `void*` is always nullptr

namespace {
JSContext *__cx = nullptr;
} // namespace

Object::Object() {
    _currentVMId = ScriptEngine::getInstance()->getVMId();
}

Object::~Object() {
    unroot();
    
    delete _privateObject;
    _privateObject = nullptr;

    auto iter = __objectMap.find(this);
    if (iter != __objectMap.end()) {
        __objectMap.erase(iter);
    }
}

bool Object::init(Class *cls, JSValue obj) {
    _cls = cls;
    _obj = obj;

    assert(__objectMap.find(this) == __objectMap.end());
    __objectMap.emplace(this, nullptr);

    if (_cls == nullptr) {
        root();
    }

    return true;
}

Object *Object::_createJSObject(Class *cls, JSValue obj) {
    Object *ret = new Object();
    if (!ret->init(cls, obj)) {
        delete ret;
        ret = nullptr;
    }

    return ret;
}

Object *Object::createPlainObject() {
    Object *obj = Object::_createJSObject(nullptr, JS_NewObject(__cx));
    return obj;
}

Object *Object::createObjectWithClass(Class *cls) {
    JSValue jsobj = Class::_createJSObjectWithClass(cls);
    Object *obj   = Object::_createJSObject(cls, jsobj);
    return obj;
}

/* static */
Object *Object::createProxyTarget(se::Object *proxy) {
    //TODO:
    return nullptr;
}

Object *Object::getObjectWithPtr(void *ptr) {
    Object *obj  = nullptr;
    auto    iter = NativePtrToObjectMap::find(ptr);
    if (iter != NativePtrToObjectMap::end()) {
        obj = iter->second;
        obj->incRef();
    }
    return obj;
}

Object *Object::createArrayObject(size_t length) {
    JSValue jsobj = JS_NewArray(__cx);
    for (size_t i = 0; i < length; ++i) {
        JS_SetPropertyUint32(__cx, jsobj, i, JS_UNDEFINED);
    }
    Object *obj = Object::_createJSObject(nullptr, jsobj);
    return obj;
}

Object *Object::createArrayBufferObject(const void *data, size_t byteLength) {
    Object *obj   = nullptr;
    JSValue jsobj = JS_NewArrayBufferCopy(__cx, reinterpret_cast<const uint8_t *>(data), byteLength);
    if (!JS_IsException(jsobj)) {
        obj = Object::_createJSObject(nullptr, jsobj);
    } else {
        ScriptEngine::getInstance()->clearException();
    }

    return obj;
}

/* static */
Object *Object::createExternalArrayBufferObject(void *contents, size_t byteLength, BufferContentsFreeFunc freeFunc, void *freeUserData /* = nullptr*/) {
    struct BackingStoreUserData {
        BufferContentsFreeFunc freeFunc;
        void *                 freeUserData;
        size_t                 byteLength;
    };

    auto *userData         = new BackingStoreUserData();
    userData->freeFunc     = freeFunc;
    userData->freeUserData = freeUserData;
    userData->byteLength   = byteLength;

    Object *obj = nullptr;

    JSValue jsobj = JS_NewArrayBuffer(
        __cx, reinterpret_cast<uint8_t *>(contents), byteLength, [](JSRuntime *rt, void *opaque, void *ptr) {
            auto *userData = reinterpret_cast<BackingStoreUserData *>(opaque);
            userData->freeFunc(ptr, userData->byteLength, userData->freeUserData);
            delete userData;
        },
        userData, 0);

    if (!JS_IsException(jsobj)) {
        obj = Object::_createJSObject(nullptr, jsobj);
    } else {
        ScriptEngine::getInstance()->clearException();
    }
    return obj;
}

Object *Object::createTypedArray(TypedArrayType type, const void *data, size_t byteLength) {
    if (type == TypedArrayType::NONE) {
        SE_LOGE("Don't pass se::Object::TypedArrayType::NONE to createTypedArray API!");
        return nullptr;
    }

    #define CREATE_TYPEDARRAY(_name, _classId, bytesPerElement)                                                                               \
        {                                                                                                                    \
            JSValue argv[1] = { JS_NewInt64(__cx, byteLength / bytesPerElement) }; \
            JSValue typedArray = JS_NewTypedArray(__cx, 1, argv, _classId);                   \
            size_t pbyte_offset = 0; \
            size_t pbyte_length = 0; \
            size_t pbytes_per_element = 0; \
            JSValue ab = JS_GetTypedArrayBuffer(__cx, typedArray, &pbyte_offset, &pbyte_length, &pbytes_per_element); \
            size_t abSize = 0; \
            uint8_t *mem = JS_GetArrayBuffer(__cx, &abSize, ab); \
            assert(abSize == byteLength); \
            memcpy(mem, data, abSize); \
            Object *obj = Object::_createJSObject(nullptr, typedArray);                                                              \
            return obj;                                                                                                      \
        }

    switch (type) {
        case TypedArrayType::INT8:
            CREATE_TYPEDARRAY(Int8Array, JS_TYPED_ARRAY_INT8, 1)
        case TypedArrayType::INT16:
            CREATE_TYPEDARRAY(Int16Array, JS_TYPED_ARRAY_INT16, 2)
        case TypedArrayType::INT32:
            CREATE_TYPEDARRAY(Int32Array, JS_TYPED_ARRAY_INT32, 4)
        case TypedArrayType::UINT8:
            CREATE_TYPEDARRAY(Uint8Array, JS_TYPED_ARRAY_UINT8, 1)
        case TypedArrayType::UINT8_CLAMPED:
            CREATE_TYPEDARRAY(Uint8Array, JS_TYPED_ARRAY_UINT8C, 1)
        case TypedArrayType::UINT16:
            CREATE_TYPEDARRAY(Uint16Array, JS_TYPED_ARRAY_UINT16, 2)
        case TypedArrayType::UINT32:
            CREATE_TYPEDARRAY(Uint32Array, JS_TYPED_ARRAY_UINT32, 4)
        case TypedArrayType::FLOAT32:
            CREATE_TYPEDARRAY(Float32Array, JS_TYPED_ARRAY_FLOAT32, 4)
        case TypedArrayType::FLOAT64:
            CREATE_TYPEDARRAY(Float64Array, JS_TYPED_ARRAY_FLOAT64, 8)
        default:
            assert(false); // Should never go here.
            break;
    }

    return nullptr;
    #undef CREATE_TYPEDARRAY
}

/* static */
Object *Object::createTypedArrayWithBuffer(TypedArrayType type, const Object *obj) {
    return Object::createTypedArrayWithBuffer(type, obj, 0);
}

/* static */
Object *Object::createTypedArrayWithBuffer(TypedArrayType type, const Object *obj, size_t offset) {
    size_t   byteLength{0};
    uint8_t *skip{nullptr};
    obj->getTypedArrayData(&skip, &byteLength);
    return Object::createTypedArrayWithBuffer(type, obj, offset, byteLength - offset);
}

/* static */
Object *Object::createTypedArrayWithBuffer(TypedArrayType type, const Object *obj, size_t offset, size_t byteLength) {
    if (type == TypedArrayType::NONE) {
        SE_LOGE("Don't pass se::Object::TypedArrayType::NONE to createTypedArray API!");
        return nullptr;
    }

    if (type == TypedArrayType::UINT8_CLAMPED) {
        SE_LOGE("Doesn't support to create Uint8ClampedArray with Object::createTypedArray API!");
        return nullptr;
    }

    assert(obj->isArrayBuffer());
    //    JS::RootedObject jsobj(__cx, obj->_getJSObject());
    //
    //    switch (type) {
    //        case TypedArrayType::INT8: {
    //            JS::RootedObject typeArray(__cx, JS_NewInt8ArrayWithBuffer(__cx, jsobj, offset, byteLength));
    //            return Object::_createJSObject(nullptr, typeArray);
    //        }
    //        case TypedArrayType::INT16: {
    //            JS::RootedObject typeArray(__cx, JS_NewInt16ArrayWithBuffer(__cx, jsobj, offset, byteLength / 2));
    //            return Object::_createJSObject(nullptr, typeArray);
    //        }
    //        case TypedArrayType::INT32: {
    //            JS::RootedObject typeArray(__cx, JS_NewInt32ArrayWithBuffer(__cx, jsobj, offset, byteLength / 4));
    //            return Object::_createJSObject(nullptr, typeArray);
    //        }
    //        case TypedArrayType::UINT8: {
    //            JS::RootedObject typeArray(__cx, JS_NewUint8ArrayWithBuffer(__cx, jsobj, offset, byteLength));
    //            return Object::_createJSObject(nullptr, typeArray);
    //        }
    //        case TypedArrayType::UINT16: {
    //            JS::RootedObject typeArray(__cx, JS_NewUint16ArrayWithBuffer(__cx, jsobj, offset, byteLength / 2));
    //            return Object::_createJSObject(nullptr, typeArray);
    //        }
    //        case TypedArrayType::UINT32: {
    //            JS::RootedObject typeArray(__cx, JS_NewUint32ArrayWithBuffer(__cx, jsobj, offset, byteLength / 4));
    //            return Object::_createJSObject(nullptr, typeArray);
    //        }
    //        case TypedArrayType::FLOAT32: {
    //            JS::RootedObject typeArray(__cx, JS_NewFloat32ArrayWithBuffer(__cx, jsobj, offset, byteLength / 4));
    //            return Object::_createJSObject(nullptr, typeArray);
    //        }
    //        case TypedArrayType::FLOAT64: {
    //            JS::RootedObject typeArray(__cx, JS_NewFloat64ArrayWithBuffer(__cx, jsobj, offset, byteLength / 8));
    //            return Object::_createJSObject(nullptr, typeArray);
    //        }
    //        default:
    //            assert(false); // Should never go here.
    //            break;
    //    }

    return nullptr;
}

Object *Object::createUint8TypedArray(uint8_t *data, size_t dataCount) {
    return createTypedArray(TypedArrayType::UINT8, data, dataCount);
}

Object *Object::createJSONObject(const std::string &jsonStr) {
    Object *obj   = nullptr;
    JSValue jsval = JS_ParseJSON(__cx, jsonStr.c_str(), jsonStr.length(), "json_file");
    if (!JS_IsException(jsval)) {
        obj = Object::_createJSObject(nullptr, jsval);
    } else {
        ScriptEngine::getInstance()->clearException();
    }
    return obj;
}

Object *Object::createJSONObject(std::u16string &&jsonStr) {
    std::string utf8Str;
    if (cc::StringUtils::UTF16ToUTF8(jsonStr, utf8Str)) {
        return Object::createJSONObject(utf8Str);
    }
    return nullptr;
}

void Object::_setFinalizeCallback(JSClassFinalizer finalizeCb) {
    _finalizeCb = finalizeCb;
}

bool Object::getProperty(const char *name, Value *data, bool cachePropertyName) {
    assert(data != nullptr);

    bool    ret   = false;
    JSValue jsval = JS_UNDEFINED;

    JSAtom atom = JS_NewAtom(__cx, name);

    if (JS_HasProperty(__cx, _obj, atom) > 0) {
        jsval = JS_GetProperty(__cx, _obj, atom);
        ret   = true;
    }

    JS_FreeAtom(__cx, atom);
    internal::jsToSeValue(__cx, jsval, data);
    return ret;
}

bool Object::setProperty(const char *name, const Value &v) {
    JSValue jsval = JS_UNDEFINED;
    internal::seToJsValue(__cx, v, &jsval);
    return 1 == JS_SetPropertyStr(__cx, _obj, name, jsval);
}

bool Object::defineProperty(const char *name, JSPropGetter getter, JSPropSetter setter) {
    JSCFunctionListEntry entry = JS_CGETSET_DEF(name, getter, setter);
    JS_SetPropertyFunctionList(__cx, _obj, &entry, 1);
    return true;
}

bool Object::defineOwnProperty(const char *name, const se::Value &value, bool writable, bool enumerable, bool configurable) {
    JSValue jsval = JS_UNDEFINED;
    internal::seToJsValue(__cx, value, &jsval);

    int flags = 0;
    if (writable) {
        flags |= JS_PROP_WRITABLE;
    }
    if (enumerable) {
        flags |= JS_PROP_ENUMERABLE;
    }
    if (configurable) {
        flags |= JS_PROP_CONFIGURABLE;
    }

    return JS_DefinePropertyValueStr(__cx, _obj, name, jsval, flags) > 0;
}

bool Object::call(const ValueArray &args, Object *thisObject, Value *rval /* = nullptr*/) {
    assert(isFunction());

    JSValue *jsArgs = reinterpret_cast<JSValue *>(alloca(args.size() * sizeof(JSValue)));
    internal::seToJsArgs(__cx, args, jsArgs);
    JSValue jsRet = JS_Call(__cx, _obj, (thisObject != nullptr ? thisObject->_getJSObject() : JS_UNDEFINED), args.size(), jsArgs);
    if (!JS_IsException(jsRet)) {
        if (rval) {
            internal::jsToSeValue(__cx, jsRet, rval);
        }
        return true;
    }

    ScriptEngine::getInstance()->clearException();
    return false;
}

bool Object::defineFunction(const char *funcName, JSCFunction *func) {
    JSValue jsFuncVal = JS_NewCFunction2(__cx, func, funcName, 0, JS_CFUNC_generic, 0);
    JS_SetPropertyStr(__cx, _obj, funcName, jsFuncVal);
    return true;
}

bool Object::getArrayLength(uint32_t *length) const {
    assert(length != nullptr);
    if (!isArray())
        return false;

    JSValue lengthVal = JS_GetPropertyStr(__cx, _obj, "length");
    assert(JS_IsNumber(lengthVal));
    return 0 == JS_ToUint32(__cx, length, lengthVal);
}

bool Object::getArrayElement(uint32_t index, Value *data) const {
    assert(data != nullptr);
    data->setUndefined();

    if (!isArray())
        return false;

    uint32_t length{0};
    getArrayLength(&length);
    if (length > 0 && index < length) {
        JSValue jsval = JS_GetPropertyUint32(__cx, _obj, index);
        internal::jsToSeValue(__cx, jsval, data);
        return true;
    }
    return false;
}

bool Object::setArrayElement(uint32_t index, const Value &data) {
    if (!isArray())
        return false;

    JSValue jsval = JS_UNDEFINED;
    internal::seToJsValue(__cx, data, &jsval);
    JS_SetPropertyUint32(__cx, _obj, index, jsval);
    return true;
}

bool Object::isFunction() const {
    return JS_IsFunction(__cx, _obj);
}

bool Object::isTypedArray() const {
    bool result = false;
    auto context = __cx;
    JSValue constructor = JS_GetPropertyStr(context, _obj, "constructor");
    JSValue name = JS_GetPropertyStr(context, constructor, "name");
    const char* cName = JS_ToCString(context, name);
    result = !strcmp("Uint8ClampedArray", cName ? cName : "") || !strcmp("Int8Array", cName ? cName : "") ||
             !strcmp("Uint8Array", cName ? cName : "") || !strcmp("Int16Array", cName ? cName : "") ||
             !strcmp("Uint16Array", cName ? cName : "") || !strcmp("Int32Array", cName ? cName : "") ||
             !strcmp("Uint32Array", cName ? cName : "") || !strcmp("BigInt64Array", cName ? cName : "") ||
             !strcmp("BigUint64Array", cName ? cName : "") || !strcmp("Float32Array", cName ? cName : "") ||
             !strcmp("Float64Array", cName ? cName : "");

    JS_FreeCString(context, cName);
    JS_FreeValue(context, name);
    JS_FreeValue(context, constructor);
    return result;
}

bool Object::isProxy() const {
    //TODO:
    return false;//
}

Object::TypedArrayType Object::getTypedArrayType() const {
    TypedArrayType ret = TypedArrayType::NONE;
    //    JSValue     obj = _getJSObject();
    //    if (JS_IsInit(obj))
    //        ret = TypedArrayType::INT8;
    //    else if (JS_IsInt16Array(obj))
    //        ret = TypedArrayType::INT16;
    //    else if (JS_IsInt32Array(obj))
    //        ret = TypedArrayType::INT32;
    //    else if (JS_IsUint8Array(obj))
    //        ret = TypedArrayType::UINT8;
    //    else if (JS_IsUint8ClampedArray(obj))
    //        ret = TypedArrayType::UINT8_CLAMPED;
    //    else if (JS_IsUint16Array(obj))
    //        ret = TypedArrayType::UINT16;
    //    else if (JS_IsUint32Array(obj))
    //        ret = TypedArrayType::UINT32;
    //    else if (JS_IsFloat32Array(obj))
    //        ret = TypedArrayType::FLOAT32;
    //    else if (JS_IsFloat64Array(obj))
    //        ret = TypedArrayType::FLOAT64;

    return ret;
}

bool Object::getTypedArrayData(uint8_t **ptr, size_t *length) const {
    size_t byte_offset = 0;
    size_t byte_length = 0;
    size_t bytes_per_element = 0;
    JSValue typedArray = JS_GetTypedArrayBuffer(__cx, _obj, &byte_offset, &byte_length, &bytes_per_element);
    size_t size = 0;
    uint8_t* buf = JS_GetArrayBuffer(__cx, &size, typedArray);
    if (ptr) {
        *ptr = buf;
    }
    if (length) {
        *length = size;
    }
    return true;
}

bool Object::isArray() const {
    return JS_IsArray(__cx, _obj) != 0;
}

bool Object::isArrayBuffer() const {
    bool result = false;
    auto context = __cx;
    JSValue constructor = JS_GetPropertyStr(context, _obj, "constructor");
    JSValue name = JS_GetPropertyStr(context, constructor, "name");
    const char* cName = JS_ToCString(context, name);
    result = !strcmp("ArrayBuffer", cName ? cName : "");
    JS_FreeCString(context, cName);
    JS_FreeValue(context, name);
    JS_FreeValue(context, constructor);
    return result;
}

bool Object::hasProperty(const char *name) const {
    JSAtom atom = JS_NewAtom(__cx, name);
    bool   ret  = false;
    if (JS_HasProperty(__cx, _obj, atom) > 0) {
        ret = true;
    }

    JS_FreeAtom(__cx, atom);
    return ret;
}

bool Object::getArrayBufferData(uint8_t **ptr, size_t *length) const {
    assert(isArrayBuffer());
    assert(ptr != nullptr);
    size_t byteLength{0};
    *ptr = JS_GetArrayBuffer(__cx, &byteLength, _obj);
    if (length != nullptr) {
        *length = byteLength;
    }
    return false;
}

bool Object::getAllKeys(std::vector<std::string> *allKeys) const {
    assert(allKeys != nullptr);
    allKeys->clear();

    uint32_t        len = 0;
    JSPropertyEnum *tab = nullptr;
    const char *    key = nullptr;

    if (JS_GetOwnPropertyNames(__cx, &tab, &len, _obj, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) < 0) {
        return false;
    }

    do {
        allKeys->reserve(len);
        for (uint32_t i = 0; i < len; i++) {
            key = JS_AtomToCString(__cx, tab[i].atom);
            if (key != nullptr) {
                allKeys->emplace_back(key);
                JS_FreeCString(__cx, key);
            } else {
                break;
            }
        }
    } while (false);

    return true;
}

void Object::setPrivateObject(PrivateObjectBase *data) {
    CC_ASSERT_NULL(_privateObject);
    #if CC_DEBUG
    // CC_ASSERT(!NativePtrToObjectMap::contains(data->getRaw()));
    if (data != nullptr) {
        NativePtrToObjectMap::filter(data->getRaw(), _getClass())
            .forEach([&](se::Object *seObj) {
                auto *pri = seObj->getPrivateObject();
                SE_LOGE("Already exists object %s/[%s], trying to add %s/[%s]\n", pri->getName(), typeid(*pri).name(), data->getName(), typeid(*data).name());
        #if JSB_TRACK_OBJECT_CREATION
                SE_LOGE(" previous object created at %s\n", it->second->_objectCreationStackFrame.c_str());
        #endif
                CC_ABORT();
            });
    }
    #endif
    internal::setPrivate(_obj, this);
    _privateObject = data;

    if (data != nullptr) {
        _privateData = data->getRaw();
        NativePtrToObjectMap::emplace(_privateData, this);
    } else {
        _privateData = nullptr;
    }
}

PrivateObjectBase *Object::getPrivateObject() const {
    return _privateObject;
}

void Object::clearPrivateData(bool clearMapping) {
    if (_privateObject != nullptr) {
        if (clearMapping) {
            NativePtrToObjectMap::erase(_privateData, this);
        }
        internal::clearPrivate(_obj);
        delete _privateObject;
        _privateObject = nullptr;
        _privateData = nullptr;
    }
}
void Object::setContext(JSContext *cx) {
    __cx = cx;
}

// static
void Object::cleanup() {
    for (const auto &e : __objectMap) {
        e.first->reset();
    }

    ScriptEngine::getInstance()->addAfterCleanupHook([]() {
        __objectMap.clear();
        const auto &instance = NativePtrToObjectMap::instance();
        for (const auto &e : instance) {
            e.second->decRef();
        }
        NativePtrToObjectMap::clear();
        __cx = nullptr;
    });
}

JSValue Object::_getJSObject() const {
    return _obj;
}

void Object::root() {
    JS_DupValue(__cx, _obj);
    ++_rootCount;
}

void Object::unroot() {
    if (_rootCount > 0) {
        --_rootCount;
//        JS_FreeValue(__cx, _obj);
    }
}

void Object::reset() {
    _obj = JS_UNDEFINED;
}

bool Object::isRooted() const {
    return _rootCount > 0;
}

bool Object::strictEquals(Object *o) const {
    assert(false);
    return false;
}

bool Object::attachObject(Object *obj) {
    assert(obj);

    Object *global = ScriptEngine::getInstance()->getGlobalObject();
    Value   jsbVal;
    if (!global->getProperty("jsb", &jsbVal))
        return false;
    Object *jsbObj = jsbVal.toObject();

    Value func;

    if (!jsbObj->getProperty("registerNativeRef", &func))
        return false;

    ValueArray args;
    args.push_back(Value(this));
    args.push_back(Value(obj));
    func.toObject()->call(args, global);
    return true;
}

bool Object::detachObject(Object *obj) {
    assert(obj);
    Object *global = ScriptEngine::getInstance()->getGlobalObject();
    Value   jsbVal;
    if (!global->getProperty("jsb", &jsbVal))
        return false;
    Object *jsbObj = jsbVal.toObject();

    Value func;

    if (!jsbObj->getProperty("unregisterNativeRef", &func))
        return false;

    ValueArray args;
    args.push_back(Value(this));
    args.push_back(Value(obj));
    func.toObject()->call(args, global);
    return true;
}

std::string Object::toString() const {
    std::string ret;
    if (isFunction() || isArray() || isTypedArray()) {
        internal::forceConvertJsValueToStdString(__cx, _obj, &ret);
    } else if (isArrayBuffer()) {
        ret = "[object ArrayBuffer]";
    } else {
        ret = "[object Object]";
    }
    return ret;
}

} // namespace se

#endif // #if SCRIPT_ENGINE_TYPE == SCRIPT_ENGINE_QUICKJS
