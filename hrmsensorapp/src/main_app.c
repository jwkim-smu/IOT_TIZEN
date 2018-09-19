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
#include "user_callbacks.h"

extern int target;
extern int check_state1;
extern int check_state2;

int daychanged = 0;
int played = 0;
//extern float f, p, q;

sensor_listener_h listener;
sensor_listener_h listener1;

double hour_total[MAX_COUNT] = {0.0};
int measured_num[MAX_COUNT] = {0};
double hrm_avr[MAX_COUNT] = {0.0};

double p = 0.0;
double epsilon = 1.0;
double sensitivity = 100.0;
double count = 10;
double epsilon1 = 0.0;
double scale = 0.0;

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
app_get_data(const char *res_file_in, char *res_path_out, int res_path_max){
    char *res_path = NULL;
    res_path= "/opt/usr/media/Documents/";
    if (res_path) {
        snprintf(res_path_out, res_path_max, "%s%s", res_path, res_file_in);
    }
}

static void
write_file(char* filepath, char* buf){
	FILE *fp;
    fp = fopen(filepath, "a+");

    if(fp==NULL) {
    	//error
    	dlog_print(DLOG_INFO, "error", "%s", "error");
    }
    else {
		fputs(buf, fp);
    }
    fclose(fp);
}
double sgn(double x) {
	return x<0 ? -1 : 1;
}
double* randomN(){									 //data[MAX_COUNT] -> 10시간 동안 수집한 시간별 평균 심장박동
	static double data[MAX_COUNT];

	srand((unsigned int) time(NULL));

	for (int i = 0; i < MAX_COUNT; i++)				 //0~1 사이 실수 난수값
		data[i] = rand() / (double) RAND_MAX;

	return data;
}
double laplace(double mu, double b, double ran) {
	double u = (ran) - 0.5;            				 //0~1 random number
	printf("ran : %.2f / u : %.2f / sgn(u) : %.2f\n", ran, u, sgn(u));
	return mu - (b * sgn(u) * log(1 - 2 * fabs(u))); //fabs - double type absolute number
}

double privatize(double f, double sensitivity, double epsil, double ran) {
	return f + laplace(0.0, sensitivity / epsil, ran);
}

//float ran(int x){
//	float a = (float)((rand() % 100)/100.0);
//	dlog_print(DLOG_INFO, "ran", "%d %f", x, a);
//	return a;
//}

