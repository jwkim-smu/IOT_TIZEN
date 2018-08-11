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

#include "main_app.h"
#define BUFLEN 200

Evas_Object *GLOBAL_DEBUG_BOX;
Evas_Object *start, *stop, *next;
Evas_Object *conform;
Evas_Object *text;
Evas_Object *main_layout;
sensor_listener_h listener;
sensor_listener_h listener1;
Evas_Object *event_label;
Evas_Object *event_label1;
Evas_Object *bt1, *bt;
Evas_Object *check1;
Evas_Object *check2;
Eina_Bool state1, state2;

int target = 0;
int check_state1 = 0;
int check_state2 = 0;
float f,p=0.25,q=0.75;

char ID[100];

time_t cur_time;

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


char* tizenID(void) {
	char *value;
	int ret;
	ret = system_info_get_platform_string("http://tizen.org/system/tizenid", &value);

	if (ret != SYSTEM_INFO_ERROR_NONE) {
		/* Error handling */

		return "ERROR";
	}

	dlog_print(DLOG_INFO, LOG_TAG, "Model name: %s", value);

	return value;

	free(value); /* Release after use */
}

static void
app_get_data(const char *res_file_in, char *res_path_out, int res_path_max)
{
    char *res_path = NULL;
    res_path= "/opt/usr/media/Documents/";
    if (res_path)
    {
        snprintf(res_path_out, res_path_max, "%s%s", res_path, res_file_in);
    }
}

static void
write_file(char* filepath, char* buf)
{

    FILE *fp;
    fp = fopen(filepath, "a+");

    if(fp==NULL){
    	//error
    	dlog_print(DLOG_INFO, "error", "%s", "error");
    }
    else{
		fputs(buf, fp);
    }
    fclose(fp);
}

float ran(int x){
	float a = (float)((rand() % 100)/100.0);
	dlog_print(DLOG_INFO, "ran", "%d %f", x, a);
	return a;
}
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
//socket
static void SM1(char* msg) {
	//appdata_s *ad = data;
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	////////////////////////////////////////////////////////////////////////////////////
	//address.sin_addr.s_addr = inet_addr("192.168.0.197");	//怨듭쑀湲?ip??而댄벂??怨좎쑀ip ?섎떎 ?뺤씤
	//address.sin_addr.s_addr = inet_addr("117.16.123.192");	//?쒕쾭ip
	address.sin_addr.s_addr = inet_addr("117.16.123.12");	//?쒕쾭ip	//?섎? 怨듭쑀湲?ip ?ы듃?ъ썙?⑺빐???ｊ린
	//address.sin_addr.s_addr = inet_addr("117.16.123.55");	//怨듭쑀湲?ip
	//address.sin_addr.s_addr = inet_addr("172.16.168.19");	//notebookip
	//address.sin_addr.ss_addr = inet_addr("203.237.172.100");	//?쒕쾭ip
	////////////////////////////////////////////////////////////////////////////////////
	//address.sin_port = htons(4427);	/////////////////dsock = new DatagramSocket(4422);
	address.sin_port = htons(4427);	/////////////////?dsock = new DatagramSocket(4422);
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		dlog_print(DLOG_INFO, "USR_TAG", "socket create fail");
		return;
	}
	connect(sockfd, (struct sockaddr *) &address, sizeof(struct sockaddr_in));
	memset(&(address.sin_zero), 0, 8);
	//sendto(sockfd,msg,strlen(msg)+1,0,(struct sockaddr *)&address,sizeof(struct sockaddr_in));
// if(sendto(sockfd,msg,strlen(msg)+1,0,(struct sockaddr *)&address,sizeof(struct sockaddr_in))<=0){
//   dlog_print(DLOG_INFO,"USR_TAG","write 1 error");
//  }
	if (write(sockfd, msg, strlen(msg) + 1) <= 0) {
		dlog_print(DLOG_INFO, "USR_TAG", "write 1 error");
	}

	close(sockfd);
}

static void SM2(char* msg) {
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("117.16.123.12");	//?쒕쾭ip	//?섎? 怨듭쑀湲?ip ?ы듃?ъ썙?⑺빐???ｊ린
	address.sin_port = htons(4428);	/////////////////?먮컮???ы듃踰덊샇 留욎텛湲?dsock = new DatagramSocket(4422);
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		dlog_print(DLOG_INFO, "USR_TAG", "socket create fail");
		return;
	}
	connect(sockfd, (struct sockaddr *) &address, sizeof(struct sockaddr_in));
	memset(&(address.sin_zero), 0, 8);
	if (write(sockfd, msg, strlen(msg) + 1) <= 0) {
		dlog_print(DLOG_INFO, "USR_TAG", "write 1 error");
	}

	close(sockfd);
}


