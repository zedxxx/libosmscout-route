
#include <osmscout/GeoCoord.h>
#include <osmscout/Database.h>
#include <osmscout/routing/RoutingService.h>
#include <osmscout/routing/SimpleRoutingService.h>
#include <osmscout/util/Logger.h>

#include "Router.h"
#include "RouterContext.h"
#include "RouterOptions.h"
#include "RoutingProfileBuilder.h"

router_result RouterSingleNew(RouterContext* ctx)
{
    ctx->database = std::make_shared<osmscout::Database>(ctx->databaseParameter);

    auto path = ctx->opt->databasePath[0];
    if ( !ctx->database->Open(path) ) {
        osmscout::log.Error() << "Cannot open DB: " << path;
        return ROUTER_RESULT_ERROR;
    }

    ctx->router = std::make_shared<osmscout::SimpleRoutingService>(ctx->database,
                                                                   ctx->routerParameter,
                                                                   osmscout::RoutingService::DEFAULT_FILENAME_BASE);

    return ROUTER_RESULT_OK;
};

router_result RouterSingleCalc(RouterContext* ctx, route_profile profile,
                               const point_t* p1, const point_t* p2,
                               uint32_t* out_count, const point_t** out_points)
{
    auto router = ctx->router;
    if (!router) {
        return ROUTER_RESULT_ERROR;
    }

    // prepare profile
    if (ctx->prevProfile != profile) {
        ctx->routingProfileSingle = MakeRoutingProfile(profile, ctx->database);
        ctx->prevProfile = profile;
    }
    auto routingProfile = *ctx->routingProfileSingle;

    // open router
    if (!router->IsOpen()) {
        if (!router->Open()) {
            osmscout::log.Error() << "Router open failed!";
            ctx->router = nullptr;
            return ROUTER_RESULT_ERROR;
        }
    }

    auto closesNodeRadius = osmscout::Meters(ctx->opt->closestRoutableNodeRadius);

    // clalc
    osmscout::GeoCoord startCoord{p1->lat, p1->lon};

    #ifdef ROUTER_DEBUG
    osmscout::log.Debug() << "Start: " << startCoord.GetDisplayText();
    #endif

    auto startResult = router->GetClosestRoutableNode(startCoord,
                                                      routingProfile,
                                                      closesNodeRadius);

    if (!startResult.IsValid()) {
        return ROUTER_RESULT_NODATA_START;
    }

    osmscout::RoutePosition start = startResult.GetRoutePosition();

    #ifdef ROUTER_DEBUG
    if (start.GetObjectFileRef().GetType() == osmscout::refNode) {
        osmscout::log.Debug() << "Cannot find start node for start location!";
    }
    #endif

    osmscout::GeoCoord targetCoord{p2->lat, p2->lon};

    #ifdef ROUTER_DEBUG
    osmscout::log.Debug() << "Target: " << targetCoord.GetDisplayText();
    #endif

    auto targetResult = router->GetClosestRoutableNode(targetCoord,
                                                       routingProfile,
                                                       closesNodeRadius);

    if (!targetResult.IsValid()) {
        return ROUTER_RESULT_NODATA_TARGET;
    }

    osmscout::RoutePosition target = targetResult.GetRoutePosition();

    #ifdef ROUTER_DEBUG
    if (target.GetObjectFileRef().GetType() == osmscout::refNode) {
        osmscout::log.Debug() << "Cannot find start node for target location!";
    }
    #endif

    osmscout::RoutingParameter parameter;
    osmscout::RoutingResult result = router->CalculateRoute(routingProfile,
                                                            start,
                                                            target,
                                                            parameter);

    if (!result.Success()) {
        return ROUTER_RESULT_NODATA_ROUTE;
    }

    auto routePointsResult = router->TransformRouteDataToPoints(result.GetRoute());

    if (!routePointsResult.Success()) {
        osmscout::log.Error() << "Error during transform route data to points!";
        return ROUTER_RESULT_ERROR;
    }

    auto points = routePointsResult.GetPoints()->points;
    ctx->pointsCount = points.size();

    if (ctx->pointsCount != 0) {
        auto p = (point_t*)realloc(ctx->points, ctx->pointsCount * sizeof(point_t));
        ctx->points = p;
        for (const auto& point : points) {
            p->lon = point.GetLon();
            p->lat = point.GetLat();
            ++p;
        }
    }

    *out_count = ctx->pointsCount;
    *out_points = ctx->points;

    return ROUTER_RESULT_OK;
};
