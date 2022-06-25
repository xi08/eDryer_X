// code = utf-8

#include "Qmath.h"
#include <limits.h>

const _Qtype _Qmath_trigLUT1[14] = {16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2};
const _Qtype _Qmath_trigLUT2[14] = {12867, 7596, 4013, 2037, 1022, 511, 255, 127, 63, 31, 15, 7, 3, 1};
const _Qtype _Qmath_trigK = 9949;

/**
 * @brief 定点数乘
 *
 * @param A 被乘数
 * @param B 乘数
 * @return _Qtype 计算结果
 */
_Qtype _Qmpy(_Qtype A, _Qtype B)
{
    signed char i = 14;

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
    if (i >= 0)
        return (A * B) >> i;
    else
        return (A * B) << (-i);
}

/**
 * @brief 定点数除
 *
 * @param A 被除数
 * @param B 除数
 * @return _Qtype 计算结果
 */
_Qtype _Qdiv(_Qtype A, _Qtype B)
{
    signed char i = 14;
    while (A < (LONG_MAX >> 1) && A > (LONG_MIN >> 1) && i)
    {
        A <<= 1;
        i--;
    }
    B >>= i;
    return A / B;
}

/**
 * @brief 定点数求正弦
 *
 * @param A 函数自变量
 * @return _Qtype 计算结果
 */
_Qtype _Qsin(_Qtype A)
{
    signed char sign = 1;
    char i;
    _Qtype x = _Q(1), y = _Q(0), tmp;

    if (A > _Qmath_PI_2)
    {
        A -= _Qmath_PI;
        sign = -1;
    }
    else if (A < -_Qmath_PI_2)
    {
        A += _Qmath_PI;
        sign = -1;
    }
    for (i = 0; i < 14; i++)
    {
        tmp = x;
        if (A > 0)
        {
            x -= _Qmpy(y, _Qmath_trigLUT1[i]);
            y += _Qmpy(tmp, _Qmath_trigLUT1[i]);
            A -= _Qmath_trigLUT2[i];
        }
        else
        {
            x += _Qmpy(y, _Qmath_trigLUT1[i]);
            y -= _Qmpy(tmp, _Qmath_trigLUT1[i]);
            A += _Qmath_trigLUT2[i];
        }
    }
    return sign * (_Qmpy(_Qmath_trigK, y));
}

/**
 * @brief 定点数求余弦
 *
 * @param A 函数自变量
 * @return _Qtype 计算结果
 */
_Qtype _Qcos(_Qtype A)
{
    signed char sign = 1;
    char i;
    _Qtype x = _Q(1), y = _Q(0), tmp;

    if (A > _Qmath_PI_2)
    {
        A -= _Qmath_PI;
        sign = -1;
    }
    else if (A < -_Qmath_PI_2)
    {
        A += _Qmath_PI;
        sign = -1;
    }
    for (i = 0; i < 14; i++)
    {
        tmp = x;
        if (A > 0)
        {
            x -= _Qmpy(y, _Qmath_trigLUT1[i]);
            y += _Qmpy(tmp, _Qmath_trigLUT1[i]);
            A -= _Qmath_trigLUT2[i];
        }
        else
        {
            x += _Qmpy(y, _Qmath_trigLUT1[i]);
            y -= _Qmpy(tmp, _Qmath_trigLUT1[i]);
            A += _Qmath_trigLUT2[i];
        }
    }
    return sign * (_Qmpy(_Qmath_trigK, x));
}

/**
 * @brief 定点数求正切
 *
 * @param A 函数自变量
 * @return _Qtype 计算结果
 */
_Qtype _Qtan(_Qtype A)
{
    signed char i;
    _Qtype x = _Q(1), y = _Q(0), tmp;

    if (A > _Qmath_PI_2)
    {
        A -= _Qmath_PI;
    }
    else if (A < -_Qmath_PI_2)
    {
        A += _Qmath_PI;
    }
    for (i = 0; i < 14; i++)
    {
        tmp = x;
        if (A > 0)
        {
            x -= _Qmpy(y, _Qmath_trigLUT1[i]);
            y += _Qmpy(tmp, _Qmath_trigLUT1[i]);
            A -= _Qmath_trigLUT2[i];
        }
        else
        {
            x += _Qmpy(y, _Qmath_trigLUT1[i]);
            y -= _Qmpy(tmp, _Qmath_trigLUT1[i]);
            A += _Qmath_trigLUT2[i];
        }
    }
    return _Qdiv(y, x);
}

