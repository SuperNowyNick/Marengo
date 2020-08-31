#include "coord.h"

stpCoordF_t stpCoordF_Zero()
{
  stpCoordF_t temp;
  temp.x = fzero();
  temp.y = fzero();
  temp.z = fzero();
  temp.e = fzero();
  return temp;
}

stpCoordF_t stpCoordF_Add(stpCoordF_t a, stpCoordF_t b)
{
  a.x = fadd(a.x, b.x);
  a.y= fadd(a.y, b.y);
  a.z = fadd(a.z, b.z);
  a.e = fadd(a.e, b.e);
  return a;
}

stpCoordF_t stpCoordFSub(stpCoordF_t a, stpCoordF_t b)
{
  stpCoordF_t ret;
  ret.x=fsub(a.x,b.x);
  ret.y=fsub(a.y,b.y);
  ret.z=fsub(a.z,b.z);
  ret.e=fsub(a.e,b.e);
  return ret;
}

uint32_t stpCoordF_Abs(stpCoordF_t a)
{
  return 0; // TODO: Write coordinate length function
}

stpCoord_t stpCoord_Add(stpCoord_t a, stpCoord_t b)
{
  a.x+=b.x;
  a.y+=b.y;
  a.z+=b.z;
  a.e+=b.e;
  return a;
}
stpCoord_t stpCoord_Sub(stpCoord_t a, stpCoord_t b)
{
  a.x-=b.x;
  a.y-=b.y;
  a.z-=b.z;
  a.e-=b.e;
  return a;
}
uint32_t stpCoord_Abs(stpCoord_t a)
{
  uint64_t op = a.x*a.x+a.y*a.y+a.z*a.z;
  return isqrt64(op);
}
stpCoord_t stpCoord_Zero(void)
{
  stpCoord_t a;
  a.x=0;
  a.y=0;
  a.z=0;
  a.e=0;
  return a;
}
