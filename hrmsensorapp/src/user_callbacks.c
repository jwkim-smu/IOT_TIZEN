/*
 * user_callbacks.c
 *
 *  Created on: Aug 14, 2018
 *      Author: dlsrk
 */


#include "main_app.h"
#include "user_callbacks.h"

int target = 0;
int check_state1 = 0;
int check_state2 = 0;
float f,p=0.25,q=0.75;

//noise
void minusCb(void *data, Evas_Object *obj, void *event_info) {
	if (target > 0) {
		target--;
		char a[100];
		sprintf(a, "<align=center>%d</align>", target);
		elm_object_text_set(text, a);
	}
}
void plusCb(void *data, Evas_Object *obj, void *event_info) {
	if (target < 3) {
		target++;
		char a[100];
		sprintf(a, "<align=center>%d</align>", target);
		elm_object_text_set(text, a);
	}
}

void _scroll_start_cb(void *data, Evas_Object *obj, void *event_info) {
	printf("Scroll starts\n");
}

void check_changed_cb(void *data, Evas_Object *obj, void *event_info) {

   Eina_Bool state = elm_check_state_get(obj);

   if((obj==check1) && (state == EINA_TRUE)){
	   check_state1 = 1;
   }
   else if((obj==check2) && (state == EINA_TRUE)){
	   check_state2 = 1;
   }
   else if((obj==check1) && (state == EINA_FALSE)){
	   check_state1 = 0;
   }
   else if((obj==check2) && (state == EINA_FALSE)){
	   check_state2 = 0;
   }
   else{
	   return;
   }
}

void _sensor_accuracy_changed_cb(sensor_h sensor, unsigned long long timestamp,
		sensor_data_accuracy_e accuracy, void *data) {
	dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor accuracy change callback invoked");
}

void _sensor_debug_start(sensor_h sensor, sensor_type_e type, sensor_listener_h listener, sensor_event_cb callback){
	void *user_data = NULL;

	char out[100];
	bool supported;

	int error = sensor_is_supported(type, &supported);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_is_supported error: %d", error);
		return;
	}
	if (supported) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "HRM is%s supported",
				supported ? "" : " not");
		sprintf(out, "SENSOR is %s supported", supported ? "" : " not");
		//elm_object_text_set(event_label, out);
	}
	error = sensor_get_default_sensor(type, &sensor);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_default_sensor error: %d",
				error);
		return;
	}
	dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_get_default_sensor");
	// Registering a Sensor Event
	error = sensor_create_listener(sensor, &listener);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_create_listener error: %d",
				error);
		return;
	}

	dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_create_listener");
	int min_interval = 0;
	error = sensor_get_min_interval(sensor, &min_interval);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_min_interval error: %d",
				error);
		return;
	}

	dlog_print(DLOG_DEBUG, LOG_TAG, "Minimum interval of the sensor: %d",
			min_interval);
	// Callback for sensor value change
	error = sensor_listener_set_event_cb(listener, min_interval,
			callback, user_data);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG,
				"sensor_listener_set_event_cb error: %d", error);
		return;
	}

	dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_event_cb");
	// Registering the Accuracy Changed Callback
	error = sensor_listener_set_accuracy_cb(listener,
			_sensor_accuracy_changed_cb, user_data);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG,
				"sensor_listener_set_accuracy_cb error: %d", error);
		return;
	}

	dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_accuracy_cb");
	////////////////////////////////////////////////1초마다 측정 man~
	error = sensor_listener_set_interval(listener, 1000);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG,
				"sensor_listener_set_interval error: %d", error);
		return;
	}

	dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_intervals");
	error = sensor_listener_start(listener);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_listener_start error: %d",
				error);
		return;
	}

	dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_start");

	sensor_event_s event;

	error = sensor_listener_read_data(listener, &event);
	if (error != SENSOR_ERROR_NONE) {

		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_listener_read_data error: %d",
				error);
		return;
	}

	char *name = NULL;
	char *vendor = NULL;
	float min_range = 0.0;
	float max_range = 0.0;
	int resolution = 0;

	switch (type) {

		case SENSOR_HRM:

			max_range=220.0;

			dlog_print(DLOG_INFO, LOG_TAG,
					"<align=center><font_size=20>%.2f</font_size></align>",
					event.values[0]);		//log
			sprintf(out, "<align=center><font_size=20>%.2f</font_size></align>",
					event.values[0]);
			elm_object_text_set(event_label, out);

			break;

		case SENSOR_HUMAN_PEDOMETER:

			max_range = 10000.0;

			dlog_print(DLOG_INFO, LOG_TAG, "<align=center>%.2f</align>",
					event.values[0]);		//log
			sprintf(out, "<align=center><font_size=20>%.2f</font_size></align>",
					event.values[0]);
			elm_object_text_set(event_label1, out);

			break;

		default:
			dlog_print(DLOG_ERROR, LOG_TAG, "Not an SENSOR event");
	}

	dlog_print(DLOG_DEBUG, LOG_TAG, out);

	error = sensor_get_type(sensor, &type);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_type error: %d", error);
		return;
	}

	error = sensor_get_name(sensor, &name);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_name error: %d", error);
		return;
	}

	dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor name: %s", name);
	free(name);
	error = sensor_get_vendor(sensor, &vendor);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_vendor error: %d", error);
		return;
	}
	dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor vendor: %s", vendor);
	free(vendor);

	dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor type: %s",
			type == SENSOR_ACCELEROMETER ? "Pedometer" :
			type == SENSOR_HRM ?
					"Heart Rate Monitor sensor (Since Tizen 2.3.1)" :
			type == SENSOR_LAST ?
					"End of sensor enum values" : "Custom sensor");

	error = sensor_get_min_range(sensor, &min_range);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_min_range error: %d",
				error);
		return;
	}
	dlog_print(DLOG_DEBUG, LOG_TAG, "Minimum range of the sensor: %f",
			min_range);

	error = sensor_get_max_range(sensor, &max_range);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_max_range error: %d",
				error);
		return;
	}
	dlog_print(DLOG_DEBUG, LOG_TAG, "Maximum range of the sensor: %f",
			max_range);
	error = sensor_get_resolution(sensor, &resolution);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_resolution error: %d",
				error);
		return;
	}
	dlog_print(DLOG_DEBUG, LOG_TAG, "Resolution of the sensor: %f", resolution);
}

