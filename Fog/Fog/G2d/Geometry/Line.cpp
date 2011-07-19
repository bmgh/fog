// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Internals_p.h>
#include <Fog/G2d/Geometry/Line.h>

namespace Fog {

// ============================================================================
// [Fog::Line - Intersect]
// ============================================================================

template<typename NumT>
static uint32_t FOG_CDECL LineT_intersect(NumT_(Point)* dst,
  const NumT_(Point)* lineA,
  const NumT_(Point)* lineB)
{
  NumT_(Point) ptA = lineA[1] - lineA[0];
  NumT_(Point) ptB = lineB[1] - lineB[0];

  NumT d = ptA.y * ptB.x - ptA.x * ptB.y;
  if (d == NumT(0.0) || !Math::isFinite(d)) return LINE_INTERSECTION_NONE;

  d = Math::recip(d);

  NumT_(Point) off = lineA[0] - lineB[0];
  NumT t = (ptB.y * off.x - ptB.x * off.y) * d;
  dst->set(lineA[0].x + ptA.x * t, lineA[0].y + ptA.y * t);

  if (t < NumT(0.0) && t > NumT(1.0)) return LINE_INTERSECTION_UNBOUNDED;
  t = (ptA.x * off.y - ptA.y * off.x) * d;
  if (t < NumT(0.0) && t > NumT(1.0)) return LINE_INTERSECTION_UNBOUNDED;

  return LINE_INTERSECTION_BOUNDED;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Line_init(void)
{
  _api.linef.intersect = LineT_intersect<float>;
  _api.lined.intersect = LineT_intersect<double>;
}

} // Fog namespace
