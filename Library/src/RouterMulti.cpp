
#include <osmscout/GeoCoord.h>
#include <osmscout/db/Database.h>
#include <osmscout/routing/RoutingService.h>
#include <osmscout/routing/MultiDBRoutingService.h>
#include <osmscout/log/Logger.h>

#include "Router.h"
#include "RouterContext.h"
#include "RouterOptions.h"
#include "RoutingProfileBuilder.h"

router_result RouterMultiNew(RouterContext* ctx)
{
    for (uint32_t i = 0; i < ctx->opt->databasePath.size(); i++) {
        auto path = ctx->opt->databasePath[i];

        #ifdef ROUTER_DEBUG
        osmscout::log.Debug() << "DataBase " << i << ": " << path;
        #endif

        osmscout::DatabaseRef database =
            std::make_shared<osmscout::Database>(ctx->databaseParameter);

        if ( !database->Open(path) ) {
            osmscout::log.Error() << "Cannot open DB: " << path;
            return ROUTER_RESULT_ERROR;
        }

        ctx->databases.push_back(database);
    }

    ctx->routerMulti =
        std::make_shared<osmscout::MultiDBRoutingService>(ctx->routerParameter,
                                                          ctx->databases);

    return ROUTER_RESULT_OK;
};

router_result RouterMultiCalc(RouterContext* ctx, route_profile profile,
                              const point_t* p1, const point_t* p2,
                              uint32_t* out_count, const point_t** out_points)
{
    auto router = ctx->routerMulti;
    if (!router) {
        return ROUTER_RESULT_ERROR;
    }

    // prepare profile builder
    if (ctx->prevProfile != profile) {

        if (ctx->isMultiOpened){
            ctx->isMultiOpened = false;
            router->Close();
        }

        // builder: lambda expression which captures "profile" param by value
        ctx->profileBuilderMulti =
            [profile](const osmscout::DatabaseRef& database){
                return MakeRoutingProfile(profile, database);
            };

        ctx->prevProfile = profile;
    }

    // open router
    if (!ctx->isMultiOpened){
        ctx->isMultiOpened = router->Open(ctx->profileBuilderMulti);
        if (!ctx->isMultiOpened){
            osmscout::log.Error() << "Router open failed!";
            return ROUTER_RESULT_ERROR;
        }
    }

    auto closesNodeRadius = osmscout::Meters(ctx->opt->closestRoutableNodeRadius);

    // calculate
    osmscout::GeoCoord startCoord{p1->lat, p1->lon};

    #ifdef ROUTER_DEBUG
    osmscout::log.Debug() << "Start: " << startCoord.GetDisplayText();
    #endif

    auto startResult = router->GetClosestRoutableNode(startCoord,
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
    osmscout::RoutingResult result = router->CalculateRoute(start,
                                                            target,
                                                            std::nullopt,
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
}
