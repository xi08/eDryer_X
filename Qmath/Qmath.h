// code = utf-8
#ifndef __Qmath_h
#define __Qmath_h

typedef long _Qtype;

#define _QtypeSize 14

#define _Q(A) ((_Qtype)((A) * (1 << _QtypeSize)))
#define _Q2f(A) ((float)(A) / (1 << _QtypeSize))
#define _Q2i(A) (A >> _QtypeSize)
#define _Qfrac(A) (A & ((1 << _QtypeSize) - 1))

_Qtype _Qmpy(_Qtype A, _Qtype B);
_Qtype _Qdiv(_Qtype A, _Qtype B);

#endif
