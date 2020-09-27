#include "osmscout_route.h"

#include <osmscout/GeoCoord.h>
#include <osmscout/Database.h>
#include <osmscout/routing/SimpleRoutingService.h>

struct RouterContext {
    osmscout::DatabaseParameter databaseParameter;
    osmscout::DatabaseRef database;

    osmscout::RouterParameter routerParameter;
    osmscout::SimpleRoutingServiceRef router;

    uint32_t pointsCount;
    point_t* points;

    std::string err;
};

DLL_EXPORT bool router_new(void** ctx_ptr, const char* db_path)
{
    auto ctx = new(std::nothrow) RouterContext();
    *ctx_ptr = ctx;

    if (!ctx) {
        // memory allocation error
        return false;
    }

    ctx->pointsCount = 0;
    ctx->points = nullptr;
    ctx->err.clear();

    ctx->database = std::make_shared<osmscout::Database>(ctx->databaseParameter);

    if ( !ctx->database->Open(std::string(db_path)) ) {
        ctx->err = "Cannot open database";
        return false;
    }

    ctx->router = std::make_shared<osmscout::SimpleRoutingService>(ctx->database,
                                                                   ctx->routerParameter,
                                                                   "");

    return true;
};

DLL_EXPORT bool router_del(void* ctx_ptr)
{
    if (!ctx_ptr) {
        return false;
    }

    auto ctx = static_cast<RouterContext*>(ctx_ptr);

    router_clear(ctx_ptr);
    ctx->router=nullptr;
    ctx->database=nullptr;
    delete ctx;

    return true;
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

DLL_EXPORT bool router_calc(void* ctx_ptr, route_profile profile,
    const point_t* p1, const point_t* p2,
    uint32_t *out_count, const point_t **out_points)
{

    if (!ctx_ptr) {
        return false;
    }

    auto ctx = static_cast<RouterContext*>(ctx_ptr);
    auto router = ctx->router;

    ctx->err.clear();

    if (!router) {
        return false;
    }
    if (!router->IsOpen()) {
        if (!router->Open()) {
            ctx->err = "Cannot open routing database";
            ctx->router=nullptr;
            return false;
        }
    }

    auto typeConfig=ctx->database->GetTypeConfig();
    auto routingProfile=std::make_shared<osmscout::FastestPathRoutingProfile>(typeConfig);

    std::map<std::string, double> carSpeedTable;

    switch (profile) {
    case ROUTE_PROFILE_FOOT:
        routingProfile->ParametrizeForFoot(*typeConfig, 5.0);
        break;

    case ROUTE_PROFILE_BIKE:
        routingProfile->ParametrizeForBicycle(*typeConfig, 20.0);
        break;

    case ROUTE_PROFILE_CAR:
        GetCarSpeedTable(carSpeedTable);
        routingProfile->ParametrizeForCar(*typeConfig, carSpeedTable, 160.0);
        break;
    }

    osmscout::GeoCoord _start(p1->lat, p1->lon);
    auto startResult=router->GetClosestRoutableNode(_start,
                                                    *routingProfile,
                                                    osmscout::Kilometers(1));

    if (!startResult.IsValid()) {
        ctx->err = "Error while searching for routing node near start location!";
        return false;
    }

    osmscout::RoutePosition start=startResult.GetRoutePosition();
    if (start.GetObjectFileRef().GetType()==osmscout::refNode) {
        ctx->err = "Cannot find start node for start location!";
    }

    osmscout::GeoCoord _target(p2->lat, p2->lon);
    auto targetResult=router->GetClosestRoutableNode(_target,
                                                     *routingProfile,
                                                     osmscout::Kilometers(1));

    if (!targetResult.IsValid()) {
        ctx->err = "Error while searching for routing node near target location!";
        return false;
    }

    osmscout::RoutePosition target=targetResult.GetRoutePosition();
    if (target.GetObjectFileRef().GetType()==osmscout::refNode) {
        ctx->err = "Cannot find start node for target location!";
    }

    osmscout::RoutingParameter parameter;
    osmscout::RoutingResult result=router->CalculateRoute(*routingProfile,
                                                          start,
                                                          target,
                                                          parameter);

    if (!result.Success()) {
        ctx->err = "There was an error while calculating the route!";
        router->Close();
        return false;
    }

    osmscout::RoutePointsResult routePointsResult=router->TransformRouteDataToPoints(result.GetRoute());

    if (!routePointsResult.Success()) {
        ctx->err = "Error during route conversion";
        return false;
    }

    std::vector<osmscout::Point> points = routePointsResult.GetPoints()->points;
    ctx->pointsCount = points.size();

    if (ctx->pointsCount > 0) {
        point_t* p = (point_t*)realloc(ctx->points, ctx->pointsCount * sizeof(point_t));
        ctx->points = p;
        for (const auto& point : points) {
            p->lon = point.GetLon();
            p->lat = point.GetLat();
            ++p;
        }

        *out_count = ctx->pointsCount;
        *out_points = ctx->points;
        return true;
    }
    else {
      return false;
    }
};

DLL_EXPORT bool router_clear(void* ctx_ptr)
{
    if (!ctx_ptr) {
        return false;
    }

    auto ctx = static_cast<RouterContext*>(ctx_ptr);

    if (ctx->points) {
        free(ctx->points);
        ctx->points = nullptr;
    }
    ctx->pointsCount = 0;

    if (ctx->router && ctx->router->IsOpen()) {
        ctx->router->Close();
    }

    ctx->err.clear();

    return true;
};

DLL_EXPORT const char* router_get_error_message(void* ctx_ptr)
{
    if (!ctx_ptr) {
        return nullptr;
    }

    auto ctx = static_cast<RouterContext*>(ctx_ptr);
    return ctx->err.c_str();
};
