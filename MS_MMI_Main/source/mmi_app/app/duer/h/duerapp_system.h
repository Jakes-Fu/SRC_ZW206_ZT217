
#ifndef BAIDU_DUERAPP_SYSTEM_H
#define BAIDU_DUERAPP_SYSTEM_H

typedef struct {
    float longitude;
    float latitude;
    char coordinate_sys[16];
} gps_info_t;

int duer_get_gps_info(gps_info_t *info);

int duer_set_gps_info(float longitude, float latitude, char *coord);

#endif
