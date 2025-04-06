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

#include "quickjs.h"

#define USE_PRIMJS 0

#if USE_PRIMJS

#ifndef ENABLE_COMPATIBLE_MM
#error "ENABLE_COMPATIBLE_MM was not defined!"
#endif

#if ENABLE_COMPATIBLE_MM != 1
#error "ENABLE_COMPATIBLE_MM was not enabled!"
#endif

#include "quickjs/include/quickjs-inner.h"

static inline LEPUS_BOOL LEPUS_IsBigInt(LEPUSContext *ctx, LEPUSValueConst v)
{
    int tag = LEPUS_VALUE_GET_TAG(v);
    return tag == LEPUS_TAG_BIG_INT;
}

#define JS_TYPED_ARRAY_UINT8C JS_CLASS_UINT8C_ARRAY        /* u.array (typed_array) */
#define JS_TYPED_ARRAY_INT8 JS_CLASS_INT8_ARRAY          /* u.array (typed_array) */
#define JS_TYPED_ARRAY_UINT8  JS_CLASS_UINT8_ARRAY         /* u.array (typed_array) */
#define JS_TYPED_ARRAY_INT16  JS_CLASS_INT16_ARRAY         /* u.array (typed_array) */
#define JS_TYPED_ARRAY_UINT16  JS_CLASS_UINT16_ARRAY        /* u.array (typed_array) */
#define JS_TYPED_ARRAY_INT32  JS_CLASS_INT32_ARRAY         /* u.array (typed_array) */
#define JS_TYPED_ARRAY_UINT32  JS_CLASS_UINT32_ARRAY        /* u.array (typed_array) */
#define JS_TYPED_ARRAY_FLOAT32  JS_CLASS_FLOAT32_ARRAY /* u.array (typed_array) */
#define JS_TYPED_ARRAY_FLOAT64  JS_CLASS_FLOAT64_ARRAY /* u.array (typed_array) */

#define JSValue LEPUSValue
#define JSRuntime LEPUSRuntime
#define JSContext LEPUSContext
#define JSValueConst LEPUSValueConst
#define JSCFunction LEPUSCFunction
#define JSClassFinalizer LEPUSClassFinalizer
#define JS_UNDEFINED LEPUS_UNDEFINED
#define JS_NULL LEPUS_NULL
#define JS_BOOL LEPUS_BOOL
#define JSClassID LEPUSClassID
#define JSClassDef LEPUSClassDef
#define JSCFunctionListEntry LEPUSCFunctionListEntry
#define JS_RunGC LEPUS_RunGC
#define JS_GetOpaqueDontCheckClassId LEPUS_GetOpaqueDontCheckClassId

#define JS_IsNativeObject LEPUS_IsNativeObject
#define JS_NewClassID LEPUS_NewClassID
#define JS_NewClass LEPUS_NewClass
#define JS_GetRuntime LEPUS_GetRuntime
#define JS_NewObjectProtoClass LEPUS_NewObjectProtoClass
#define JS_NewObject LEPUS_NewObject
#define JS_SetPropertyFunctionList LEPUS_SetPropertyFunctionList
#define JS_NewCFunction2 LEPUS_NewCFunction2
#define JS_CFUNC_constructor LEPUS_CFUNC_constructor
//#define JS_SetConstructor LEPUS_SetConstructor
#define JS_SetClassProto LEPUS_SetClassProto
#define JS_SetPropertyStr LEPUS_SetPropertyStr
#define JS_FreeValue(...) // LEPUS_FreeValue
#define JS_DupValue LEPUS_DupValue
#define JS_Call LEPUS_Call

