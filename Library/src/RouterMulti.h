#ifndef ROUTERMULTI_H_INCLUDED
#define ROUTERMULTI_H_INCLUDED

#include "Router.h"
#include "RouterContext.h"

router_result RouterMultiNew(RouterContext* ctx);

router_result RouterMultiCalc(RouterContext* ctx, route_profile profile,
                              const point_t* p1, const point_t* p2,
                              uint32_t* out_count, const point_t** out_points);


#endif // ROUTERMULTI_H_INCLUDED
