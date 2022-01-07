
#include "RouterContext.h"

RouterContext::~RouterContext()
{
    Clear();

    router = nullptr;
    routerMulti = nullptr;

    database = nullptr;

    for (auto &db: databases) {
        db->Close();
        db.reset();
    }
}

void RouterContext::Clear()
{
    if (points) {
        free(points);
        points = nullptr;
    }
    pointsCount = 0;

    if (router && router->IsOpen()) {
        router->Close();
    }
    if (database) {
        database->FlushCache();
    }

    if (routerMulti && isMultiOpened) {
        isMultiOpened = false;
        routerMulti->Close();
    }
    for (auto &db: databases) {
        db->FlushCache();
    }

    err.clear();

    prevProfile = ROUTE_PROFILE_UNDEF;
    routingProfileSingle = nullptr;
    profileBuilderMulti = nullptr;
}