#define JS_GetPropertyStr LEPUS_GetPropertyStr
#define JS_ThrowSyntaxError LEPUS_ThrowSyntaxError
#define JS_SetOpaque LEPUS_SetOpaque
#define JS_ToCString LEPUS_ToCString
#define JS_FreeCString(...) //LEPUS_FreeCString
#define JS_NewFloat64 LEPUS_NewFloat64
#define JS_NewStringLen LEPUS_NewStringLen
#define JS_NewBool LEPUS_NewBool
#define JS_NewBigInt64 LEPUS_NewBigInt64
#define JS_IsNumber LEPUS_IsNumber
#define JS_IsString LEPUS_IsString
#define JS_IsBool LEPUS_IsBool
#define JS_ToBool LEPUS_ToBool
#define JS_IsObject LEPUS_IsObject
#define JS_IsNull LEPUS_IsNull
#define JS_IsUndefined LEPUS_IsUndefined
#define JS_IsBigInt LEPUS_IsBigInt
#define JS_ToBigInt64 LEPUS_ToBigInt64
#define JS_ToFloat64 LEPUS_ToFloat64
#define JS_IsObject LEPUS_IsObject
#define JS_ToUint32 LEPUS_ToUint32
#define JS_IsFunction LEPUS_IsFunction
#define JS_NewRuntime LEPUS_NewRuntime
#define JS_NewContext LEPUS_NewContext
#define JS_FreeContext LEPUS_FreeContext
#define JS_FreeRuntime LEPUS_FreeRuntime
#define JS_SetMaxStackSize LEPUS_SetMaxStackSize
#define JS_AddIntrinsicBigFloat LEPUS_AddIntrinsicBigFloat
#define JS_AddIntrinsicBigDecimal LEPUS_AddIntrinsicBigDecimal
#define JS_AddIntrinsicOperators LEPUS_AddIntrinsicOperators
#define JS_EnableBignumExt LEPUS_EnableBignumExt
#define JS_GetGlobalObject LEPUS_GetGlobalObject
#define JS_Eval LEPUS_Eval
#define JS_IsException LEPUS_IsException
#define JS_ExecutePendingJob LEPUS_ExecutePendingJob
#define JS_NewError LEPUS_NewError
#define JS_NewArray LEPUS_NewArray
#define JS_SetPropertyUint32 LEPUS_SetPropertyUint32
#define JS_NewArrayBufferCopy LEPUS_NewArrayBufferCopy
#define JS_NewArrayBuffer LEPUS_NewArrayBuffer
#define JS_NewInt64 LEPUS_NewInt64
#define JS_NewTypedArray LEPUS_NewTypedArray
#define JS_GetTypedArrayBuffer LEPUS_GetTypedArrayBuffer
#define JS_GetArrayBuffer LEPUS_GetArrayBuffer
#define JS_GetTypedArrayBuffer LEPUS_GetTypedArrayBuffer
#define JS_GetOwnPropertyNames LEPUS_GetOwnPropertyNames
#define JS_AtomToCString LEPUS_AtomToCString
#define js_free(...) //lepus_free
#define JS_ParseJSON LEPUS_ParseJSON
#define JS_NewAtom LEPUS_NewAtom
#define JS_HasProperty LEPUS_HasProperty
#define JS_GetProperty LEPUS_GetProperty
#define JS_FreeAtom(...) //LEPUS_FreeAtom
#define JS_DefinePropertyValueStr LEPUS_DefinePropertyValueStr
#define JS_PROP_WRITABLE LEPUS_PROP_WRITABLE
#define JS_PROP_CONFIGURABLE LEPUS_PROP_CONFIGURABLE
#define JS_PROP_ENUMERABLE LEPUS_PROP_ENUMERABLE
#define JS_DEF_CFUNC LEPUS_DEF_CFUNC
#define JS_CFUNC_generic LEPUS_CFUNC_generic
#define JS_DEF_CGETSET LEPUS_DEF_CGETSET
#define JS_CFUNC_DEF LEPUS_CFUNC_DEF
#define JS_EVAL_TYPE_GLOBAL LEPUS_EVAL_TYPE_GLOBAL
#define JS_IsError LEPUS_IsError
#define JS_IsUninitialized LEPUS_IsUninitialized
#define JS_GetException LEPUS_GetException
#define JS_GetPropertyUint32 LEPUS_GetPropertyUint32
#define JS_IsArray LEPUS_IsArray
#define JS_IsArrayBuffer LEPUS_IsArrayBuffer
#define JSPropertyEnum LEPUSPropertyEnum
#define JS_GPN_STRING_MASK LEPUS_GPN_STRING_MASK
#define JS_GPN_ENUM_ONLY LEPUS_GPN_ENUM_ONLY

#define JSTypedArrayEnum LEPUSClassID
    
#endif


using JSPropGetter = JSValue (*)(JSContext *ctx, JSValueConst this_val);
using JSPropSetter = JSValue (*)(JSContext *ctx, JSValueConst this_val, JSValueConst val);

#define SE_JS_CFUNC_DEF(name, length, func1) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE | JS_PROP_ENUMERABLE, JS_DEF_CFUNC, 0, .u = { .func = { length, JS_CFUNC_generic, { .generic = func1 } } } }

#define SE_JS_CGETSET_DEF(name, fgetter, fsetter) { name, JS_PROP_CONFIGURABLE | JS_PROP_ENUMERABLE, JS_DEF_CGETSET, 0, .u = { .getset = { .get = { .getter = fgetter }, .set = { .setter = fsetter } } } }

#include "../PrivateObject.h"
#include "HelperMacros.h"

#include <assert.h>
#include <chrono>
#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <vector>
