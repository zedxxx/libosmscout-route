
#include <osmscout/util/Logger.h>

#include "Router.h"
#include "RouterContext.h"
#include "RouterOptions.h"
#include "RouterSingle.h"
#include "RouterMulti.h"
#include "Version.h"

static std::ostringstream logStream;
static bool debugPerformance = false;

DLL_EXPORT void router_init()
{
    #ifdef ROUTER_DEBUG
    auto logger = new osmscout::ConsoleLogger();

    debugPerformance = true;
    osmscout::log.Debug(true);
    osmscout::log.Info(true);
    osmscout::log.Warn(true);
    osmscout::log.Error(true);
    #else
    auto logger = new osmscout::StreamLogger(logStream, logStream);

    osmscout::log.Debug(false);
    osmscout::log.Info(false);
    osmscout::log.Warn(true);
    osmscout::log.Error(true);
    #endif

    osmscout::log.SetLogger(logger);
}

DLL_EXPORT router_result router_new(void** ctx_ptr, const void* opt_ptr)
{
    logStream.str("");

    if (!opt_ptr) {
        return ROUTER_RESULT_ERROR;
    }

    auto ctx = new RouterContext();
    *ctx_ptr = ctx;

    auto opt = static_cast<const RouterOptions*>(opt_ptr);
    ctx->opt = opt;

    ctx->isMulti = opt->databasePath.size() > 1;
    ctx->routerParameter.SetDebugPerformance(debugPerformance);

    return ctx->isMulti ? RouterMultiNew(ctx) : RouterSingleNew(ctx);
}

DLL_EXPORT void router_del(void* ctx_ptr)
{
    if (!ctx_ptr) {
        return;
    }

    auto ctx = static_cast<RouterContext*>(ctx_ptr);
    delete ctx;
};

DLL_EXPORT router_result
router_calc(void* ctx_ptr, route_profile profile,
            const point_t* p1, const point_t* p2,
            uint32_t* out_count, const point_t** out_points)
{
    logStream.str("");

    if (!ctx_ptr) {
        return ROUTER_RESULT_ERROR;
    }

    auto ctx = static_cast<RouterContext*>(ctx_ptr);

    ctx->err.clear();

    if (ctx->isMulti) {
        return RouterMultiCalc(ctx, profile, p1, p2, out_count, out_points);
    } else {
        return RouterSingleCalc(ctx, profile, p1, p2, out_count, out_points);
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
    ctx->err = logStream.str();
    return ctx->err.c_str();
};

static const router_version_t router_version = {
    LIBOSMSCOUT_FILE_FORMAT_VERSION,
    LIBOSMSCOUT_COMMIT_HASH
};

DLL_EXPORT const router_version_t* router_get_version()
{
    return &router_version;
};
