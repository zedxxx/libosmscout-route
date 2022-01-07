#ifndef HEADER_B0FCA66E3955A488
#define HEADER_B0FCA66E3955A488

#include <stdint.h>
#include <stdbool.h>

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    ROUTE_PROFILE_CAR,
    ROUTE_PROFILE_BIKE,
    ROUTE_PROFILE_FOOT,
    ROUTE_PROFILE_UNDEF
} route_profile;

typedef enum {
    ROUTER_RESULT_OK            = 0,
    ROUTER_RESULT_ERROR         = 1,
    ROUTER_RESULT_NODATA_START  = 100,
    ROUTER_RESULT_NODATA_TARGET = 101,
    ROUTER_RESULT_NODATA_ROUTE  = 102
} router_result;

typedef struct {
    double lon;
    double lat;
} point_t;

DLL_EXPORT void router_init();

// options
DLL_EXPORT router_result router_opt_new(void** opt);
DLL_EXPORT void router_opt_del(void* opt);

DLL_EXPORT router_result
router_opt_set_dbpath(void* opt, const char* db_path[], uint32_t db_count);

DLL_EXPORT router_result
router_opt_set_rnode_radius(void* opt, uint32_t radius);

// routing
DLL_EXPORT router_result router_new(void** ctx, const void* opt);
DLL_EXPORT void router_del(void* ctx);

DLL_EXPORT router_result router_calc(void* ctx, route_profile profile,
                                     const point_t* p1, const point_t* p2,
                                     uint32_t* out_count, const point_t** out_points);

DLL_EXPORT void router_clear(void* ctx);

DLL_EXPORT const char* router_get_error_message(void* ctx);

typedef struct {
    uint32_t    libosmscout_db_file_version;
    const char* libosmscout_commit_hash;
} router_version_t;

DLL_EXPORT const router_version_t* router_get_version();

#ifdef __cplusplus
}
#endif

#endif // header guard

