// code = utf-8

#ifndef __Qmath_h
#define __Qmath_h

typedef long _Qtype;

#define _Qmath_E 44536        // e
#define _Qmath_log2E 23637    // log2(e)
#define _Qmath_log10E 7115    // lg10(e)
#define _Qmath_ln2 11356      // ln(e)
#define _Qmath_ln10 37725     // ln(e)
#define _Qmath_PI 51471       // pi
#define _Qmath_PI_2 25735     // pi/2
#define _Qmath_PI_4 12867     // pi/4
#define _Qmath_1_PI 5125      // 1/pi
#define _Qmath_2_PI 10430     // 2/pi
#define _Qmath_2_sqrtPI 18487 // 2/sqrt(pi)
#define _Qmath_sqrt2 23170    // sqrt(2)
#define _Qmath_sqrt1_2 11585  // sqrt(0.5)

#define _Q(A) ((_Qtype)((A) * (1 << 14)))
#define _Q2f(A) ((float)(A) / (1 << 14))
#define _Q2i(A) (A >> 14)
#define _Qfrac(A) (A & ((1 << 14) - 1))

#define _Qmpy2(A) ((A) << 1)
#define _Qmpy4(A) ((A) << 2)
#define _Qmpy8(A) ((A) << 3)
#define _Qmpy16(A) ((A) << 4)
_Qtype _Qmpy(_Qtype A, _Qtype B);

#define _Qdiv2(A) ((A) >> 1)
#define _Qdiv4(A) ((A) >> 2)
#define _Qdiv8(A) ((A) >> 3)
#define _Qdiv16(A) ((A) >> 4)
_Qtype _Qdiv(_Qtype A, _Qtype B);

_Qtype _Qsin(_Qtype A);
_Qtype _Qcos(_Qtype A);
_Qtype _Qtan(_Qtype A);

#endif