//socket
static void SM(char* msg, int port_num) {
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	//address.sin_addr.s_addr = inet_addr("192.168.0.197");
	//address.sin_addr.s_addr = inet_addr("117.16.123.192");
	address.sin_addr.s_addr = inet_addr("117.16.123.12");
	//address.sin_addr.s_addr = inet_addr("117.16.123.55");
	//address.sin_addr.s_addr = inet_addr("172.16.168.19");
	//address.sin_addr.ss_addr = inet_addr("203.237.172.100");
	//address.sin_port = htons(4427);
	address.sin_port = htons(port_num);
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

void hrm_average(int hour, double hrm) {

	time_t t = time(NULL);
	struct tm tm1 = *localtime(&t);


	int min = tm1.tm_min;

	int today = tm1.tm_yday;
	int nextday;

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


	if (today == 365) {
		nextday = 0;
	} else {
		nextday = today + 1;
	}

	int cur_hour = hour;
	//int tmp3 = atoi(tmp3);

	if(hrm != -3 && hrm != 0){
		switch (cur_hour) {
		case 0:
			hour_total[0] += hrm;
			measured_num[0]++;
			break;
		case 1:
			hour_total[1] += hrm;
			measured_num[1]++;
			break;
		case 2:
			hour_total[2] += hrm;
			measured_num[2]++;
			break;
		case 3:
			hour_total[3] += hrm;
			measured_num[3]++;
			break;
		case 4:
			hour_total[4] += hrm;
			measured_num[4]++;
			break;
		case 5:
			hour_total[5] += hrm;
			measured_num[5]++;
			break;
		case 6:
			hour_total[6] += hrm;
			measured_num[6]++;
			break;
		case 7:
			hour_total[7] += hrm;
			measured_num[7]++;
			break;
		case 8:
			hour_total[8] += hrm;
			measured_num[8]++;
			break;
		case 9:
			hour_total[9] += hrm;
			measured_num[9]++;
			break;
		case 10:
			hour_total[10] += hrm;
			measured_num[10]++;
			break;
		case 11:
			hour_total[11] += hrm;
			measured_num[11]++;
			break;
		case 12:
			hour_total[12] += hrm;
			measured_num[12]++;
			break;
		case 13:
			hour_total[13] += hrm;
			measured_num[13]++;
			break;
		case 14:
			hour_total[14] += hrm;
			measured_num[14]++;
			break;
		case 15:
			hour_total[15] += hrm;
			measured_num[15]++;
			break;
		case 16:
			hour_total[16] += hrm;
			measured_num[16]++;
			break;
		case 17:
			hour_total[17] += hrm;
			measured_num[17]++;
			break;
		case 18:
			hour_total[18] += hrm;
			measured_num[18]++;
			break;
		case 19:
			hour_total[19] += hrm;
			measured_num[19]++;
			break;
		case 20:
			hour_total[20] += hrm;
			measured_num[20]++;
			break;
		case 21:
			hour_total[21] += hrm;
			measured_num[21]++;
			break;
		case 22:
			hour_total[22] += hrm;
			measured_num[22]++;
			break;
		case 23:
			hour_total[23] += hrm;
			measured_num[23]++;
			break;
		}
	}

	epsilon1 = epsilon / count;
	scale = sensitivity / epsilon1;
	//if 조건문에 day 바뀌는 조건 추가할 것 - done
	//hrm_avr[23]에 노이즈 추가 하기 - done
	//hrm_avr[23]에 노이즈 추가한 것을 서버로 전송하기 - done
	//if (tm1.tm_yday == nextday) {
	if(tm1.tm_hour == 0){
		daychanged = 1;
	}else if(tm1.tm_hour == 1){
		daychanged = 0;
		played = 0;
	}

	if(daychanged == 1 && played == 0){

		double *rando = randomN();
		char resultValue[100] = "";

		for (int i = 0; i < MAX_COUNT; i++) {
			char tmpLine3[100] = "";
			char tmp3[100]= "";

			//시간대별 평균 구하기
			hrm_avr[i] = hour_total[i] / measured_num[i];
			//hrm_avr[23]에 노이즈 추가 하기
			printf("%.2f\n", privatize(hrm_avr[i], 100.0, epsilon1, rando[i]));
			//char형 버퍼에 담기
			sprintf(tmpLine3, "%.2f", privatize(hrm_avr[i], sensitivity, epsilon1, rando[i]));

			strcat(resultValue,tmpLine3);
			strcat(resultValue, "|");
			//hrm_avr[23]에 노이즈 추가 하기
			printf("%.2f\n", hrm_avr[i]);
			//char형 버퍼에 담기
			sprintf(tmp3, " raw: %.2f ", hrm_avr[i]);

			//히스토그램 서버로 전송
			//파일에 쓰기 - 로그확인용
			write_file(filepath, tmpLine3);
			write_file(filepath, tmp3);
		}
		SM(resultValue, 4430);

		daychanged = 0;
		played = 1;

		memset(&hour_total, 0, sizeof(hour_total));
		memset(&measured_num, 0, sizeof(measured_num));
		memset(&hrm_avr, 0, sizeof(hrm_avr));
	}

//	if (tm1.tm_min == 6) {
//		char hrm_avr_c[MAX_COUNT] = { 0, };
//		char tmpLine3[100] = "";
//		char tmp3[100]= "";
//		double *rando = randomN();
//
//			//시간대별 평균 구하기
//			hrm_avr[15] = hour_total[15] / measured_num[15];
//			//hrm_avr[23]에 노이즈 추가 하기
//			printf("%.2f\n", privatize(hrm_avr[15], 100.0, epsilon1, rando[15]));
//			//			//히스토그램 서버로 전송
//			//SM(hrm_avr_c[i], 4430);
//
//			sprintf(tmpLine3, "%.2f", privatize(hrm_avr[15], sensitivity, epsilon1, rando[15]));
//			strcpy(tmp3, tmpLine3);
//
//			//파일에 쓰기 - 로그확인용
//			write_file(filepath, tmp3);
//
//	}



//	    char tmpLine3[100] = "";
//	    char tmp3[100]= "";
//	    double *rando = randomN();
//
//		hrm_avr[22] = hour_total[22] / measured_num[22];
//
//		//hrm_avr[23]에 노이즈 추가 하기
//		printf("%.2f\n", privatize(hrm_avr[22], 100.0, epsilon1, rando[22]));
//		//char형 버퍼에 담기
//		sprintf(tmpLine3, "%.2f", privatize(hrm_avr[22], sensitivity, epsilon1, rando[22]));
//		//히스토그램 서버로 전송
//		SM(hrm_avr_c[22], 4430);
//		//파일에 쓰기 - 로그확인용
//		strcpy(tmp3, tmpLine3);
//
//		/*sprintf(tmpLine3, "%.2f", hrm_avr[14]);
//		strcpy(tmp3, tmpLine3);*/
//		write_file(fname, tmp3);
}


void on_sensor_event(sensor_h sensor, sensor_event_s *event, void *user_data) {
   //for SENSOR_HRM

   time_t t = time(NULL);
   struct tm tm1 = *localtime(&t);

   // int today = tm1.tm_mday;
   int today = tm1.tm_yday;
   int hour = 0;

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
      SM(ID, 4427);

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
         strcpy(tmp3, tmpLine3);      //write_file

         strcat(tmpLine3, "|");      //SM2

         SM(tmpLine3, 4428);

         if (check_state1 == 1 && check_state2 ==0) {
            write_file(filepath, "\r\n");
            write_file(filepath, ctime(&cur_time));
            write_file(filepath, " ");
            write_file(filepath, tmp3);
         }
         else if (check_state1 == 1 && check_state2 == 1) {
            write_file(filepath, "\r\n");
            write_file(filepath, ctime(&cur_time));
            write_file(filepath, " ");
            write_file(filepath, tmp3);
            write_file(filepath, " ");
         }
      }
      if(tm1.tm_hour != hour){
         hour = tm1.tm_hour;
         hrm_average(hour, event->values[0]);

      }else{
         hrm_average(hour, event->values[0]);
      }
   }
}

