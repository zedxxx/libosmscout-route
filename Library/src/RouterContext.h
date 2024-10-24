#ifndef HEADER_3F995FB366198164
#define HEADER_3F995FB366198164

#include <osmscout/db/Database.h>
#include <osmscout/routing/SimpleRoutingService.h>
#include <osmscout/routing/MultiDBRoutingService.h>

#include "Router.h"
#include "RouterOptions.h"

struct RouterContext {
    const RouterOptions* opt = nullptr;

    osmscout::DatabaseParameter databaseParameter{};
    osmscout::DatabaseRef database = nullptr;

    std::vector<osmscout::DatabaseRef> databases{};

    osmscout::RouterParameter routerParameter{};
    osmscout::SimpleRoutingServiceRef router = nullptr;
    osmscout::MultiDBRoutingServiceRef routerMulti = nullptr;

    bool isMulti = false;
    bool isMultiOpened = false;

    route_profile prevProfile = ROUTE_PROFILE_UNDEF;
    std::shared_ptr<osmscout::FastestPathRoutingProfile> routingProfileSingle = nullptr;
    osmscout::MultiDBRoutingService::RoutingProfileBuilder profileBuilderMulti = nullptr;

    uint32_t pointsCount = 0;
    point_t* points = nullptr;

    std::string err{};

    RouterContext() = default;

    RouterContext(const RouterContext&) = delete;
    RouterContext(RouterContext&&) = delete;
    RouterContext& operator=(const RouterContext&) = delete;
    RouterContext& operator=(RouterContext&&) = delete;

    ~RouterContext();

    void Clear();
};

#endif

