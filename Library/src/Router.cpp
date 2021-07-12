#include <cassert>

#include <osmscout/GeoCoord.h>
#include <osmscout/Database.h>
#include <osmscout/routing/RoutingService.h>
#include <osmscout/routing/SimpleRoutingService.h>
#include <osmscout/routing/MultiDBRoutingService.h>
#include <osmscout/util/Logger.h>

#include "Router.h"
#include "RouterContext.h"

static bool debugPerformance = false;

DLL_EXPORT void router_init()
{
    #ifdef ROUTER_DEBUG
    debugPerformance = true;
    osmscout::log.Debug(true);
    osmscout::log.Info(true);
    osmscout::log.Warn(true);
    osmscout::log.Error(true);
    #else
    auto routerLogger = new osmscout::NoOpLogger();
    osmscout::log.SetLogger(routerLogger);
    #endif
}

DLL_EXPORT bool router_new(void** ctx_ptr, const char* db_path)
{
    auto ctx = new RouterContext();
    *ctx_ptr = ctx;

    ctx->isMulti = false;
    ctx->routerParameter.SetDebugPerformance(debugPerformance);

    auto path = std::string(db_path);
    #ifdef ROUTER_DEBUG
    osmscout::log.Debug() << "DataBase: " << path;
    #endif

    ctx->database = std::make_shared<osmscout::Database>(ctx->databaseParameter);
    if (!ctx->database->Open(path)) {
        ctx->err = "Can't open DataBase: " + path;
        return false;
    }

    ctx->router = std::make_shared<osmscout::SimpleRoutingService>(ctx->database,
                                                                   ctx->routerParameter,
                                                                   "");

    return true;
};

DLL_EXPORT bool router_new_multi(void** ctx_ptr, const char* db_path[], uint32_t db_count)
{
    auto ctx = new RouterContext();
    *ctx_ptr = ctx;

    ctx->isMulti = true;
    ctx->routerParameter.SetDebugPerformance(debugPerformance);

    #ifdef ROUTER_DEBUG
    osmscout::log.Debug() << "DataBases count: " << db_count;
    #endif

    for (uint32_t i = 0; i < db_count; i++) {
        auto path = std::string(db_path[i]);

        #ifdef ROUTER_DEBUG
        osmscout::log.Debug() << "DataBase " << i << ": " << path;
        #endif

        osmscout::DatabaseRef database =
            std::make_shared<osmscout::Database>(ctx->databaseParameter);

        if ( !database->Open(path) ) {
            ctx->err = "Can't open DataBase: " + path;
            return false;
        }

        ctx->databases.push_back(database);
    }

    ctx->routerMulti =
        std::make_shared<osmscout::MultiDBRoutingService>(ctx->routerParameter,
                                                          ctx->databases);

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

static std::shared_ptr<osmscout::FastestPathRoutingProfile>
MakeRoutingProfile(const route_profile profile,
                   const osmscout::DatabaseRef& database)
{
    auto typeConfig = database->GetTypeConfig();
    auto routingProfile = std::make_shared<osmscout::FastestPathRoutingProfile>(typeConfig);

    switch (profile) {
        case ROUTE_PROFILE_FOOT: {
            routingProfile->ParametrizeForFoot(*typeConfig, 5.0);
            break;
        }
        case ROUTE_PROFILE_BIKE: {
            routingProfile->ParametrizeForBicycle(*typeConfig, 20.0);
            break;
        }
        case ROUTE_PROFILE_CAR: {
            static std::map<std::string, double> speedTable = {
                {"highway_motorway",          110.0},
                {"highway_motorway_trunk",    100.0},
                {"highway_motorway_primary",  70.0},
                {"highway_motorway_link",     60.0},
                {"highway_motorway_junction", 60.0},
                {"highway_trunk",             100.0},
                {"highway_trunk_link",        60.0},
                {"highway_primary",           70.0},
                {"highway_primary_link",      60.0},
                {"highway_secondary",         60.0},
                {"highway_secondary_link",    50.0},
                {"highway_tertiary_link",     55.0},
                {"highway_tertiary",          55.0},
                {"highway_unclassified",      50.0},
                {"highway_road",              50.0},
                {"highway_residential",       20.0},
                {"highway_roundabout",        40.0},
                {"highway_living_street",     10.0},
                {"highway_service",           30.0},
            };
            routingProfile->ParametrizeForCar(*typeConfig, speedTable, 160.0);
            break;
        }
        default: {
            assert("Unexpected profile value!");
            break;
        }
    }

    return routingProfile;
}

static route_calc_result
RouterCalcMulti(RouterContext* ctx, route_profile profile,
                const point_t* p1, const point_t* p2,
                uint32_t* out_count, const point_t** out_points)
{
    auto router = ctx->routerMulti;
    if (!router) {
        return CALC_RESULT_ERROR;
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
            ctx->err = "Cannot open routing databases";
            return CALC_RESULT_ERROR;
        }
    }

    // calculate
    osmscout::GeoCoord startCoord{p1->lat, p1->lon};

    #ifdef ROUTER_DEBUG
    osmscout::log.Debug() << startCoord.GetDisplayText();
    #endif

    auto startResult = router->GetClosestRoutableNode(startCoord);

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
    osmscout::log.Debug() << targetCoord.GetDisplayText();
    #endif

    auto targetResult = router->GetClosestRoutableNode(targetCoord);

    if (!targetResult.IsValid()) {
        ctx->err = "Error while searching for routing node near target location!";
        return CALC_RESULT_NODATA;
    }

    osmscout::RoutePosition target = targetResult.GetRoutePosition();
    if (target.GetObjectFileRef().GetType() == osmscout::refNode) {
        ctx->err = "Cannot find start node for target location!";
    }

    osmscout::RoutingParameter parameter;
    osmscout::RoutingResult result = router->CalculateRoute(start,
                                                            target,
                                                            parameter);

    if (!result.Success()) {
        ctx->err = "There was an error while calculating the route!";
        ctx->isMultiOpened = false;
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
}

static route_calc_result
RouterCalcSingle(RouterContext* ctx, route_profile profile,
                 const point_t* p1, const point_t* p2,
                 uint32_t* out_count, const point_t** out_points)
{
    auto router = ctx->router;
    if (!router) {
        return CALC_RESULT_ERROR;
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
            ctx->err = "Cannot open routing database";
            ctx->router = nullptr;
            return CALC_RESULT_ERROR;
        }
    }

    // clalc
    osmscout::GeoCoord startCoord{p1->lat, p1->lon};

    #ifdef ROUTER_DEBUG
    osmscout::log.Debug() << startCoord.GetDisplayText();
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
    osmscout::log.Debug() << targetCoord.GetDisplayText();
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

DLL_EXPORT route_calc_result
router_calc(void* ctx_ptr, route_profile profile,
            const point_t* p1, const point_t* p2,
            uint32_t* out_count, const point_t** out_points)
{
    if (!ctx_ptr) {
        return CALC_RESULT_ERROR;
    }

    auto ctx = static_cast<RouterContext*>(ctx_ptr);

    ctx->err.clear();

    if (ctx->isMulti){
        return RouterCalcMulti(ctx, profile, p1, p2, out_count, out_points);
    } else {
        return RouterCalcSingle(ctx, profile, p1, p2, out_count, out_points);
    }
}

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