static void SM3(char* msg) {
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("117.16.123.12");	//?쒕쾭ip	//?섎? 怨듭쑀湲?ip ?ы듃?ъ썙?⑺빐???ｊ린
	address.sin_port = htons(4429);	/////////////////?먮컮???ы듃踰덊샇 留욎텛湲?dsock = new DatagramSocket(4422);
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd == -1) {
		dlog_print(DLOG_INFO, "USR_TAG", "socket create fail");
		return;
	}

	connect(sockfd, (struct sockaddr *) &address, sizeof(struct sockaddr_in));
	memset(&(address.sin_zero), 0, 8);

	if (write(sockfd, msg, strlen(msg) + 1) <= 0) {
		dlog_print(DLOG_INFO, "USR_TAG", "write 1 error");
	}

	close(sockfd);
}

//void write_sensor(char *tmp1, char *tmp2){
//	//char *d1 = timelog(date);
//	time(&cur_time);
//
//	char filepath[PATH_MAX] = { 0, };
//	app_get_data("data.txt", filepath, PATH_MAX);
//	strcat(tmp1, tmp2);
//	strcat(ctime(&cur_time), tmp1);
//	write_file(filepath, ctime(&cur_time));
//}

void on_sensor_event(sensor_h sensor, sensor_event_s *event, void *user_data) {
	time_t t = time(NULL);
	struct tm tm1 = *localtime(&t);

	// int today = tm1.tm_mday;
	int today = tm1.tm_yday;
	int nextday;


	if(today == 365){
		nextday = 0;
	}else{
		nextday = today + 1;
	}

	//해당 함수 호출시 초 check

	// Select a specific sensor with a sensor handle
	sensor_type_e type;
	sensor_get_type(sensor, &type);


	time(&cur_time);
	int start = tm1.tm_sec;

	char filepath[PATH_MAX] = { 0, };

	char fname[100] = "data";
	char day[10] = "";
	char fname1[100] = ".txt";

	if (tm1.tm_yday == nextday) {
		sprintf(day, "%d", nextday);
		strcat(fname, day);
		strcat(fname, fname1);
	} else {
		sprintf(day, "%d", today);
		strcat(fname, day);
		strcat(fname, fname1);
	}

	app_get_data(fname, filepath, PATH_MAX);

	if(start==30 || start==0){
		SM1(ID);

		char tmpLine2[100] = "";
		char tmpLine3[100] = "";
		char tmp3[100]= "";
		if (type == SENSOR_HRM) {
			/////////////////heartrate///////////////////
			dlog_print(DLOG_INFO, LOG_TAG,
					"<align=center><font_size=20>%d</font_size></align>",
					event->values[0]);

			sprintf(tmpLine2,
					"<align=center><font_size=20>%d - %.2f</font_size></align>",
					start, event->values[0]);
			sprintf(tmpLine3, "%.2f", event->values[0]);
			elm_object_text_set(event_label, tmpLine2);
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s", tmpLine3);
			strcpy(tmp3, tmpLine3);		//write_file

			strcat(tmpLine3, "|");		//SM2

			SM2(tmpLine3);

			if (check_state1 == 1) {
				write_file(filepath, ctime(&cur_time));
				write_file(filepath, " ");
				write_file(filepath, tmp3);
				write_file(filepath, " ");

			}
		}
//		char a[100], b[80] = "";
//
//		for (int i = 31; i < 101; i++) {
//			if ((int) event->values[0] == i) {
//				strcat(b, "1");
//				for (int j = i + 1; j < 101; j++) {
//					strcat(b, "0");
//				}
//				break;
//			}
//			strcat(b, "0");
//		}
//		sprintf(a, "<align=center>%f</align>", event->values[0]);
//
//		elm_object_text_set(event_label, a);
//		srand(time(NULL));
//		if (event->values[0] > 30) {
//			char tmpLine[80] = "";
//			for (int i = 0; i < 70; i++) {
//				bool check = false;
//				if (ran(1) < f)
//					check = true;
//				if (check) {
//					if (ran(2) < 0.5)
//						strcat(tmpLine, "1");
//					else
//						strcat(tmpLine, "0");
//				} else {
//					tmpLine[i] = b[i];
//				}
//			}
//			char tmpLine2[80];
//			for (int i = 0; i < 70; i++) {
//				if (tmpLine[i] == '0') {
//					if (ran(3) < p)
//						//tmpLine2[i] = '1';
//						strcat(tmpLine2, "1");
//					else
//						//tmpLine2[i] = '0';
//						strcat(tmpLine2, "0");
//				} else {
//					if (ran(4) < q)
//						strcat(tmpLine2, "1");
//					//tmpLine2[i] = '1';
//					else
//						strcat(tmpLine2, "0");
//					//tmpLine2[i] = '0';
//				}
//			}
//			dlog_print(DLOG_INFO, LOG_TAG, "%s", tmpLine2);
//			SM2(tmpLine2);
//			strcpy(tmpLine,"");
//			strcpy(tmpLine2,"");
//		}
//	if (type == SENSOR_HUMAN_PEDOMETER) {//mobile only sensor, when system rebooted, steps will be detected from
//		///////////////////Step//////////////////////
//		dlog_print(DLOG_INFO, LOG_TAG,
//				"<align=center><font_size=20>%.2f</font_size></align>",
//				event->values[0]);
//
//		sprintf(tmpLine4,
//				"<align=center><font_size=20>%.2f</font_size></align>",
//				event->values[0]);
//		sprintf(tmpLine5, "%.2f", event->values[0]);
//		elm_object_text_set(event_label1, tmpLine4);
//		dlog_print(DLOG_INFO, LOG_TAG, "%s", tmpLine5);
//
//		SM3(tmpLine5);
//
//	}
//	write_sensor(tmpLine3, tmpLine5);
//	write_file(filepath, "\t");
	}
}

