#ifndef HEADER_3F995FB366198164
#define HEADER_3F995FB366198164

#include <osmscout/Database.h>
#include <osmscout/routing/SimpleRoutingService.h>

struct RouterContext {
    osmscout::DatabaseParameter databaseParameter{};
    osmscout::DatabaseRef database = nullptr;

    osmscout::RouterParameter routerParameter{};
    osmscout::SimpleRoutingServiceRef router = nullptr;

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

RouterContext::~RouterContext()
{
    Clear();

    router = nullptr;
    database = nullptr;
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

    err.clear();
}

#endif

