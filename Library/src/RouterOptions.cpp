
#include "Router.h"
#include "RouterOptions.h"

DLL_EXPORT router_result router_opt_new(void** opt_ptr)
{
    auto opt = new RouterOptions;
    *opt_ptr = opt;

    return ROUTER_RESULT_OK;
};

DLL_EXPORT void router_opt_del(void* opt_ptr)
{
    if (!opt_ptr) {
        return;
    }

    auto opt = static_cast<RouterOptions*>(opt_ptr);
    delete opt;
}

DLL_EXPORT router_result
router_opt_set_dbpath(void* opt_ptr, const char* db_path[], uint32_t db_count)
{
    if (!opt_ptr || (db_count == 0)) {
        return ROUTER_RESULT_ERROR;
    }

    auto opt = static_cast<RouterOptions*>(opt_ptr);

    for (uint32_t i = 0; i < db_count; i++) {
        opt->databasePath.push_back(std::string(db_path[i]));
    }

    return ROUTER_RESULT_OK;
}

DLL_EXPORT router_result
router_opt_set_rnode_radius(void* opt_ptr, uint32_t radius)
{
    if (!opt_ptr || (radius == 0)) {
        return ROUTER_RESULT_ERROR;
    }

    auto opt = static_cast<RouterOptions*>(opt_ptr);

    opt->closestRoutableNodeRadius = radius;

    return ROUTER_RESULT_OK;
}