//void on_sensor_event(sensor_h sensor, sensor_event_s *event, void *user_data) {
//	//for SENSOR_HRM
//
//	time_t t = time(NULL);
//	struct tm tm1 = *localtime(&t);
//
//	// int today = tm1.tm_mday;
//	int today = tm1.tm_yday;
//	int nextday;
//
//
//	if(today == 365){
//		nextday = 0;
//	}else{
//		nextday = today + 1;
//	}
//
//	//해당 함수 호출시 초 check
//	int start = tm1.tm_sec;
//
//	//히스토그램 생성을 위한 시간 변수
//	cur_hour = tm1.tm_hour;//on_sensor_event()가 호출될 때 마다 현재 시간 구함
//
//
//
//
//	// Select a specific sensor with a sensor handle
//	sensor_type_e type;
//	sensor_get_type(sensor, &type);
//
//	//file에 현재시각 출력
//	time(&cur_time);
//
//	char filepath[PATH_MAX] = { 0, };
//
//	char fname[100] = "data";
//	char day[10] = "";
//	char fname1[100] = ".txt";
//
//	if (tm1.tm_yday == nextday) {
//		sprintf(day, "%d", nextday);
//		strcat(fname, day);
//		strcat(fname, fname1);
//	} else {
//		sprintf(day, "%d", today);
//		strcat(fname, day);
//		strcat(fname, fname1);
//	}
//
//	app_get_data(fname, filepath, PATH_MAX);
//
//	if(start==30 || start==0){
//		SM(ID, 4427);
//
//		char tmpLine2[100] = "";
//		char tmpLine3[100] = "";
//		char tmp3[100]= "";
//
//		if (type == SENSOR_HRM) {
//			/////////////////heartrate///////////////////
//			dlog_print(DLOG_INFO, LOG_TAG,
//					"<align=center><font_size=20>%d</font_size></align>",
//					event->values[0]);
//
//			sprintf(tmpLine2,
//					"<align=center><font_size=20>%d - %.2f</font_size></align>",
//					start, event->values[0]);
//
//			sprintf(tmpLine3, "%.2f", event->values[0]);
//			elm_object_text_set(event_label, tmpLine2);
//			dlog_print(DLOG_DEBUG, LOG_TAG, "%s", tmpLine3);
//			strcpy(tmp3, tmpLine3);		//write_file
//
//			strcat(tmpLine3, "|");		//SM2
//
//			//히스토그램 생성부
//			if(tm1.tm_hour == cur_hour){
//
//			}
//
//
//
//			SM(tmpLine3, 4428);
//
//			if (check_state1 == 1 && check_state2 ==0) {
//				write_file(filepath, "\r\n");
//				write_file(filepath, ctime(&cur_time));
//				write_file(filepath, " ");
//				write_file(filepath, tmp3);
//			}
//			else if (check_state1 == 1 && check_state2 == 1) {
//				write_file(filepath, "\r\n");
//				write_file(filepath, ctime(&cur_time));
//				write_file(filepath, " ");
//				write_file(filepath, tmp3);
//				write_file(filepath, " ");
//			}
//		}
////		char a[100], b[80] = "";
////
////		for (int i = 31; i < 101; i++) {
////			if ((int) event->values[0] == i) {
////				strcat(b, "1");
////				for (int j = i + 1; j < 101; j++) {
////					strcat(b, "0");
////				}
////				break;
////			}
////			strcat(b, "0");
////		}
////		sprintf(a, "<align=center>%f</align>", event->values[0]);
////
////		elm_object_text_set(event_label, a);
////		srand(time(NULL));
////		if (event->values[0] > 30) {
////			char tmpLine[80] = "";
////			for (int i = 0; i < 70; i++) {
////				bool check = false;
////				if (ran(1) < f)
////					check = true;
////				if (check) {
////					if (ran(2) < 0.5)
////						strcat(tmpLine, "1");
////					else
////						strcat(tmpLine, "0");
////				} else {
////					tmpLine[i] = b[i];
////				}
////			}
////			char tmpLine2[80];
////			for (int i = 0; i < 70; i++) {
////				if (tmpLine[i] == '0') {
////					if (ran(3) < p)
////						//tmpLine2[i] = '1';
////						strcat(tmpLine2, "1");
////					else
////						//tmpLine2[i] = '0';
////						strcat(tmpLine2, "0");
////				} else {
////					if (ran(4) < q)
////						strcat(tmpLine2, "1");
////					//tmpLine2[i] = '1';
////					else
////						strcat(tmpLine2, "0");
////					//tmpLine2[i] = '0';
////				}
////			}
////			dlog_print(DLOG_INFO, LOG_TAG, "%s", tmpLine2);
////			SM2(tmpLine2);
////			strcpy(tmpLine,"");
////			strcpy(tmpLine2,"");
////		}
//	}
//}

