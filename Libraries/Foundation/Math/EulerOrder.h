#pragma once

#include <cmath>
#include "Utils.h"

//
// Support for 24 angle schemes based on code by Ken Shoemake,
// in "Graphics Gems IV", Academic Press, 1994 
//
// Order type constants, constructors, extractors  
// There are 24 possible conventions, designated by:
//     o EulAxI = axis used initially      
//     o EulPar = parity of axis permutation 
//     o EulRep = repetition of initial axis as last  
//     o EulFrm = frame from which axes are taken
//
// Axes I,J,K will be a permutation of X,Y,Z.
//  Axis H will be either I or K, depending on EulRep.
//  Frame S takes axes from initial static frame. 
// If ord = (AxI=X, Par=Even, Rep=No, Frm=S), then {a,b,c,ord} means Rz(c)Ry(b)Rx(a), where Rz(c)v rotates v around Z by c radians.        
//

#define EulFrmS 0
#define EulFrmR 1
#define EulFrm(ord) ((unsigned)(ord)&1)

#define EulRepNo 0
#define EulRepYes 1
#define EulRep(ord) (((unsigned)(ord)>>1)&1)

#define EulParEven 0
#define EulParOdd 1
#define EulPar(ord) (((unsigned)(ord)>>2)&1)

#define EulSafe "\000\001\002\000"
#define EulNext "\001\002\000\001"

#define EulAxI(ord) ((i32)(EulSafe[(((unsigned)(ord)>>3)&3)]))
#define EulAxJ(ord) ((i32)(EulNext[EulAxI(ord)+(EulPar(ord)==EulParOdd)]))
#define EulAxK(ord) ((i32)(EulNext[EulAxI(ord)+(EulPar(ord)!=EulParOdd)]))
#define EulAxH(ord) ((EulRep(ord)==EulRepNo)?EulAxK(ord):EulAxI(ord))

/* EulGetOrd unpacks all useful information about order simultaneously. */
#define EulGetOrd(ord,i,j,k,h,n,s,f) { \
  unsigned o=ord; \
  f=o&1; o=o>>1; \
  s=o&1; o=o>>1; \
  n=o&1; o=o>>1; \
  i=EulSafe[o&3]; \
  j=EulNext[i+n]; \
  k=EulNext[i+1-n]; \
  h=s?k:i; \
}

/* EulOrd creates an order value between 0 and 23 from 4-tuple choices. */
#define EulOrd(i,p,r,f)    (((((((i)<<1)+(p))<<1)+(r))<<1)+(f))

namespace Math
{
  namespace EulerOrders
  {
    enum EulerOrder
    {
      XYZ = EulOrd(SingleAxes::X, EulParEven, EulRepNo, EulFrmS),
      XYX = EulOrd(SingleAxes::X, EulParEven, EulRepYes, EulFrmS),
      XZY = EulOrd(SingleAxes::X, EulParOdd, EulRepNo, EulFrmS),
      XZX = EulOrd(SingleAxes::X, EulParOdd, EulRepYes, EulFrmS),
      YZX = EulOrd(SingleAxes::Y, EulParEven, EulRepNo, EulFrmS),
      YZY = EulOrd(SingleAxes::Y, EulParEven, EulRepYes, EulFrmS),
      YXZ = EulOrd(SingleAxes::Y, EulParOdd, EulRepNo, EulFrmS),
      YXY = EulOrd(SingleAxes::Y, EulParOdd, EulRepYes, EulFrmS),
      ZXY = EulOrd(SingleAxes::Z, EulParEven, EulRepNo, EulFrmS),
      ZXZ = EulOrd(SingleAxes::Z, EulParEven, EulRepYes, EulFrmS),
      ZYX = EulOrd(SingleAxes::Z, EulParOdd, EulRepNo, EulFrmS),
      ZYZ = EulOrd(SingleAxes::Z, EulParOdd, EulRepYes, EulFrmS),

      XYZs = EulOrd(SingleAxes::X, EulParEven, EulRepNo, EulFrmS),
      XYXs = EulOrd(SingleAxes::X, EulParEven, EulRepYes, EulFrmS),
      XZYs = EulOrd(SingleAxes::X, EulParOdd, EulRepNo, EulFrmS),
      XZXs = EulOrd(SingleAxes::X, EulParOdd, EulRepYes, EulFrmS),
      YZXs = EulOrd(SingleAxes::Y, EulParEven, EulRepNo, EulFrmS),
      YZYs = EulOrd(SingleAxes::Y, EulParEven, EulRepYes, EulFrmS),
      YXZs = EulOrd(SingleAxes::Y, EulParOdd, EulRepNo, EulFrmS),
      YXYs = EulOrd(SingleAxes::Y, EulParOdd, EulRepYes, EulFrmS),
      ZXYs = EulOrd(SingleAxes::Z, EulParEven, EulRepNo, EulFrmS),
      ZXZs = EulOrd(SingleAxes::Z, EulParEven, EulRepYes, EulFrmS),
      ZYXs = EulOrd(SingleAxes::Z, EulParOdd, EulRepNo, EulFrmS),
      ZYZs = EulOrd(SingleAxes::Z, EulParOdd, EulRepYes, EulFrmS),

      ZYXr = EulOrd(SingleAxes::X, EulParEven, EulRepNo, EulFrmR),
      XYXr = EulOrd(SingleAxes::X, EulParEven, EulRepYes, EulFrmR),
      YZXr = EulOrd(SingleAxes::X, EulParOdd, EulRepNo, EulFrmR),
      XZXr = EulOrd(SingleAxes::X, EulParOdd, EulRepYes, EulFrmR),
      XZYr = EulOrd(SingleAxes::Y, EulParEven, EulRepNo, EulFrmR),
      YZYr = EulOrd(SingleAxes::Y, EulParEven, EulRepYes, EulFrmR),
      ZXYr = EulOrd(SingleAxes::Y, EulParOdd, EulRepNo, EulFrmR),
      YXYr = EulOrd(SingleAxes::Y, EulParOdd, EulRepYes, EulFrmR),
      YXZr = EulOrd(SingleAxes::Z, EulParEven, EulRepNo, EulFrmR),
      ZXZr = EulOrd(SingleAxes::Z, EulParEven, EulRepYes, EulFrmR),
      XYZr = EulOrd(SingleAxes::Z, EulParOdd, EulRepNo, EulFrmR),
      ZYZr = EulOrd(SingleAxes::Z, EulParOdd, EulRepYes, EulFrmR),
    };
  }

  class FOUNDATION_API EulerOrder
  {
  public:
    EulerOrders::EulerOrder o;

    EulerOrder      ()
      : o (EulerOrders::XYZs)
    {
    
    }

    EulerOrder      (EulerOrders::EulerOrder v)
      : o (v)
    {
    
    }

    EulerOrder&     operator=(const EulerOrder& v)
    {
      o = v.o;
      return *this;
    }

    bool            operator== (const EulerOrder& v) const
    {
      return o == v.o;
    }

    bool            operator!= (const EulerOrder& v) const
    {
      return (!(*this == v));
    }

    u32    					I () const
    {
      return EulAxI(o);
    }

    u32    					J () const
    {
      return EulAxJ(o);
    }

    u32    					K () const
    {
      return EulAxK(o);
    }

    u32    					H () const
    {
      return EulAxH(o);
    }

    bool            RepeatingAngles () const
    {
      return EulRep(o);
    }

    bool            RotatingFrame () const
    {
      return EulFrm(o);
    }

    bool            OddParity () const
    {
      return EulPar(o);
    }
  };
}
