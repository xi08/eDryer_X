// code = utf-8

#include "Qmath.h"
#include <limits.h>

/**
 * @brief 定点数乘
 *
 * @param A 被乘数
 * @param B 乘数
 * @return _Qtype 结果
 */
_Qtype _Qmpy(_Qtype A, _Qtype B)
{
    signed char i = _QtypeSize;

    while (A > SHRT_MAX || A < SHRT_MIN)
    {
        A >>= 1;
        i--;
    }
    while (B > SHRT_MAX || B < SHRT_MIN)
    {
        B >>= 1;
        i--;
    }
    if (i > 0)
        return (A * B) >> i;
    if (i < 0)
        return (A * B) << (-i);
}

/**
 * @brief 定点数除
 *
 * @param A 被除数
 * @param B 除数
 * @return _Qtype 结果
 */
_Qtype _Qdiv(_Qtype A, _Qtype B)
{
    signed char i = _QtypeSize;
    while (A < (LONG_MAX >> 1) && A > (LONG_MIN >> 1) && i)
    {
        A <<= 1;
        i--;
    }
    B >>= i;
    return A / B;
}