void on_sensor_event1(sensor_h sensor, sensor_event_s *event, void *user_data){
	//for SENSOR_HUMAN_PEDOMETER

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


			SM(tmpLine5, 4429);

			if (check_state1 == 0 && check_state2 == 1) {
				write_file(filepath, "\r\n");
				write_file(filepath, ctime(&cur_time));
				write_file(filepath, " ");
				write_file(filepath, tmpLine5);
			} else if (check_state1 == 1 && check_state2 == 1) {
				write_file(filepath, " ");
				write_file(filepath, tmpLine5);
			}
		}
	}
}

void _add_entry_text(const char *text) {
	Evas_Coord c_y;

	elm_entry_entry_append(GLOBAL_DEBUG_BOX, text);
	elm_entry_entry_append(GLOBAL_DEBUG_BOX, "<br>");
	elm_entry_cursor_end_set(GLOBAL_DEBUG_BOX);
	elm_entry_cursor_geometry_get(GLOBAL_DEBUG_BOX, NULL, &c_y, NULL, NULL);
	elm_scroller_region_show(GLOBAL_DEBUG_BOX, 0, c_y, 0, 0);
}

Evas_Object *_new_button(appdata_s *ad, Evas_Object *display, char *name,
		void *cb) {
	// Create a button
	bt = elm_button_add(display);
	elm_object_text_set(bt, name);
	evas_object_smart_callback_add(bt, "clicked", (Evas_Smart_Cb) cb, ad);
	evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_layout_content_set(display, NULL, bt);
	evas_object_show(bt);
	return bt;
}