void on_sensor_event1(sensor_h sensor, sensor_event_s *event, void *user_data){

	time_t t = time(NULL);
	struct tm tm1 = *localtime(&t);

	sensor_type_e type;
	sensor_get_type(sensor, &type);

	int today = tm1.tm_yday;
	int nextday;

	if (today == 365) {
		nextday = 0;
	} else {
		nextday = today + 1;
	}

	time(&cur_time);
	//해당 함수 호출시 초 check
	int start = tm1.tm_sec;

	char filepath[PATH_MAX] = { 0, };

	char fname[100] = "data";
	char day[10] = "";
	char fname1[100] = ".txt";

	if (tm1.tm_yday == nextday) {
		sprintf(day, "%d", nextday);
		strcat(fname, day);
		strcat(fname, fname1);
	}
	else{
		sprintf(day, "%d", today);
		strcat(fname, day);
		strcat(fname, fname1);
	}
	app_get_data(fname, filepath, PATH_MAX);

	if(start==30 || start==0){

		char tmpLine4[100] = "";
		char tmpLine5[100] = "";

		if (type == SENSOR_HUMAN_PEDOMETER) {
			//mobile only sensor, when system rebooted, steps will be detected from
			///////////////////Step//////////////////////
			dlog_print(DLOG_INFO, LOG_TAG,
					"<align=center><font_size=20>%.2f</font_size></align>",
					event->values[0]);

			sprintf(tmpLine4,
					"<align=center><font_size=20>%d - %.2f</font_size></align>",
					start, event->values[0]);

			sprintf(tmpLine5, "%.2f", event->values[0]);
			elm_object_text_set(event_label1, tmpLine4);
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s", tmpLine5);


			SM3(tmpLine5);

			if (check_state1 == 0 && check_state2 == 1) {
				write_file(filepath, ctime(&cur_time));
				write_file(filepath, " ");
				write_file(filepath, tmpLine5);
				write_file(filepath, "\t");
			} else if (check_state1 == 1 && check_state2 == 1) {
				write_file(filepath, " ");
				write_file(filepath, tmpLine5);
				write_file(filepath, "\t");
			}
		}
	}
	//nextday = tm1.tm_mday;
}



void _sensor_accuracy_changed_cb(sensor_h sensor, unsigned long long timestamp,
		sensor_data_accuracy_e accuracy, void *data) {
	dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor accuracy change callback invoked");
}

