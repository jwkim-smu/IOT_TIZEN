/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#include <Elementary.h>
#include <app.h>
#include <dlog.h>
#include <sensor.h>
#include <storage.h>
#include <system_info.h>

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <Ecore.h>
#include <unistd.h>
#include <efl_extension.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <net_connection.h>
#include <netinet/in.h>
#include <curl/curl.h>
#include <stdalign.h>
#include <tizen.h>

#define BUFLEN 200
#define MAX_COUNT 24
#define _PRINT_MSG_LOG_BUFFER_SIZE_ 1024
#define PRINT_MSG(fmt, args...) do { char _log_[_PRINT_MSG_LOG_BUFFER_SIZE_]; \
    snprintf(_log_, _PRINT_MSG_LOG_BUFFER_SIZE_, fmt, ##args); _add_entry_text(_log_); } while (0)

typedef struct {
    Evas_Object *win;
    Evas_Object *navi;
} appdata_s;

struct tm1 {
	int tm_sec;         /* 초,  range 0 to 59            */
	int tm_min;         /* 분, range 0 to 59             */
	int tm_hour;        /* 시간, range 0 to 23            */
	int tm_mday;        /* 일, range 1 to 31             */
	int tm_mon;         /* 월, range 0 to 11             */
	int tm_year;        /* 1900년 부터의 년                */
	int tm_wday;        /* 요일, range 일(0) to 토(6)      */
	int tm_yday;        /* 1년 중 경과 일, range 0 to 365  */
	int tm_isdst;       /* 써머타임                        */
};

time_t cur_time;					//CURRENT TIME

char ID[100];						//TIZEN_ID

Evas_Object *GLOBAL_DEBUG_BOX;
Evas_Object *start, *stop, *next;
Evas_Object *conform;
Evas_Object *text;
Evas_Object *main_layout;
Evas_Object *event_label;
Evas_Object *event_label1;
Evas_Object *bt1, *bt;
Evas_Object *check1;
Evas_Object *check2;

char* tizenID(void);

static void app_get_data(const char *res_file_in, char *res_path_out, int res_path_max);
static void write_file(char* filepath, char* buf);

double sgn(double x);
double* randomN();
double laplace(double mu, double b, double ran);
double privatize(double f, double sensitivity, double epsil, double ran);
//float ran(int x);

static void SM(char* msg, int port_num);
//static void SM1(double* msg, int port_num);

void hrm_average(int hour, double hrm);
void on_sensor_event(sensor_h sensor, sensor_event_s *event, void *user_data);
void on_sensor_event1(sensor_h sensor, sensor_event_s *event, void *user_data);
void _add_entry_text(const char *text);

Evas_Object *_new_button(appdata_s *ad, Evas_Object *display, char *name, void *cb);
void create_sub_layout(void *data, Evas_Object *obj, void *event_info);
static void app_get_resource(const char *edj_file_in, char *edj_path_out, int edj_path_max);
void _animation_test_display(void *data, Evas_Object *obj, void *event_info);
void create_second_layout(void *data, Evas_Object *obj, void *event_info);
void _create_new_cd_display(appdata_s *ad, char *name, void *cb);
static void create_base_gui(appdata_s *ad);
static bool app_create(void *data);

int main(int argc, char *argv[]);


#ifndef PACKAGE
#define PACKAGE "org.example.sensor"
#endif

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "sensor"


#endif                           /* __MAIN_H__ */
