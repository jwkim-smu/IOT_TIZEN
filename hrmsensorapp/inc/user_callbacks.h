/*
 * user_callbacks.h
 *
 *  Created on: Aug 14, 2018
 *      Author: dlsrk
 */

#ifndef USER_CALLBACKS_H_
#define USER_CALLBACKS_H_

#include "main_app.h"

void minusCb(void *data, Evas_Object *obj, void *event_info);
void plusCb(void *data, Evas_Object *obj, void *event_info);
void _scroll_start_cb(void *data, Evas_Object *obj, void *event_info);
void check_changed_cb(void *data, Evas_Object *obj, void *event_info);
void _sensor_accuracy_changed_cb(sensor_h sensor, unsigned long long timestamp,	sensor_data_accuracy_e accuracy, void *data);
//void _sensor_debug_start(void *data, sensor_h sensor, sensor_type_e type, sensor_listener_h listener, sensor_event_cb callback);
void _sensor_start_cb(void *data, Evas_Object *obj, void *event_info);
void _sensor_stop_cb(void *data, Evas_Object *obj, void *event_info);
void win_delete_request_cb(void *data, Evas_Object *obj, void *event_info);
Eina_Bool _pop_cb(void *data, Elm_Object_Item *item);

#endif /* USER_CALLBACKS_H_ */