void _sensor_start_cb(void *data, Evas_Object *obj, void *event_info) {
	strcpy(ID, tizenID());

	sensor_h sensor;
	sensor_type_e type = SENSOR_HRM;

	sensor_h sensor1;
	sensor_type_e type1 = SENSOR_HUMAN_PEDOMETER;

	// Retrieving a Sensor
	if (check_state1 == 1 && check_state2 == 0) {
		_sensor_debug_start(sensor, type, listener, on_sensor_event);
	}
	if (check_state1 == 0 && check_state2 == 1) {
		_sensor_debug_start(sensor1, type1, listener1, on_sensor_event1);
	}
	if ( (check_state1 == 1) && (check_state2 == 1) ) {
		_sensor_debug_start(sensor, type, listener, on_sensor_event);
		_sensor_debug_start(sensor1, type1, listener1, on_sensor_event1);
	}

	elm_object_disabled_set(start, EINA_TRUE);
	elm_object_disabled_set(stop, EINA_FALSE);
}

void _sensor_stop_cb(void *data, Evas_Object *obj, void *event_info) {
	int error = sensor_listener_unset_event_cb(listener);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG,
				"sensor_listener_unset_event_cb error: %d", error);
	}

	int error1 = sensor_listener_unset_event_cb(listener1);
	if (error1 != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG,
				"sensor_listener_unset_event_cb error: %d", error1);
	}

	error = sensor_listener_stop(listener);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_listener_stop error: %d",
				error);
	}
	error1 = sensor_listener_stop(listener1);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_listener_stop error: %d",
				error1);
	}

	error = sensor_destroy_listener(listener);
	if (error != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_destroy_listener error: %d",
				error);
	}
	error1 = sensor_destroy_listener(listener1);
	if (error1 != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sensor_destroy_listener error: %d",
				error1);
	}

	elm_object_disabled_set(start, EINA_FALSE);
	elm_object_disabled_set(stop, EINA_TRUE);
}

void win_delete_request_cb(void *data, Evas_Object *obj,
		void *event_info) {
	elm_exit();
}

Eina_Bool _pop_cb(void *data, Elm_Object_Item *item) {
	elm_win_lower(((appdata_s *) data)->win);
	return EINA_FALSE;
}

