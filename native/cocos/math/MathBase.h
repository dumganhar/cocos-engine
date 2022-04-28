/****************************************************************************
 Copyright (c) 2017-2021 Xiamen Yaji Software Co., Ltd.

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

#ifndef __CCMATHBASE_H__
#define __CCMATHBASE_H__

#include <memory>
#include "base/Macros.h"
#include "base/std/container/string.h"
/**
 * @addtogroup base
 * @{
 */

/**Util macro for conversion from degrees to radians.*/
#define MATH_DEG_TO_RAD(x) ((x)*0.0174532925f)
/**Util macro for conversion from radians to degrees.*/
#define MATH_RAD_TO_DEG(x) ((x)*57.29577951f)
/**
@{ Util macro for const float such as epsilon, small float and float precision tolerance.
*/
#define MATH_FLOAT_SMALL 1.0e-37f
#define MATH_TOLERANCE   2e-37f
#define MATH_PIOVER2     1.57079632679489661923f
#define MATH_EPSILON     0.000001f
/**@}*/

//#define MATH_PIOVER4                0.785398163397448309616f
//#define MATH_PIX2                   6.28318530717958647693f
//#define MATH_E                      2.71828182845904523536f
//#define MATH_LOG10E                 0.4342944819032518f
//#define MATH_LOG2E                  1.442695040888963387f
//#define MATH_PI                     3.14159265358979323846f
//#define MATH_RANDOM_MINUS1_1()      ((2.0f*((float)rand()/RAND_MAX))-1.0f)      // Returns a random float between -1 and 1.
//#define MATH_RANDOM_0_1()           ((float)rand()/RAND_MAX)                    // Returns a random float between 0 and 1.
//#define MATH_CLAMP(x, lo, hi)       ((x < lo) ? lo : ((x > hi) ? hi : x))
//#ifndef M_1_PI
//#define M_1_PI                      0.31830988618379067154

#ifdef __cplusplus
    #define NS_CC_MATH_BEGIN namespace cc {
    #define NS_CC_MATH_END   }
    #define USING_NS_CC_MATH using namespace cc
#else
    #define NS_CC_MATH_BEGIN
    #define NS_CC_MATH_END
    #define USING_NS_CC_MATH
#endif

NS_CC_MATH_BEGIN

template <typename T, typename Enable = std::enable_if_t<std::is_class<T>::value>>
struct Hasher final { uint32_t operator()(const T &info) const; };

// make this boost::hash compatible
template <typename T, typename Enable = std::enable_if_t<std::is_class<T>::value>>
uint32_t hash_value(const T &info) { return Hasher<T>()(info); } // NOLINT(readability-identifier-naming)

NS_CC_MATH_END

/**
 * end of base group
 * @}
 */

#endif // __CCMATHBASE_H__