void _sensor_start_cb(void *data, Evas_Object *obj, void *event_info) {
	strcpy(ID, tizenID());

	void *user_data = NULL;
	char out[100];
	char out1[100];

	sensor_type_e type = SENSOR_HRM;
	sensor_h sensor;
	bool supported;

	sensor_type_e type1 = SENSOR_HUMAN_PEDOMETER;
	sensor_h sensor1;
	bool supported1;

	// Retrieving a Sensor1
	///////////////////////////////////
	if (check_state1 == 1 && check_state2 == 0) {
		int error = sensor_is_supported(type, &supported);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_is_supported error: %d",
					error);
			return;
		}
		if (supported) {
			dlog_print(DLOG_DEBUG, LOG_TAG, "HRM is%s supported",
					supported ? "" : " not");
			sprintf(out, "HRM is%s supported", supported ? "" : " not");
			elm_object_text_set(event_label, out);
		}
		error = sensor_get_default_sensor(type, &sensor);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_get_default_sensor error: %d", error);
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
				on_sensor_event, user_data);
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
		/////////////////////////////////////////////////////////////////////////////////////////
		sensor_event_s event;
		error = sensor_listener_read_data(listener, &event);
		if (error != SENSOR_ERROR_NONE) {

			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_read_data error: %d", error);
			return;
		}
		switch (type) {
			///////////////////////////////////////////////////////////////////////////////////////////
			case SENSOR_HRM:
				dlog_print(DLOG_INFO, LOG_TAG,
						"<align=center><font_size=20>%.2f</font_size></align>",
						event.values[0]);		//log
				sprintf(out, "<align=center><font_size=20>%.2f</font_size></align>",
						event.values[0]);
				elm_object_text_set(event_label, out);
				break;
			default:
				dlog_print(DLOG_ERROR, LOG_TAG, "Not an HRM event");
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, out);
		char *name = NULL;
		char *vendor = NULL;
		float min_range = 0.0;
		float max_range = 220.0;
		int resolution = 0;
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
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_vendor error: %d",
					error);
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
		dlog_print(DLOG_DEBUG, LOG_TAG, "Resolution of the sensor: %f",
				resolution);
	}
	///////////////////////////////////
	if (check_state1 == 0 && check_state2 == 1) {
		int error1 = sensor_is_supported(type1, &supported1);

		if (error1 != SENSOR_ERROR_NONE) {

			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_is_supported error: %d",
					error1);
			return;
		}

		if (supported1) {
			dlog_print(DLOG_DEBUG, LOG_TAG, "Pedometer is%s supported",
					supported1 ? "" : " not");
			sprintf(out1, "Pedometer is%s supported", supported1 ? "" : " not");
			elm_object_text_set(event_label1, out1);
		}

		error1 = sensor_get_default_sensor(type1, &sensor1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_get_default_sensor error: %d", error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_get_default_sensor");

		error1 = sensor_create_listener(sensor1, &listener1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_create_listener error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_create_listener");

		int min_interval1 = 1000;
		error1 = sensor_get_min_interval(sensor1, &min_interval1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_min_interval error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Minimum interval of the sensor: %d",
				min_interval1);

		error1 = sensor_listener_set_event_cb(listener1, min_interval1,
				on_sensor_event1, user_data);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_set_event_cb error: %d", error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_event_cb");

		// Registering the Accuracy Changed Callback
		error1 = sensor_listener_set_accuracy_cb(listener1,
				_sensor_accuracy_changed_cb, user_data);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_set_accuracy_cb error: %d", error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_accuracy_cb");

		error1 = sensor_listener_set_interval(listener1, 1000);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_set_interval error: %d", error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_intervals");

		error1 = sensor_listener_set_option(listener1, SENSOR_OPTION_ALWAYS_ON);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_set_option error: %d", error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_option");

		error1 = sensor_listener_start(listener1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_listener_start error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_start");

		sensor_event_s event1;
		error1 = sensor_listener_read_data(listener1, &event1);
		if (error1 != SENSOR_ERROR_NONE) {

			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_read_data error: %d", error1);
			return;
		}

		switch (type1) {
			///////////////////////////////////////////////////////////////////////////////////////////
			case SENSOR_HUMAN_PEDOMETER:
				dlog_print(DLOG_INFO, LOG_TAG, "<align=center>%.2f</align>",
						event1.values[0]);		//log
				sprintf(out1,
						"<align=center><font_size=20>%.2f</font_size></align>",
						event1.values[0]);
				elm_object_text_set(event_label1, out1);

				break;
			default:
				dlog_print(DLOG_ERROR, LOG_TAG, "Not an HRM event");
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, out1);

		char *name1 = NULL;
		char *vendor1 = NULL;
		float min_range1 = 0.0;
		float max_range1 = 10000.0;
		int resolution1 = 0;

		error1 = sensor_get_name(sensor1, &name1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_name error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor name: %s", name1);
		free(name1);

		error1 = sensor_get_vendor(sensor1, &vendor1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_vendor error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor vendor: %s", vendor1);
		free(vendor1);

		error1 = sensor_get_type(sensor1, &type1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_type error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor type: %s",
				type == SENSOR_ACCELEROMETER ? "Pedometer" :
				type == SENSOR_HRM ?
						"Heart Rate Monitor sensor (Since Tizen 2.3.1)" :
				type == SENSOR_LAST ?
						"End of sensor enum values" : "Custom sensor");

		error1 = sensor_get_min_range(sensor1, &min_range1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_min_range error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Minimum range of the sensor: %f",
				min_range1);

		error1 = sensor_get_max_range(sensor1, &max_range1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_max_range error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Maximum range of the sensor: %f",
				max_range1);

		error1 = sensor_get_resolution(sensor1, &resolution1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_resolution error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Resolution of the sensor: %f",
				resolution1);
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////
	if ( (check_state1 == 1) && (check_state2 == 1) ) {
		int error = sensor_is_supported(type, &supported);
		int error1 = sensor_is_supported(type1, &supported1);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_is_supported error: %d",
					error);
			return;
		}
		if (error1 != SENSOR_ERROR_NONE) {

			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_is_supported error: %d",
					error1);
			return;
		}

		if (supported) {
			dlog_print(DLOG_DEBUG, LOG_TAG, "HRM is%s supported",
					supported ? "" : " not");
			sprintf(out, "HRM is%s supported", supported ? "" : " not");
			elm_object_text_set(event_label, out);
		}
		if (supported1) {
			dlog_print(DLOG_DEBUG, LOG_TAG, "Pedometer is%s supported",
					supported1 ? "" : " not");
			sprintf(out1, "Pedometer is%s supported", supported1 ? "" : " not");
			elm_object_text_set(event_label1, out1);
		}

		error = sensor_get_default_sensor(type, &sensor);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_get_default_sensor error: %d", error);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_get_default_sensor");

		error1 = sensor_get_default_sensor(type1, &sensor1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_get_default_sensor error: %d", error1);
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

		error1 = sensor_create_listener(sensor1, &listener1);
		if (error1 != SENSOR_ERROR_NONE) {
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

		int min_interval1 = 1000;
		error1 = sensor_get_min_interval(sensor1, &min_interval1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_min_interval error: %d",
					error);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Minimum interval of the sensor: %d",
				min_interval1);

		// Callback for sensor value change
		error = sensor_listener_set_event_cb(listener, min_interval,
				on_sensor_event, user_data);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_set_event_cb error: %d", error);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_event_cb");

		error1 = sensor_listener_set_event_cb(listener1, 1000,
				on_sensor_event1, NULL);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_set_event_cb error: %d", error1);
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

		// Registering the Accuracy Changed Callback
		error1 = sensor_listener_set_accuracy_cb(listener1,
				_sensor_accuracy_changed_cb, user_data);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_set_accuracy_cb error: %d", error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_accuracy_cb");
		////////////////////////////////////////////////30초마다 측정 man~
		error = sensor_listener_set_interval(listener, 1000);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_set_interval error: %d", error);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_intervals");
		////////////////////////////////////////////////30초마다 측정 man~
		error1 = sensor_listener_set_interval(listener1, 1000);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_set_interval error: %d", error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_intervals");

		error = sensor_listener_set_option(listener, SENSOR_OPTION_ALWAYS_ON);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_set_option error: %d", error);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_option");

		error1 = sensor_listener_set_option(listener1, SENSOR_OPTION_ALWAYS_ON);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_set_option error: %d", error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_option");

		error = sensor_listener_start(listener);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_listener_start error: %d",
					error);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_start");

		error1 = sensor_listener_start(listener1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_listener_start error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_start");
		/////////////////////////////////////////////////////////////////////////////////////////
		sensor_event_s event;
		error = sensor_listener_read_data(listener, &event);
		if (error != SENSOR_ERROR_NONE) {

			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_read_data error: %d", error);
			return;
		}

		sensor_event_s event1;
		error1 = sensor_listener_read_data(listener1, &event1);
		if (error1 != SENSOR_ERROR_NONE) {

			dlog_print(DLOG_ERROR, LOG_TAG,
					"sensor_listener_read_data error: %d", error1);
			return;
		}

		switch (type) {
		///////////////////////////////////////////////////////////////////////////////////////////
		case SENSOR_HRM:
			dlog_print(DLOG_INFO, LOG_TAG,
					"<align=center><font_size=20>%.2f</font_size></align>",
					event.values[0]);		//log
			sprintf(out, "<align=center><font_size=20>%.2f</font_size></align>",
					event.values[0]);
			elm_object_text_set(event_label, out);

			break;
		case SENSOR_HUMAN_PEDOMETER:
			dlog_print(DLOG_INFO, LOG_TAG,
					"<align=center><font_size=20>%.2f</font_size></align>",
					event1.values[0]);		//log
			sprintf(out1,
					"<align=center><font_size=20>%.2f</font_size></align>",
					event1.values[0]);
			elm_object_text_set(event_label1, out1);

			break;
		default:
			dlog_print(DLOG_ERROR, LOG_TAG, "Not an HRM event");
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, out);

		switch (type1) {
		///////////////////////////////////////////////////////////////////////////////////////////
		case SENSOR_HRM:
			dlog_print(DLOG_INFO, LOG_TAG,
					"<align=center><font_size=20>/%f<font_size></align>",
					event.values[0]);		//log
			sprintf(out, "<align=center><font_size=20>%f</font_size></align>",
					event.values[0]);
			elm_object_text_set(event_label, out);
			break;

		case SENSOR_HUMAN_PEDOMETER:
			dlog_print(DLOG_INFO, LOG_TAG, "<align=center>%.2f</align>",
					event1.values[0]);		//log
			sprintf(out1,
					"<align=center><font_size=20>%.2f</font_size></align>",
					event1.values[0]);
			elm_object_text_set(event_label1, out1);

			break;
		default:
			dlog_print(DLOG_ERROR, LOG_TAG, "Not an HRM event");
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, out1);

		char *name = NULL;
		char *vendor = NULL;
		float min_range = 0.0;
		float max_range = 220.0;
		int resolution = 0;

		char *name1 = NULL;
		char *vendor1 = NULL;
		float min_range1 = 0.0;
		float max_range1 = 10000.0;
		int resolution1 = 0;

		error = sensor_get_name(sensor, &name);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_name error: %d", error);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor name: %s", name);
		free(name);

		error1 = sensor_get_name(sensor1, &name1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_name error: %d", error);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor name: %s", name1);
		free(name1);

		error = sensor_get_vendor(sensor, &vendor);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_vendor error: %d",
					error);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor vendor: %s", vendor);
		free(vendor);

		error1 = sensor_get_vendor(sensor1, &vendor1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_vendor error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor vendor: %s", vendor1);
		free(vendor1);

		error = sensor_get_type(sensor, &type);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_type error: %d", error);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Sensor type: %s",
				type == SENSOR_ACCELEROMETER ? "Pedometer" :
				type == SENSOR_HRM ?
						"Heart Rate Monitor sensor (Since Tizen 2.3.1)" :
				type == SENSOR_LAST ?
						"End of sensor enum values" : "Custom sensor");

		error1 = sensor_get_type(sensor1, &type1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_type error: %d",
					error1);
			return;
		}

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

		error1 = sensor_get_min_range(sensor1, &min_range1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_min_range error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Minimum range of the sensor: %f",
				min_range1);

		error = sensor_get_max_range(sensor, &max_range);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_max_range error: %d",
					error);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Maximum range of the sensor: %f",
				max_range);

		error1 = sensor_get_max_range(sensor, &max_range1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_max_range error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Maximum range of the sensor: %f",
				max_range1);

		error = sensor_get_resolution(sensor, &resolution);
		if (error != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_resolution error: %d",
					error);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Resolution of the sensor: %f",
				resolution);

		error1 = sensor_get_resolution(sensor1, &resolution1);
		if (error1 != SENSOR_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "sensor_get_resolution error: %d",
					error1);
			return;
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Resolution of the sensor: %f",
				resolution1);

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
void _add_entry_text(const char *text) {
	Evas_Coord c_y;

	elm_entry_entry_append(GLOBAL_DEBUG_BOX, text);
	elm_entry_entry_append(GLOBAL_DEBUG_BOX, "<br>");
	elm_entry_cursor_end_set(GLOBAL_DEBUG_BOX);
	elm_entry_cursor_geometry_get(GLOBAL_DEBUG_BOX, NULL, &c_y, NULL, NULL);
	elm_scroller_region_show(GLOBAL_DEBUG_BOX, 0, c_y, 0, 0);
}

