#ifndef OSMSCOUT_ROUTE_H_
#define OSMSCOUT_ROUTE_H_

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

enum route_profile {
    ROUTE_PROFILE_CAR,
    ROUTE_PROFILE_BIKE,
    ROUTE_PROFILE_FOOT
};

typedef struct {
    double lon;
    double lat;
} point_t;

DLL_EXPORT bool router_new(void** ctx, const char* db_path);
DLL_EXPORT bool router_del(void* ctx);

DLL_EXPORT bool router_calc(void* ctx, route_profile profile,
                            const point_t* p1, const point_t* p2,
                            uint32_t* out_count, const point_t** out_points);

DLL_EXPORT bool router_clear(void* ctx);

DLL_EXPORT const char* router_get_error_message(void* ctx);

#ifdef __cplusplus
}
#endif

#endif // OSMSCOUT_ROUTE_H_
