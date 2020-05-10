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
  a.x = fsub(a.x, b.x);
  a.y= fsub(a.y, b.y);
  a.z = fsub(a.z, b.z);
  a.e = fsub(a.e, b.e);
  return a;
}

stpCoordF_t stpCoordFSub(stpCoordF_t a, stpCoordF_t b)
{
  stpCoordF_t ret;
  ret.x=fsub(a.x,b.x);
  ret.y=fsub(a.y,b.y);
  ret.z=fsub(a.z,b.z);
  ret.e=fsub(a.stpE,b.stpE);
  return ret;
}

float_t stpCoordF_Abs(stpCoordF_t a)
{
  return 0; // TODO: Write coordinate length function
}
