#ifndef ROUTERSINGLE_H_INCLUDED
#define ROUTERSINGLE_H_INCLUDED

#include "Router.h"
#include "RouterContext.h"

router_result RouterSingleNew(RouterContext* ctx);

router_result RouterSingleCalc(RouterContext* ctx, route_profile profile,
                               const point_t* p1, const point_t* p2,
                               uint32_t* out_count, const point_t** out_points);

#endif // ROUTERSINGLE_H_INCLUDED
