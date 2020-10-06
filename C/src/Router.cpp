
#include <osmscout/GeoCoord.h>
#include <osmscout/Database.h>
#include <osmscout/routing/RoutingService.h>
#include <osmscout/routing/SimpleRoutingService.h>
#include <osmscout/util/Logger.h>

#include "Router.h"
#include "RouterContext.h"

DLL_EXPORT void router_init()
{
    #ifdef ROUTER_DEBUG
    osmscout::log.Debug(true);
    #else
    auto routerLogger = new osmscout::NoOpLogger();
    osmscout::log.SetLogger(routerLogger);
    #endif
}

DLL_EXPORT bool router_new(void** ctx_ptr, const char* db_path)
{
    auto ctx = new RouterContext();
    *ctx_ptr = ctx;

    auto db = std::string(db_path);

    ctx->database = std::make_shared<osmscout::Database>(ctx->databaseParameter);
    if (!ctx->database->Open(db)) {
        ctx->err = "Cannot open database: " + db;
        return false;
    }

    ctx->router = std::make_shared<osmscout::SimpleRoutingService>(ctx->database,
                                                                   ctx->routerParameter,
                                                                   "");

    return true;
};

DLL_EXPORT void router_del(void* ctx_ptr)
{
    if (!ctx_ptr) {
        return;
    }

    auto ctx = static_cast<RouterContext*>(ctx_ptr);
    delete ctx;
};

static void GetCarSpeedTable(std::map<std::string, double>& map)
{
    map["highway_motorway"]=110.0;
    map["highway_motorway_trunk"]=100.0;
    map["highway_motorway_primary"]=70.0;
    map["highway_motorway_link"]=60.0;
    map["highway_motorway_junction"]=60.0;
    map["highway_trunk"]=100.0;
    map["highway_trunk_link"]=60.0;
    map["highway_primary"]=70.0;
    map["highway_primary_link"]=60.0;
    map["highway_secondary"]=60.0;
    map["highway_secondary_link"]=50.0;
    map["highway_tertiary_link"]=55.0;
    map["highway_tertiary"]=55.0;
    map["highway_unclassified"]=50.0;
    map["highway_road"]=50.0;
    map["highway_residential"]=40.0;
    map["highway_roundabout"]=40.0;
    map["highway_living_street"]=10.0;
    map["highway_service"]=30.0;
}

DLL_EXPORT route_calc_result
router_calc(void* ctx_ptr, route_profile profile,
            const point_t* p1, const point_t* p2,
            uint32_t* out_count, const point_t** out_points)
{
    if (!ctx_ptr) {
        return CALC_RESULT_ERROR;
    }

    auto ctx = static_cast<RouterContext*>(ctx_ptr);

    auto router = ctx->router;

    ctx->err.clear();

    if (!router) {
        return CALC_RESULT_ERROR;
    }
    if (!router->IsOpen()) {
        if (!router->Open()) {
            ctx->err = "Cannot open routing database";
            ctx->router = nullptr;
            return CALC_RESULT_ERROR;
        }
    }

    auto typeConfig = ctx->database->GetTypeConfig();
    osmscout::FastestPathRoutingProfile routingProfile{typeConfig};

    switch (profile) {
    case ROUTE_PROFILE_FOOT:
        routingProfile.ParametrizeForFoot(*typeConfig, 5.0);
        break;

    case ROUTE_PROFILE_BIKE:
        routingProfile.ParametrizeForBicycle(*typeConfig, 20.0);
        break;

    case ROUTE_PROFILE_CAR:
        std::map<std::string, double> carSpeedTable;
        GetCarSpeedTable(carSpeedTable);
        routingProfile.ParametrizeForCar(*typeConfig, carSpeedTable, 160.0);
        break;
    }

    osmscout::GeoCoord startCoord{p1->lat, p1->lon};

    #ifdef ROUTER_DEBUG
    osmscout::log.Debug() << startCoord->GetDisplayText();
    #endif

    auto startResult = router->GetClosestRoutableNode(startCoord,
                                                      routingProfile,
                                                      osmscout::Kilometers(1));

    if (!startResult.IsValid()) {
        ctx->err = "Error while searching for routing node near start location!";
        return CALC_RESULT_NODATA;
    }

    osmscout::RoutePosition start = startResult.GetRoutePosition();
    if (start.GetObjectFileRef().GetType() == osmscout::refNode) {
        ctx->err = "Cannot find start node for start location!";
    }

    osmscout::GeoCoord targetCoord{p2->lat, p2->lon};

    #ifdef ROUTER_DEBUG
    osmscout::log.Debug() << targetCoord->GetDisplayText();
    #endif

    auto targetResult = router->GetClosestRoutableNode(targetCoord,
                                                       routingProfile,
                                                       osmscout::Kilometers(1));

    if (!targetResult.IsValid()) {
        ctx->err = "Error while searching for routing node near target location!";
        return CALC_RESULT_NODATA;
    }

    osmscout::RoutePosition target = targetResult.GetRoutePosition();
    if (target.GetObjectFileRef().GetType() == osmscout::refNode) {
        ctx->err = "Cannot find start node for target location!";
    }

    osmscout::RoutingParameter parameter;
    osmscout::RoutingResult result = router->CalculateRoute(routingProfile,
                                                            start,
                                                            target,
                                                            parameter);

    if (!result.Success()) {
        ctx->err = "There was an error while calculating the route!";
        router->Close();
        return CALC_RESULT_ERROR;
    }

    auto routePointsResult = router->TransformRouteDataToPoints(result.GetRoute());

    if (!routePointsResult.Success()) {
        ctx->err = "Error during route conversion";
        return CALC_RESULT_ERROR;
    }

    auto points = routePointsResult.GetPoints()->points;
    ctx->pointsCount = points.size();

    if (ctx->pointsCount == 0) {
        return CALC_RESULT_NODATA;
    }

    auto p = (point_t*)realloc(ctx->points, ctx->pointsCount * sizeof(point_t));
    ctx->points = p;
    for (const auto& point : points) {
        p->lon = point.GetLon();
        p->lat = point.GetLat();
        ++p;
    }

    *out_count = ctx->pointsCount;
    *out_points = ctx->points;

    return CALC_RESULT_OK;
};

DLL_EXPORT void router_clear(void* ctx_ptr)
{
    if (!ctx_ptr) {
        return;
    }

    auto ctx = static_cast<RouterContext*>(ctx_ptr);
    ctx->Clear();
};

DLL_EXPORT const char* router_get_error_message(void* ctx_ptr)
{
    if (!ctx_ptr) {
        return nullptr;
    }

    auto ctx = static_cast<RouterContext*>(ctx_ptr);
    return ctx->err.c_str();
};
