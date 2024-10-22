#include <map>
#include <string>
#include <cassert>

#include <osmscout/db/Database.h>
#include <osmscout/routing/RoutingProfile.h>

#include "Router.h"

static std::map<std::string, double> carSpeedTable = {
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

std::shared_ptr<osmscout::FastestPathRoutingProfile>
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
            routingProfile->ParametrizeForCar(*typeConfig, carSpeedTable, 160.0);
            break;
        }
        default: {
            assert("Unexpected profile value!");
            break;
        }
    }

    return routingProfile;
}