void create_sub_layout(void *data, Evas_Object *obj, void *event_info) {

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



	////////////////////

//	Evas_Object *img = elm_image_add(sub_layout);
//	elm_image_file_set(img, "animation/animation_hrm.gif", NULL);
//	elm_image_animated_play_set(img, EINA_TRUE);
//	evas_object_size_hint_weight_set(img, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//	evas_object_size_hint_align_set(img, EVAS_HINT_FILL, EVAS_HINT_FILL);
//	evas_object_show(img);

//	   //현재 출력하고자 하는 이미지 파일이 애니메이션을 지원하는지 여부를 알린다.
//	   elm_image_animated_set(img, EINA_TRUE);
//	   //애니메이션을 시작하라고 명령한다.
//	   elm_image_animated_play_set(img, EINA_TRUE);
//	   evas_object_size_hint_weight_set(img, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//	   evas_object_size_hint_align_set(img, EVAS_HINT_FILL, EVAS_HINT_FILL);
//	   evas_object_show(img);
//	   elm_win_resize_object_add(sub_layout, img);
//
//	   evas_object_resize(sub_layout, 400, 400);
//	   evas_object_show(img);




	event_label = elm_label_add(sub_layout);
	elm_object_text_set(event_label,
			"<align=center><font_size=20>Press Start and Wait</font_size></align>");
	evas_object_move(event_label, 0, 145);
	evas_object_resize(event_label, 365, 30);
	evas_object_show(event_label);

	event_label1 = elm_label_add(sub_layout);
	elm_object_text_set(event_label1,
			"<align=center><font_size=20>Press Start and Wait</font_size></align>");
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
   evas_object_smart_callback_add(btnj, "clicked", _animation_test_display,
         naviframe);
   evas_object_move(btnj, 0, 310);
   evas_object_resize(btnj, 365, 30);
   evas_object_show(btnj);

   elm_naviframe_item_push(naviframe, NULL, NULL, NULL, main_layout1, "empty");

}


static void
app_get_resource(const char *edj_file_in, char *edj_path_out, int edj_path_max)
{
    char *res_path = app_get_resource_path();
    if (res_path) {
        snprintf(edj_path_out, edj_path_max, "%s%s", res_path, edj_file_in);
        free(res_path);
    }
}

void _animation_test_display(void *data, Evas_Object *obj, void *event_info){

	Evas_Object *naviframe = data;
	elm_naviframe_item_pop(naviframe);
	Evas_Object *sub_layout;
	sub_layout = elm_layout_add(naviframe);
	evas_object_size_hint_weight_set(sub_layout, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_show(sub_layout);

	 Evas_Object *loadingImg;
	 char buf[PATH_MAX]={0,};

	 app_get_resource("./images/animation_hrm.gif", buf, (int)PATH_MAX);
	 loadingImg = elm_image_add(sub_layout);
	 elm_image_file_set(loadingImg, buf, NULL);

	 evas_object_size_hint_align_set(loadingImg, EVAS_HINT_FILL, EVAS_HINT_FILL);
	 evas_object_size_hint_weight_set(loadingImg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	 if (elm_image_animated_available_get(loadingImg))
	 {
		 dlog_print(DLOG_INFO, LOG_TAG, "elm_image_animated_available_get )");
	  elm_image_animated_set(loadingImg, true);
	  elm_image_animated_play_set(loadingImg, true);
	 }

	 evas_object_show(loadingImg);


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
