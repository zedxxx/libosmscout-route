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

enum route_profile {
    ROUTE_PROFILE_CAR,
    ROUTE_PROFILE_BIKE,
    ROUTE_PROFILE_FOOT,
    ROUTE_PROFILE_UNDEF
};

enum route_calc_result {
    CALC_RESULT_OK,
    CALC_RESULT_NODATA,
    CALC_RESULT_ERROR
};

typedef struct {
    double lon;
    double lat;
} point_t;

DLL_EXPORT void router_init();

DLL_EXPORT bool router_new(void** ctx, const char* db_path);
DLL_EXPORT bool router_new_multi(void** ctx_ptr, const char* db_path[], uint32_t db_count);
DLL_EXPORT void router_del(void* ctx);

DLL_EXPORT route_calc_result router_calc(void* ctx, route_profile profile,
                                         const point_t* p1, const point_t* p2,
                                         uint32_t* out_count, const point_t** out_points);

DLL_EXPORT void router_clear(void* ctx);

DLL_EXPORT const char* router_get_error_message(void* ctx);

#ifdef __cplusplus
}
#endif

#endif // header guard