static void win_delete_request_cb(void *data, Evas_Object *obj,
		void *event_info) {
	elm_exit();
}

Eina_Bool _pop_cb(void *data, Elm_Object_Item *item) {
	elm_win_lower(((appdata_s *) data)->win);
	return EINA_FALSE;
}

Evas_Object *_new_button(appdata_s *ad, Evas_Object *display, char *name,
		void *cb) {
	// Create a button
	bt = elm_button_add(display);
	elm_object_text_set(bt, name);
	evas_object_smart_callback_add(bt, "clicked", (Evas_Smart_Cb) cb, ad);
	evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
	//elm_box_pack_end(display, bt);

	elm_layout_content_set(display, NULL, bt);
	evas_object_show(bt);
	return bt;
}

void create_sub_layout(void *data, Evas_Object *obj, void *event_info) {
	switch (target) {
	case 0:
		f = 0.0;
		break;
	case 1:
		f = 0.05;
		break;
	case 2:
		f = 0.1;
		break;
	case 3:
		f = 0.15;
		break;
	}
	Evas_Object *naviframe = data;
	elm_naviframe_item_pop(naviframe);
	Evas_Object *sub_layout;
	sub_layout = elm_layout_add(naviframe);
	evas_object_size_hint_weight_set(sub_layout, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_show(sub_layout);

	start = elm_button_add(sub_layout);
	elm_object_text_set(start,
			"<align=center><font_size=20>Start</font_size></align>");
	evas_object_smart_callback_add(start, "clicked", _sensor_start_cb, NULL);
	evas_object_move(start, 0, 50);
	evas_object_resize(start, 365, 20);
	evas_object_show(start);

	event_label = elm_label_add(sub_layout);
	elm_object_text_set(event_label,
			"<align=center><font_size=20>Press Start and Wait</font_size></align>");
	//elm_box_pack_end(box, event_label);
	evas_object_move(event_label, 0, 145);
	evas_object_resize(event_label, 365, 30);
	evas_object_show(event_label);

	event_label1 = elm_label_add(sub_layout);
	elm_object_text_set(event_label1,
			"<align=center><font_size=20>Press Start and Wait</font_size></align>");
	//elm_box_pack_end(box, event_label);
	evas_object_move(event_label1, 0, 185);
	evas_object_resize(event_label1, 365, 30);
	evas_object_show(event_label1);


	stop = elm_button_add(sub_layout);
	elm_object_text_set(stop,
			"<align=center><font_size=20>Stop</font_size></align>");
	evas_object_smart_callback_add(stop, "clicked", _sensor_stop_cb, NULL);
	evas_object_move(stop, 0, 280);
	evas_object_resize(stop, 365, 20);
	evas_object_show(stop);

	//evas_object_smart_callback_add(sub_button, "clicked", create_fir_layout, naviframe);
	elm_naviframe_item_push(naviframe, NULL, NULL, NULL, sub_layout, "empty");
}
void create_second_layout(void *data, Evas_Object *obj, void *event_info){

   Evas_Object *naviframe = data;
   elm_naviframe_item_pop(naviframe);
   Evas_Object *main_layout1;
   main_layout1 = elm_layout_add(naviframe);
   evas_object_size_hint_weight_set(main_layout1, EVAS_HINT_EXPAND,
            EVAS_HINT_EXPAND);
   evas_object_show(main_layout1);

   /* Create conformant */
   //Evas *e = evas_object_evas_get(ad->win);
   text = elm_label_add(main_layout1);
   char a[100];
   //sprintf(a,"<align=center><font_size=17>%d</font_size></align>",target);
   sprintf(a, "<align=center>%d</align>", target);
   elm_object_text_set(text, a);
   evas_object_move(text, 140, 150);
   evas_object_resize(text, 70, 30);
   evas_object_color_set(text, 255, 255, 255, 255);
   evas_object_show(text);

   Evas_Object *btnp = elm_button_add(main_layout1);
   elm_object_text_set(btnp,
         "<align=center><font_size=20>+</font_size></align>");
   evas_object_smart_callback_add(btnp, "clicked", plusCb, NULL);
   evas_object_move(btnp, 220, 150);
   evas_object_resize(btnp, 70, 30);
   evas_object_show(btnp);

   Evas_Object *btnm = elm_button_add(main_layout1);
   elm_object_text_set(btnm,
         "<align=center><font_size=20>-</font_size></align>");
   evas_object_smart_callback_add(btnm, "clicked", minusCb, NULL);
   evas_object_move(btnm, 60, 150);
   evas_object_resize(btnm, 70, 30);
   evas_object_show(btnm);

   Evas_Object *btnj = elm_button_add(main_layout1);
   elm_object_text_set(btnj,
         "<align=center><font_size=25>측정</font_size></align>");
   evas_object_smart_callback_add(btnj, "clicked", create_sub_layout,
         naviframe);
   evas_object_move(btnj, 0, 310);
   evas_object_resize(btnj, 365, 30);
   evas_object_show(btnj);

   elm_naviframe_item_push(naviframe, NULL, NULL, NULL, main_layout1, "empty");

}

void _create_new_cd_display(appdata_s *ad, char *name, void *cb) {

	// Create main box
	Evas_Object *box = elm_layout_add(ad->navi);
	elm_object_content_set(ad->navi, box);

	elm_box_horizontal_set(box, EINA_FALSE);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(box);

	start = _new_button(ad, box, "Start", _sensor_start_cb);

	event_label = elm_label_add(box);
	elm_object_text_set(event_label, "Press Start and Wait");
	elm_box_pack_end(box, event_label);
	evas_object_show(event_label);

	event_label1 = elm_label_add(box);
	elm_object_text_set(event_label1, "Press Start and Wait");
	elm_box_pack_end(box, event_label1);
	evas_object_show(event_label1);

	stop = _new_button(ad, box, "Stop", create_sub_layout);
	elm_naviframe_item_push(ad->navi, NULL, NULL, NULL, box, "empty");

}

//110, 200

static void create_base_gui(appdata_s *ad) {
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_conformant_set(ad->win, EINA_TRUE);
	elm_win_autodel_set(ad->win, EINA_TRUE);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_smart_callback_add(ad->win, "delete, request",
			win_delete_request_cb, NULL);
	/* Create conformant */
	conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, conform);
	evas_object_show(conform);
	// Create a naviframe
	ad->navi = elm_naviframe_add(conform);
	evas_object_size_hint_align_set(ad->navi, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(ad->navi, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	elm_win_resize_object_add(conform, ad->navi);
	elm_object_content_set(conform, ad->navi);
	evas_object_show(ad->navi);
	eext_object_event_callback_add(ad->navi, EEXT_CALLBACK_BACK, _pop_cb, ad);
	eext_object_event_callback_add(ad->navi, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);

	main_layout = elm_layout_add(ad->navi);
	evas_object_size_hint_weight_set(main_layout, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_show(main_layout);

	/* check 1 */
	check1 = elm_check_add(main_layout);
	elm_object_style_set(check1, "popup");
	elm_object_text_set(check1, "<align=center>Heart rate</align>");
	evas_object_move(check1, 80, 100);
	evas_object_resize(check1, 200, 100);
	evas_object_smart_callback_add(check1, "changed", check_changed_cb, ad);
	evas_object_show(check1);


	/* check 2 */
	check2 = elm_check_add(main_layout);
	elm_object_style_set(check2, "popup");
	elm_object_text_set(check2, "<align=center>Step count</align>");
	evas_object_move(check2, 80, 150);
	evas_object_resize(check2, 200, 100);
	evas_object_smart_callback_add(check2, "changed", check_changed_cb, ad);
	evas_object_show(check2);


	/* startBtn */
	Evas_Object *startBtn = elm_button_add(main_layout);
	elm_object_text_set(startBtn, "<align=center>START</align>");
	evas_object_move(startBtn, 0, 290);
	evas_object_resize(startBtn, 365, 30);
	evas_object_smart_callback_add(startBtn, "clicked", create_second_layout,
	ad->navi);
	evas_object_show(startBtn);

	elm_naviframe_item_push(ad->navi, NULL, NULL, NULL, main_layout, "empty");
	evas_object_show(ad->win);

}

//static void create_base_gui(appdata_s *ad)
//
//{
//    // Setting the window
//    ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
//    elm_win_conformant_set(ad->win, EINA_TRUE);
//    elm_win_autodel_set(ad->win, EINA_TRUE);
//    elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
//    elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
//    evas_object_smart_callback_add(ad->win, "delete, request", win_delete_request_cb, NULL);
//
//    /* Create conformant */
//    conform = elm_conformant_add(ad->win);
//    evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//    elm_win_resize_object_add(ad->win, conform);
//    evas_object_show(conform);
//
//    // Create a naviframe
//    ad->navi = elm_naviframe_add(conform);
//    evas_object_size_hint_align_set(ad->navi, EVAS_HINT_FILL, EVAS_HINT_FILL);
//    evas_object_size_hint_weight_set(ad->navi, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//    elm_win_resize_object_add(conform, ad->navi);
//    elm_object_content_set(conform, ad->navi);
//    evas_object_show(ad->navi);
//
//    // Fill the list with items
//    //create_buttons_in_main_window(ad);
//    _create_new_cd_display(ad, "Sensor", _pop_cb);
//
//    eext_object_event_callback_add(ad->navi, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
//
//
////    /* main Layout */
////    Evas_Object *main_layout;
////    main_layout = elm_layout_add(ad->navi);
////    evas_object_size_hint_weight_set(main_layout, EVAS_HINT_EXPAND,EVAS_HINT_EXPAND);
////    evas_object_show(main_layout);
////    /* main button */
////    next = _new_button(ad, box, "Next", _sensor_start_cb);
////    main_button = elm_button_add(main_layout);
////    elm_object_text_set(main_button,"<align=center><font_size=20>Go To <br> Sub Layout</font_size></align>");
////    evas_object_resize(main_button, 150, 50);
////    evas_object_move(main_button, 110, 200);
////    elm_object_content_set(main_layout, main_button);
////    evas_object_show(main_button);
////    elm_naviframe_item_push(naviframe, NULL, NULL, NULL, main_layout,"empty");
////
////    evas_object_smart_callback_add(main_button, "clicked", create_sub_layout, naviframe);
//    // Show the window after base gui is set up
//    evas_object_show(ad->win);
//}

static bool app_create(void *data) {
	/*
	 * Hook to take necessary actions before main event loop starts
	 * Initialize UI resources and application's data
	 * If this function returns true, the main loop of application starts
	 * If this function returns false, the application is terminated
	 */
	create_base_gui((appdata_s *) data);

	return true;
}

int main(int argc, char *argv[]) {
	appdata_s ad;
	memset(&ad, 0x00, sizeof(appdata_s));

	ui_app_lifecycle_callback_s event_callback;
	memset(&event_callback, 0x00, sizeof(ui_app_lifecycle_callback_s));

	event_callback.create = app_create;

	int ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "ui_app_main() failed with error: %d",
				ret);

	return ret;
}
