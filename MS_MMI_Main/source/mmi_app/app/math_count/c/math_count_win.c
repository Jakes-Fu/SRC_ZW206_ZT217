//warning!!  this file must use encode :UTF8 WITH BOM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include "std_header.h"
#include <stdlib.h>
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmk_app.h"
#include "mmk_msg.h"
#include "mmicc_export.h"
#include "guitext.h"
#include "guilcd.h"
#include "guilistbox.h"
#include "guiedit.h"
#include "guilabel.h"
#include "guistring.h"
#include "mmi_default.h"
#include "mmi_common.h"
#include "mmidisplay_data.h"
#include "mmi_menutable.h"
#include "mmi_appmsg.h"
#include "mmipub.h"
#include "mmi_common.h"

#include "mmiidle_export.h"
#include "mmi_position.h"
#include "ldo_drvapi.h"
#include "mmi_resource.h"
#include "mmiset_id.h"
#include "version.h"
#include "mmicc_internal.h"
#include "mmisrvrecord_export.h"
#include "mmirecord_export.h"
#include "mmiphone_export.h"
#include "math_count_export.h"
#include "math_count_id.h"
#include "math_count_image.h"
#include "math_count_text.h"
#include "math_count_main.h"
#include "cafcontrol.h"
#include "os_api.h"
#include "cjson.h"
#include "types.h"
#include "mmidisplay_data.h"
#include "types.h"
#include "mmisrvrecord_export.h"
#include "mmisrvaud_api.h"
#include "mmiset_export.h"
#include "mmi_default.h"

#define MATCH_COUNT_USE_TWO_PAGE 0

#define MATCH_COUNT_TASK_SHOW_ITEM_MAX 3

#define MATCH_COUNT_KEYBOARD_HOR_NUM 4
#define MATCH_COUNT_KEYBOARD_VER_NUM 3
#define MATCH_COUNT_KEYBOARD_NUM MATCH_COUNT_KEYBOARD_HOR_NUM*MATCH_COUNT_KEYBOARD_VER_NUM

#define MATCH_COUNT_LINE_HIGHT MMI_MAINSCREEN_HEIGHT/12
#define MATCH_COUNT_TEXT_START 10
#define MATCH_COUNT_BLANK_START MMI_MAINSCREEN_WIDTH/7.5

#define MATCH_COUNT_KEYBOARD_WIDTH MMI_MAINSCREEN_WIDTH/10
#define MATCH_COUNT_KEYBOARD_HIGHT MMI_MAINSCREEN_HEIGHT/12

LOCAL GUI_RECT_T math_win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T math_title_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MATCH_COUNT_LINE_HIGHT};

LOCAL GUI_RECT_T set_time_txt_rect = {MATCH_COUNT_TEXT_START, MATCH_COUNT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-10, 2*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T set_time_black_rect  = {MATCH_COUNT_BLANK_START,2*MATCH_COUNT_LINE_HIGHT,MATCH_COUNT_BLANK_START,3*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T set_time_ctl_rect  = {MATCH_COUNT_BLANK_START,2*MATCH_COUNT_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START,3*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T set_time_reduce_btn_rect  = {MATCH_COUNT_TEXT_START,2*MATCH_COUNT_LINE_HIGHT,MATCH_COUNT_BLANK_START,3*(MMI_MAINSCREEN_HEIGHT/12)};
LOCAL GUI_RECT_T set_time_add_btn_rect  = {MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START,2*MATCH_COUNT_LINE_HIGHT,MMI_MAINSCREEN_WIDTH,3*MATCH_COUNT_LINE_HIGHT};

LOCAL GUI_RECT_T set_range_txt_rect = {MATCH_COUNT_TEXT_START, 3*MATCH_COUNT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-10, 4*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T set_question_range_black_rect  = {MATCH_COUNT_BLANK_START,4*MATCH_COUNT_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START,5*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T set_question_range_ctl_rect  = {MATCH_COUNT_BLANK_START,4*MATCH_COUNT_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START,5*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T set_question_range_reduce_btn_rect  = {MATCH_COUNT_TEXT_START,4*MATCH_COUNT_LINE_HIGHT,MATCH_COUNT_BLANK_START,5*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T set_question_range_add_btn_rect  = {MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START,4*MATCH_COUNT_LINE_HIGHT,MMI_MAINSCREEN_WIDTH,5*MATCH_COUNT_LINE_HIGHT};

LOCAL GUI_RECT_T set_symbol_txt_rect = {MATCH_COUNT_TEXT_START, 5*MATCH_COUNT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-10, 6*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T set_add_txt_rect = {MATCH_COUNT_BLANK_START, 6*MATCH_COUNT_LINE_HIGHT+10, 2*MATCH_COUNT_BLANK_START, 7*MATCH_COUNT_LINE_HIGHT+10};
LOCAL GUI_RECT_T set_add_but_rect = {2*MATCH_COUNT_BLANK_START, 6*MATCH_COUNT_LINE_HIGHT+10, 3*MATCH_COUNT_BLANK_START, 7*MATCH_COUNT_LINE_HIGHT+10};
LOCAL GUI_RECT_T set_minus_txt_rect = {4*MATCH_COUNT_BLANK_START, 6*MATCH_COUNT_LINE_HIGHT+10, 5*MATCH_COUNT_BLANK_START, 7*MATCH_COUNT_LINE_HIGHT+10};
LOCAL GUI_RECT_T set_minus_but_rect = {5*MATCH_COUNT_BLANK_START, 6*MATCH_COUNT_LINE_HIGHT+10, 6*MATCH_COUNT_BLANK_START, 7*MATCH_COUNT_LINE_HIGHT+10};

LOCAL GUI_RECT_T set_multi_txt_rect = {MATCH_COUNT_BLANK_START, 8*MATCH_COUNT_LINE_HIGHT, 2*MATCH_COUNT_BLANK_START, 9*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T set_multi_but_rect = {2*MATCH_COUNT_BLANK_START, 8*MATCH_COUNT_LINE_HIGHT, 3*MATCH_COUNT_BLANK_START, 9*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T set_division_txt_rect = {4*MATCH_COUNT_BLANK_START, 8*MATCH_COUNT_LINE_HIGHT, 5*MATCH_COUNT_BLANK_START, 9*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T set_division_but_rect = {5*MATCH_COUNT_BLANK_START, 8*MATCH_COUNT_LINE_HIGHT, 6*MATCH_COUNT_BLANK_START, 9*MATCH_COUNT_LINE_HIGHT};

LOCAL GUI_RECT_T math_count_page_rect = {MATCH_COUNT_BLANK_START, 10*MATCH_COUNT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START, MMI_MAINSCREEN_HEIGHT-10};
LOCAL GUI_RECT_T math_count_action_rect = {MATCH_COUNT_BLANK_START, 10*MATCH_COUNT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START, MMI_MAINSCREEN_HEIGHT-10};

LOCAL GUI_RECT_T show_time_text_rect = {0, MATCH_COUNT_LINE_HIGHT, MATCH_COUNT_BLANK_START, 2*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T show_time_bar_blank_rect = {MATCH_COUNT_BLANK_START, MATCH_COUNT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START, 2*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T show_time_bar_rect = {MATCH_COUNT_BLANK_START, MATCH_COUNT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START, 2*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T show_index_text_rect = {MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START, MATCH_COUNT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, 2*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T correct_wrong_img_rect = {MATCH_COUNT_BLANK_START, 5*MATCH_COUNT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START, MMI_MAINSCREEN_HEIGHT-10};
LOCAL GUI_RECT_T show_formula_rect = {MATCH_COUNT_BLANK_START,2*MATCH_COUNT_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START,5*MATCH_COUNT_LINE_HIGHT};

LOCAL GUI_RECT_T math_count_back_rect = {MATCH_COUNT_KEYBOARD_WIDTH-10, 9*MATCH_COUNT_LINE_HIGHT, 5*MATCH_COUNT_KEYBOARD_WIDTH-10, 11*MATCH_COUNT_LINE_HIGHT};
LOCAL GUI_RECT_T math_count_again_rect = {5*MATCH_COUNT_KEYBOARD_WIDTH+10, 9*MATCH_COUNT_LINE_HIGHT, 9*MATCH_COUNT_KEYBOARD_WIDTH+10, 11*MATCH_COUNT_LINE_HIGHT};

LOCAL GUI_RECT_T math_count_keyboard_rect[MATCH_COUNT_KEYBOARD_NUM] = {0};
LOCAL uint8 is_can_click=0;
LOCAL int16 main_tp_down_x = 0;
LOCAL int16 main_tp_down_y = 0;
LOCAL uint8 set_question_time=0;
LOCAL uint8 counting_used_time=0;
LOCAL uint8 counting_question_index=0;
LOCAL uint16 set_question_range=0;
LOCAL uint8 choose_add_symbol=0;
LOCAL uint8 choose_minus_symbol=0;
LOCAL uint8 choose_multi_symbol=0;
LOCAL uint8 choose_division_symbol=0;
LOCAL uint8 counting_timer_id=0;
LOCAL uint8 show_correct_wrong_status=0;//0 dont show  1 correct  2 wrong
LOCAL uint16 correct_answer=0;
LOCAL char question_formula_text[40]={0};
LOCAL uint16 target_number = 0;
LOCAL uint16 count_part_number_1 = 0;
LOCAL uint16 count_part_number_2 = 0;
LOCAL uint8 type_rand = 0;
LOCAL uint8 show_question_mark = 0;
LOCAL uint16 user_input_number = 0;
LOCAL uint16 user_input_forbidden = 0;
LOCAL uint32 chengchu_arr_rand_num = 0;
LOCAL uint8 correct_answer_num = 0;
LOCAL uint8 get_score_status = 0; //0加载中 1加载成功 2加载失败 显示重试按钮 3更新中
LOCAL char get_score_list_url[255];
LOCAL uint8 math_learn_count = 30;
LOCAL BOOLEAN is_homework_task = FALSE;
LOCAL char homework_task_id[50] = {0};
LOCAL uint16 homework_task_left_num = 20;
LOCAL rank_info_struct_struct rank_name_score_info[7]={0};
LOCAL int math_count_task_total = -1;
LOCAL uint8 math_count_task_cur_page = 1;
LOCAL uint8 math_count_page_index = 0;

LOCAL uint8 check_prime_number_under_100[25]={2,3,5,7,11,13,17,19,23,29,31,37,41,45,47,53,59,61,67,71,73,79,83,89,97};
LOCAL uint8 check_prime_number_under_200[21]={1,3,7,9,13,27,31,37,39,49,51,57,63,67,73,79,81,91,93,97,99};
LOCAL uint8 check_prime_number_under_300[16]={11,23,27,29,33,39,41,51,57,63,69,71,77,81,83,93};
LOCAL uint8 check_prime_number_under_400[16]={7,11,13,17,31,37,47,49,53,59,67,73,79,83,89,97};
LOCAL uint8 check_prime_number_under_500[17]={1,9,19,21,31,33,39,43,49,57,61,63,67,79,87,91,99};
LOCAL uint8 check_prime_number_under_600[14]={3,9,21,23,41,47,57,63,69,71,77,87,93,99};
LOCAL uint8 check_prime_number_under_700[16]={1,7,13,17,19,31,41,43,47,53,59,61,73,77,83,91};
LOCAL uint8 check_prime_number_under_800[14]={1,9,19,27,33,39,43,51,57,61,69,76,87,97};
LOCAL uint8 check_prime_number_under_900[15]={9,11,21,23,27,29,39,53,57,59,63,77,81,83,87};
LOCAL uint8 check_prime_number_under_1000[14]={7,11,19,29,37,41,47,53,67,71,77,83,91,97};

LOCAL void MMIZMT_CreateScoreWin(void);
LOCAL void MMIZMT_CreateCountingWin(void);
LOCAL void Counting_1S_Timeout(uint8 timer_id, uint32 param);
LOCAL void Question_generate(void);
LOCAL void Show_1S_Correct_Or_Wrong(uint8 timer_id, uint32 param);
LOCAL void post_score(char* class_id,char* imei,char* student_name);

LOCAL void Judge_correct_or_wrong(void){
	counting_question_index++;
	user_input_forbidden = 1;
	if(show_question_mark){
		show_correct_wrong_status = 2;
	}
	else if(user_input_number == target_number)
	{
		show_correct_wrong_status = 1;
		correct_answer_num ++;
	}
	else
	{
		show_correct_wrong_status = 2;
	}
	if(counting_timer_id != 0)
	{
		MMK_StopTimer(counting_timer_id);
		counting_timer_id = 0;
	}
	counting_timer_id = MMK_CreateTimerCallback(1000, Show_1S_Correct_Or_Wrong, PNULL, TRUE);
	MMK_StartTimerCallback(counting_timer_id,1000,	Show_1S_Correct_Or_Wrong,PNULL,FALSE);
	MMK_SendMsg(MATH_COUNT_COUNTING_WIN_ID, MSG_FULL_PAINT, PNULL);
	Question_generate();
}

LOCAL void Show_1S_Correct_Or_Wrong(uint8 timer_id, uint32 param)
{
	if(counting_timer_id != 0){
		MMK_StopTimer(counting_timer_id);
		counting_timer_id = 0;
	}

	user_input_forbidden = 0;
	show_question_mark = 1;
	show_correct_wrong_status = 0;

	counting_used_time = 0;
	counting_timer_id = MMK_CreateTimerCallback(1000, Counting_1S_Timeout, PNULL, TRUE);
	MMK_StartTimerCallback(counting_timer_id, 1000,Counting_1S_Timeout,PNULL,TRUE);
	MMK_SendMsg(MATH_COUNT_COUNTING_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL void Counting_1S_Timeout(uint8 timer_id, uint32 param)
{
  	counting_used_time++;
	if(counting_used_time == set_question_time){
		MMK_StopTimer(counting_timer_id);
		counting_timer_id = 0;

		if(counting_question_index >= math_learn_count-1){
			//todo 答完30题了 展示结算画面 排行榜
			if(!show_question_mark && user_input_number == target_number){
				correct_answer_num ++;
			}
			MMIZMT_CreateScoreWin();
			MMK_CloseWin(MATH_COUNT_COUNTING_WIN_ID);
		}else{
			Judge_correct_or_wrong();
		}
	}else{
		MMK_SendMsg(MATH_COUNT_COUNTING_WIN_ID, MSG_FULL_PAINT, PNULL);
	}
}

LOCAL int target_number_prime(int min, int max, int target)
{
	int is_prime = 0;
	int all[200] = {0};
	int number = 0;
	int i = 0;
	for (; min <= max; min++)
	{
		for (number = 2; number < min; number++)
		{
			if (min % number == 0)
				break;
		}
		if(number == min && i < 200){
			all[i] = number;
			i++;
		}
	}
	for(i = 0;i < sizeof(all);i++)
	{
		//SCI_TRACE_LOW("%s: target = %d, all[%d] = %d", __FUNCTION__, target, i, all[i]);
		if(all[i] == 0){
			break;
		}
		if(target == all[i]){
			is_prime = 1;
			break;
		}
	}
	return is_prime;
}

LOCAL int is_target_prime(int target, int all[])
{
	uint8 i = 0;
	int is_prime = 0;
	for(i = 0;i < sizeof(all);i++)
	{
		SCI_TRACE_LOW("%s: target = %d, all[%d] = %d", __FUNCTION__, target, i, all[i]);
		if(target == all[i]){
			is_prime = 1;
			break;
		}
	}
	return is_prime;
}

LOCAL void Question_generate(void)
{
	uint8 type_count=0;
	uint8 for_i = 0;
	type_rand = 0;
	target_number = 0;
	count_part_number_1 = 0;
	count_part_number_2 = 0;

	type_count=type_count + choose_add_symbol;
	type_count=type_count + choose_minus_symbol;
	type_count=type_count + choose_multi_symbol;
	type_count=type_count + choose_division_symbol;

	type_rand = rand() % type_count;
	switch(type_rand){
		case 0://选第1个
			if(choose_add_symbol){
				type_rand = 0;//从这里往后0 1 2 3代表加减乘除
				break;
			}
			if(choose_minus_symbol){
				type_rand = 1;//从这里往后0 1 2 3代表加减乘除
				break;
			}
			if(choose_multi_symbol){
				type_rand = 2;//从这里往后0 1 2 3代表加减乘除
				break;
			}
			type_rand = 3;//从这里往后0 1 2 3代表加减乘除
		break;
		case 1://选第2个
			if(choose_add_symbol){
				type_rand --;
			}
			if(choose_minus_symbol){
				if(!type_rand){
					type_rand = 1;
					break;
				}else{
					type_rand--;
				}
			}
			if(choose_multi_symbol){
				if(!type_rand){
					type_rand = 2;
					break;
				}
			}
			type_rand = 3;
		break;
		case 2://选第三个
			if(choose_add_symbol){
				type_rand --;
			}
			if(choose_minus_symbol){
				type_rand --;
			}
			if(choose_multi_symbol){
				if(!type_rand){
					type_rand = 2;
					break;
				}
			}
			type_rand = 3;
		break;
		case 3:
			type_rand = 3;
		break;
	}

	target_number = rand() % set_question_range + 1;
	switch(type_rand){
		uint16 temp_number = 0;
		case 0://加
			{
				if((rand() % 2)){
					count_part_number_1 = rand() % target_number;
					count_part_number_2 = target_number - count_part_number_1 ;
				}else{
					type_rand = 4;//加法 但是空在等号左边
					count_part_number_1 = rand() % target_number;
					count_part_number_2 = target_number - count_part_number_1;
					temp_number = count_part_number_2;
					count_part_number_2 = target_number;
					target_number = temp_number;
				}
			}
			break;
		case 1://减
			{
				if((rand() % 2)){
					count_part_number_1 = rand() % target_number;
					count_part_number_2 = target_number - count_part_number_1;
					temp_number = target_number;
					target_number = count_part_number_1;
					count_part_number_1 = temp_number;
				}else{
					type_rand = 5;//减法 但是空在等号左边
					count_part_number_1 = rand() % target_number;
					count_part_number_2 = target_number - count_part_number_1;
					temp_number = target_number;
					target_number = count_part_number_1;
					count_part_number_1 = temp_number;
				}
			}
			break;
		case 2://乘
		case 3://除
			{
				uint16 part_number_arr[50]={0};
				uint16 for_i =1;
				uint8 is_zhishu = 0;
				uint16 part_number_arr_size = 0;
				int do_count = 0;
				do{
					is_zhishu = target_number_prime(2, set_question_range, target_number);
					//SCI_TRACE_LOW("%s: is_zhishu = %d", __FUNCTION__, is_zhishu);
					if((is_zhishu != 1 && target_number != 1) || target_number > set_question_range)break;
					target_number = rand() % set_question_range + 1;
				}while(1);
				//SCI_TRACE_LOW("%s: target_number = %d", __FUNCTION__, target_number);
				for(for_i=1;for_i<target_number;for_i++)
				{
					if(target_number%for_i==0){
						part_number_arr[part_number_arr_size] = for_i;
						part_number_arr_size ++;
						//SCI_TRACE_LOW("%s: part_number_arr = %d", __FUNCTION__, for_i);
					}
				}
				if(chengchu_arr_rand_num % part_number_arr_size == 0){
					chengchu_arr_rand_num+=rand()% part_number_arr_size+1;
				}
				if((rand() % 2)){
					if(type_rand == 2){
						count_part_number_1 = part_number_arr[chengchu_arr_rand_num % part_number_arr_size];
						count_part_number_2 = target_number / count_part_number_1;
					}else{
						uint16 temp_number = 0;
						count_part_number_1 = part_number_arr[chengchu_arr_rand_num % part_number_arr_size];
						count_part_number_2 = target_number / count_part_number_1;
						temp_number = target_number;
						target_number = count_part_number_1;
						count_part_number_1 = temp_number;
					}
				}else{//乘 但是空在等号左边
					if(type_rand == 2){
						uint16 temp_number = 0;
						count_part_number_1 = part_number_arr[chengchu_arr_rand_num % part_number_arr_size];
						count_part_number_2 = target_number / count_part_number_1;
						temp_number = target_number;
						target_number = count_part_number_2;
						count_part_number_2 = temp_number;
						type_rand = 6;
					}else{
						uint16 temp_number = 0;
						count_part_number_1 = part_number_arr[chengchu_arr_rand_num % part_number_arr_size];
						count_part_number_2 = target_number / count_part_number_1;
						temp_number = target_number;
						target_number = count_part_number_1;
						count_part_number_1 = temp_number;
						type_rand = 7;
					}
				}
			}
		break;
	}
	chengchu_arr_rand_num ++;
}

LOCAL int MathCount_GetClickIndex(GUI_POINT_T point)
{
	int click_num = -1;
	uint8 i = 0;
	for(i = 0;i < MATCH_COUNT_KEYBOARD_NUM;i++){
		if(GUI_PointIsInRect(point, math_count_keyboard_rect[i]))
		{
			click_num = ++i;
			break;
		}
	}
	return click_num;
}

LOCAL void MathCount_ShowKeyboard(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
	MMI_STRING_T text_string = {0};
	GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
	GUISTR_STYLE_T text_style = {0};
	uint16 tmp[5] = {0};
	char tmp_char[5] = {0};
	GUI_RECT_T hor_line_rect = {MATCH_COUNT_KEYBOARD_WIDTH, 5*MATCH_COUNT_KEYBOARD_HIGHT, 9*MATCH_COUNT_KEYBOARD_WIDTH+1, 5*MATCH_COUNT_KEYBOARD_HIGHT+1};
	GUI_RECT_T ver_line_rect = {MATCH_COUNT_KEYBOARD_WIDTH, 5*MATCH_COUNT_KEYBOARD_HIGHT, MATCH_COUNT_KEYBOARD_WIDTH+1, 11*MATCH_COUNT_KEYBOARD_HIGHT};
	GUI_RECT_T num_rect = {0};
	GUI_RECT_T num_rect_t = {0};
	uint8 i,j,k = 0;
	uint8 num_str[MATCH_COUNT_KEYBOARD_VER_NUM][MATCH_COUNT_KEYBOARD_HOR_NUM] = {"1230","456X","789"};
	
	for(i = 0;i < MATCH_COUNT_KEYBOARD_VER_NUM + 1;i++){
		LCD_DrawRect(&lcd_dev_info, hor_line_rect, MMI_BLACK_COLOR);
		hor_line_rect.top += 2*MATCH_COUNT_KEYBOARD_HIGHT;
		hor_line_rect.bottom = hor_line_rect.top + 1;
	}
	for(i = 0;i < MATCH_COUNT_KEYBOARD_HOR_NUM + 1;i++){
		LCD_DrawRect(&lcd_dev_info, ver_line_rect, MMI_BLACK_COLOR);
		ver_line_rect.left+= 2*MATCH_COUNT_KEYBOARD_WIDTH;
		ver_line_rect.right= ver_line_rect.left + 1;
	}
	hor_line_rect.top = 5*MATCH_COUNT_KEYBOARD_HIGHT;
	hor_line_rect.bottom = 5*MATCH_COUNT_KEYBOARD_HIGHT+1;
	ver_line_rect.left = MATCH_COUNT_KEYBOARD_WIDTH;
	ver_line_rect.right = MATCH_COUNT_KEYBOARD_WIDTH+1;
	text_style.align = ALIGN_HVMIDDLE;
	text_style.font = SONG_FONT_20;
	text_style.font_color = MMI_BLACK_COLOR;
	for(i = 0;i < MATCH_COUNT_KEYBOARD_VER_NUM;i++){
		num_rect.top = hor_line_rect.top;
		hor_line_rect.top += 2*MATCH_COUNT_KEYBOARD_HIGHT;
		num_rect.bottom = hor_line_rect.top;
		num_rect.left = 0;
		num_rect.right = 0;
		ver_line_rect.left = MATCH_COUNT_KEYBOARD_WIDTH;
		ver_line_rect.right = MATCH_COUNT_KEYBOARD_WIDTH+1;
		for(j = 0;j < MATCH_COUNT_KEYBOARD_HOR_NUM;j++){
			num_rect.left = ver_line_rect.left;
			ver_line_rect.left+= 2*MATCH_COUNT_KEYBOARD_WIDTH;
			num_rect.right = ver_line_rect.left;
			memset(&tmp_char, 0, 5);
			if(i == MATCH_COUNT_KEYBOARD_VER_NUM - 1 && j == MATCH_COUNT_KEYBOARD_HOR_NUM - 1)
			{
				sprintf(tmp_char,"%s", "OK");
			}
			else
			{
				sprintf(tmp_char,"%c", num_str[i][j]);
			}
			GUI_GBToWstr(tmp,tmp_char,strlen(tmp_char));
			text_string.wstr_ptr=tmp;
			text_string.wstr_len= (MMIAPICOM_Wstrlen(tmp));
			num_rect_t = num_rect;
			num_rect_t.top += 1;
			GUISTR_DrawTextToLCDInRect(
				(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
				&num_rect_t,
				&num_rect_t,
				&text_string,
				&text_style,
				text_state,
				GUISTR_TEXT_DIR_AUTO
				);
			math_count_keyboard_rect[k] = num_rect;
			k++;
		}
	}
}


LOCAL MMI_RESULT_E HandleCountingWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
			{
				show_correct_wrong_status = 0;
				counting_question_index=0;
 				counting_used_time=0;

				show_question_mark = 1;
				user_input_number=0;
				user_input_forbidden = 0;
				correct_answer_num = 0;


				counting_timer_id = MMK_CreateTimerCallback(1000, Counting_1S_Timeout, PNULL, TRUE);
				MMK_StartTimerCallback(
					counting_timer_id, 
					1000,
					Counting_1S_Timeout,
					PNULL,
					TRUE
					);

				srand(TM_GetTotalSeconds());
				chengchu_arr_rand_num = rand()%1000;
				Question_generate();
				MMK_SendMsg(MATH_COUNT_COUNTING_WIN_ID, MSG_FULL_PAINT, PNULL);
			}
			break;
		case MSG_GET_FOCUS:
			
			break;
		case MSG_FULL_PAINT:
			{
				MMI_STRING_T text_string = {0};
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
				GUISTR_STYLE_T text_style = {0};
				char time_and_index_text_temp_char[200]={0};
				MMI_STRING_T time_and_index_text_str = {0};
				uint16 time_and_index_tmp[100] = {0};
				uint8 times_total_width = MMI_MAINSCREEN_WIDTH-2*MATCH_COUNT_BLANK_START;

				GUI_FillRect(&lcd_dev_info, math_win_rect, MMI_WHITE_COLOR);
				
				MathCount_ShowKeyboard(win_id, lcd_dev_info);

				if(show_correct_wrong_status){
					switch(show_correct_wrong_status){
						case 1:
							GUIRES_DisplayImg(PNULL, &correct_wrong_img_rect, PNULL, win_id, MATH_COUNT_CORRECT_IMG, &lcd_dev_info);
						break;
						case 2:
							GUIRES_DisplayImg(PNULL, &correct_wrong_img_rect, PNULL, win_id, MATH_COUNT_WRONG_IMG, &lcd_dev_info);
						break;
					}
				}
				{	                                
					uint16 tmp[50] = {0};
					char tmp_char[100] = {0};
					MMI_STRING_T text_str = {0};
					GUISTR_STYLE_T formula_text_style = {0};
					GUISTR_STATE_T formula_text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_WORDBREAK;
					switch(type_rand){
						case 0://加
							if(show_question_mark){
								sprintf(tmp_char,"%d+%d=[?]",count_part_number_1,count_part_number_2);
							}else{
								sprintf(tmp_char,"%d+%d=[%d]",count_part_number_1,count_part_number_2,user_input_number);
							}
						break;
						case 1://减
							if(show_question_mark){
								sprintf(tmp_char,"%d-%d=[?]",count_part_number_1,count_part_number_2);
							}else{
								sprintf(tmp_char,"%d-%d=[%d]",count_part_number_1,count_part_number_2,user_input_number);
							}
						break;
						case 2://乘
							if(show_question_mark){
								sprintf(tmp_char,"%dx%d=[?]",count_part_number_1,count_part_number_2);
							}else{
								sprintf(tmp_char,"%dx%d=[%d]",count_part_number_1,count_part_number_2,user_input_number);
							}
						break;
						case 3://除
							if(show_question_mark){
								sprintf(tmp_char,"%d÷%d=[?]",count_part_number_1,count_part_number_2);
							}else{
								sprintf(tmp_char,"%d÷%d=[%d]",count_part_number_1,count_part_number_2,user_input_number);
							}
						break;
						case 4://加 但是空在等号左边
							if(show_question_mark){
								sprintf(tmp_char,"%d+[?]=%d",count_part_number_1,count_part_number_2);
							}else{
								sprintf(tmp_char,"%d+[%d]=%d",count_part_number_1,user_input_number,count_part_number_2);
							}
						break;
						case 5://减 但是空在等号左边
							if(show_question_mark){
								sprintf(tmp_char,"%d-[?]=%d",count_part_number_1,count_part_number_2);
							}else{
								sprintf(tmp_char,"%d-[%d]=%d",count_part_number_1,user_input_number,count_part_number_2);
							}
						break;
						case 6://乘 但是空在等号左边
							if(show_question_mark){
								sprintf(tmp_char,"%dx[?]=%d",count_part_number_1,count_part_number_2);
							}else{
								sprintf(tmp_char,"%dx[%d]=%d",count_part_number_1,user_input_number,count_part_number_2);
							}
						break;
						case 7://除 但是空在等号左边
							if(show_question_mark){
								sprintf(tmp_char,"%d÷[?]=%d",count_part_number_1,count_part_number_2);
							}else{
								sprintf(tmp_char,"%d÷[%d]=%d",count_part_number_1,user_input_number,count_part_number_2);
							}
						break;
					}
					GUI_GBToWstr(tmp,tmp_char,strlen(tmp_char));
					text_str.wstr_ptr=tmp;
					text_str.wstr_len= (MMIAPICOM_Wstrlen(tmp));
					formula_text_style.align = ALIGN_HVMIDDLE;
					formula_text_style.font = SONG_FONT_28;
					formula_text_style.font_color = MMI_BLACK_COLOR;
					GUISTR_DrawTextToLCDInRect(
						(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
						&show_formula_rect,
						&show_formula_rect,
						&text_str,
						&formula_text_style,
						formula_text_state,
						GUISTR_TEXT_DIR_AUTO
						);
				}


				LCD_DrawRect(&lcd_dev_info, show_time_bar_blank_rect, MMI_BLACK_COLOR);
				if(set_question_time != counting_used_time){
					show_time_bar_rect.right = MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START - (((set_question_time-counting_used_time)*times_total_width)/set_question_time);
				}else{
					show_time_bar_rect.right = MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START;
				}
				GUI_FillRect(&lcd_dev_info, show_time_bar_rect, MMI_BLACK_COLOR);
				
				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = SONG_FONT_16;
				text_style.font_color = MMI_BLACK_COLOR;
				text_state = GUISTR_STATE_ALIGN;
				sprintf(time_and_index_text_temp_char,"%dS",(set_question_time-counting_used_time));
				GUI_GBToWstr(time_and_index_tmp,time_and_index_text_temp_char,strlen(time_and_index_text_temp_char));
				time_and_index_text_str.wstr_ptr=time_and_index_tmp;
				time_and_index_text_str.wstr_len= MMIAPICOM_Wstrlen(time_and_index_tmp);

				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&show_time_text_rect,
					&show_time_text_rect,
					&time_and_index_text_str,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);

				show_index_text_rect = math_title_rect;
				SCI_MEMSET(time_and_index_tmp,0,200);
				sprintf(time_and_index_text_temp_char,"%d/%d",(counting_question_index+1), math_learn_count);
				GUI_GBToWstr(time_and_index_tmp,time_and_index_text_temp_char,strlen(time_and_index_text_temp_char));
				time_and_index_text_str.wstr_ptr=time_and_index_tmp;
				time_and_index_text_str.wstr_len= MMIAPICOM_Wstrlen(time_and_index_tmp);
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&show_index_text_rect,
					&show_index_text_rect,
					&time_and_index_text_str,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);
			}
			break;
		case MSG_CTL_PENOK:
			{
			}
			break;
		case MSG_TP_PRESS_UP:
			{
				GUI_POINT_T point = {0};
				int16 tp_offset_x;
				int16 tp_offset_y;
				point.x = MMK_GET_TP_X(param);
				point.y = MMK_GET_TP_Y(param);
				tp_offset_x =  point.x- main_tp_down_x;
				tp_offset_y = point.y - main_tp_down_y;

				if(ABS(tp_offset_x) <20&&ABS(tp_offset_y)<20){//属于静态点击
					if(point.x < math_count_keyboard_rect[0].left
						|| point.x > math_count_keyboard_rect[MATCH_COUNT_KEYBOARD_NUM-1].right
						|| point.y < math_count_keyboard_rect[0].top
						|| point.y > math_count_keyboard_rect[MATCH_COUNT_KEYBOARD_NUM-1].bottom)
					{
						SCI_TRACE_LOW("%s: not click keyboard!!", __FUNCTION__);
						break;
					}
					switch(MathCount_GetClickIndex(point))
						{
							case 1:///1
							{
								if(show_question_mark){
									show_question_mark = 0;
									user_input_number = 1;
									MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
								}else{
									if(user_input_number<1000){
										user_input_number = user_input_number*10 +1;
										MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
									}
								}
							}
							break;
							case 2:///2
							{
								if(show_question_mark){
									show_question_mark = 0;
									user_input_number = 2;
									MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
								}else{
									if(user_input_number<1000){
										user_input_number = user_input_number*10 +2;
										MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
									}
								}
							}
							break;
							case 3:///3
							{
								if(show_question_mark){
									show_question_mark = 0;
									user_input_number = 3;
									MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
								}else{
									if(user_input_number<1000){
										user_input_number = user_input_number*10 +3;
										MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
									}
								}
							}
							break;
							case 4:///0
							{
								if(show_question_mark){
									show_question_mark = 0;
									user_input_number = 0;
									MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
								}else{
									if(user_input_number<1000){
										user_input_number = user_input_number*10 +0;
										MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
									}
								}
							}
							break;
							case 5:///4
							{
								if(show_question_mark){
									show_question_mark = 0;
									user_input_number = 4;
									MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
								}else{
									if(user_input_number<1000){
										user_input_number = user_input_number*10 +4;
										MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
									}
								}
							}
							break;
							case 6:///5
							{
								if(show_question_mark){
									show_question_mark = 0;
									user_input_number = 5;
									MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
								}else{
									if(user_input_number<1000){
										user_input_number = user_input_number*10 +5;
										MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
									}
								}
							}
							break;
							case 7:///6
							{
								if(show_question_mark){
									show_question_mark = 0;
									user_input_number = 6;
									MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
								}else{
									if(user_input_number<1000){
										user_input_number = user_input_number*10 +6;
										MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
									}
								}
							}
							break;
							case 8:///删除
							{
								if(!show_question_mark){
									if(user_input_number<10){
										user_input_number = 0;
										show_question_mark = 1;
										MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
									}else{
										user_input_number = user_input_number/10;
										MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
									}
								}
							}
							break;
							case 9:///7
							{
								if(show_question_mark){
									show_question_mark = 0;
									user_input_number = 7;
									MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
								}else{
									if(user_input_number<1000){
										user_input_number = user_input_number*10 +7;
										MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
									}
								}
							}
							break;
							case 10:///8
							{
								if(show_question_mark){
									show_question_mark = 0;
									user_input_number = 8;
									MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
								}else{
									if(user_input_number<1000){
										user_input_number = user_input_number*10 +8;
										MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
									}
								}
							}
							break;
							case 11:///9
							{
								if(show_question_mark){
									show_question_mark = 0;
									user_input_number = 9;
									MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
								}else{
									if(user_input_number<1000){
										user_input_number = user_input_number*10 +9;
										MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
									}
								}
							}
							break;
							case 12:///确定
							{
								if(counting_question_index >= math_learn_count-1){
									//todo 答完30题了 展示结算画面 排行榜
									if(!show_question_mark && user_input_number == target_number){
										correct_answer_num ++;
									}
									MMIZMT_CreateScoreWin();
									MMK_CloseWin(MATH_COUNT_COUNTING_WIN_ID);
								}else{
									Judge_correct_or_wrong();
								}
							}
							break;
							default:
								break;
					}
				}
			}
			break;
		case MSG_TP_PRESS_DOWN:
			{
				main_tp_down_x = MMK_GET_TP_X(param);
				main_tp_down_y = MMK_GET_TP_Y(param);
			}
			break;
		case MSG_KEYDOWN_RED:
			break;
		case MSG_KEYUP_RED:
		case MSG_APP_CANCEL:
			MMK_CloseWin(win_id);
			break;
		case MSG_CLOSE_WINDOW:
			{
				if (0 != counting_timer_id) {
					MMK_StopTimer(counting_timer_id);
					counting_timer_id = 0;
				}
				SCI_MEMSET(homework_task_id, 0, 50);
				is_homework_task = FALSE;
			}
			break;
		default:
			recode = MMI_RESULT_FALSE;
			break;
	}
	return recode;
}

LOCAL MMI_RESULT_E HandleScoreWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
			{
				
			}
			break;
		case MSG_FULL_PAINT:
			{
				MMI_STRING_T text_string = {0};
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
				GUISTR_STYLE_T text_style = {0};
				char total_score_temp_char[100]={0};
				MMI_STRING_T temp_mmi_string_str = {0};
				uint16 wchar_tmp[50] = {0};
				GUI_RECT_T show_score_text_rect = {0, 2*MATCH_COUNT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, 4*MATCH_COUNT_LINE_HIGHT};
				uint8 for_i = 0;

				GUI_FillRect(&lcd_dev_info, math_win_rect, MMI_WHITE_COLOR);

				MMIRES_GetText(MATH_COUNT_TITLE, win_id, &text_string);
				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = SONG_FONT_24;
				text_style.font_color = MMI_BLACK_COLOR;
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&math_title_rect,
					&math_title_rect,
					&text_string,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);

				text_style.font = SONG_FONT_22;
				sprintf(total_score_temp_char,"答对:%d题",correct_answer_num);
				GUI_GBToWstr(wchar_tmp,total_score_temp_char,strlen(total_score_temp_char));
				temp_mmi_string_str.wstr_ptr=wchar_tmp;
				temp_mmi_string_str.wstr_len= MMIAPICOM_Wstrlen(wchar_tmp);
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&show_score_text_rect,
					&show_score_text_rect,
					&temp_mmi_string_str,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);

				SCI_MEMSET(wchar_tmp,0,100);
				SCI_MEMSET(total_score_temp_char,0,100);
				sprintf(total_score_temp_char,"答错:%d题",(math_learn_count-correct_answer_num));
				GUI_GBToWstr(wchar_tmp,total_score_temp_char,strlen(total_score_temp_char));
				temp_mmi_string_str.wstr_ptr=wchar_tmp;
				temp_mmi_string_str.wstr_len= MMIAPICOM_Wstrlen(wchar_tmp);
				show_score_text_rect.top += 2*MATCH_COUNT_LINE_HIGHT;
				show_score_text_rect.bottom = show_score_text_rect.top+2*MATCH_COUNT_LINE_HIGHT;
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&show_score_text_rect,
					&show_score_text_rect,
					&temp_mmi_string_str,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);

				text_style.font = SONG_FONT_26;
				SCI_MEMSET(wchar_tmp,0,100);
				SCI_MEMSET(total_score_temp_char,0,100);
				sprintf(total_score_temp_char,"总分:%d分",(set_question_range*(35 - set_question_time)*(choose_add_symbol+choose_minus_symbol+choose_multi_symbol+choose_division_symbol)*correct_answer_num)/10);
				GUI_GBToWstr(wchar_tmp,total_score_temp_char,strlen(total_score_temp_char));
				temp_mmi_string_str.wstr_ptr=wchar_tmp;
				temp_mmi_string_str.wstr_len= MMIAPICOM_Wstrlen(wchar_tmp);
				show_score_text_rect.top += 2*MATCH_COUNT_LINE_HIGHT;
				show_score_text_rect.bottom = show_score_text_rect.top+2*MATCH_COUNT_LINE_HIGHT;
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&show_score_text_rect,
					&show_score_text_rect,
					&temp_mmi_string_str,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);

				text_style.font = SONG_FONT_24;
				LCD_DrawRect(&lcd_dev_info, math_count_back_rect, MMI_BLACK_COLOR);
				MMIRES_GetText(MATH_COUNT_COUNTING_BACK, win_id, &text_string);
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&math_count_back_rect,
					&math_count_back_rect,
					&text_string,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);

				LCD_DrawRect(&lcd_dev_info, math_count_again_rect, MMI_BLACK_COLOR);
				MMIRES_GetText(MATH_COUNT_AGAIN, win_id, &text_string);
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&math_count_again_rect,
					&math_count_again_rect,
					&text_string,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);
			}
			break;
		case MSG_TP_PRESS_UP:
			{
				GUI_POINT_T point = {0};
				point.x = MMK_GET_TP_X(param);
				point.y = MMK_GET_TP_Y(param);
				if(GUI_PointIsInRect(point, math_count_back_rect))
				{
					MMK_CloseWin(win_id);
				}
				else if(GUI_PointIsInRect(point, math_count_again_rect))
				{
					MMIZMT_CreateCountingWin();
					MMK_CloseWin(win_id);
				}
			}
			break;
		case MSG_TP_PRESS_DOWN:
			{

			}
			break;
		case MSG_KEYDOWN_RED:
			break;
		case MSG_KEYUP_RED:
		case MSG_APP_CANCEL:
			MMK_CloseWin(win_id);
			break;
		default:
			recode = MMI_RESULT_FALSE;
			break;
	}
	return recode;
}


LOCAL void ClickTimeOut(uint8 timer_id, uint32 param) {
	if (0 != is_can_click) {
		MMK_StopTimer(is_can_click);
		is_can_click= 0;
	}
}

LOCAL uint8 GetCanntClick(uint is_click) 
{
	if(0 == is_can_click)
	{
		if(is_click)//传1是设置timer的，传0是查询timer的
		{
			is_can_click = MMK_CreateTimerCallback(300, ClickTimeOut, 0, FALSE);
			MMK_StartTimerCallback(is_can_click, 300,ClickTimeOut,0,FALSE);
		}
		return 0;//0就可以点，只要is_can_click是0就能点
	}
	return 1;//返回1就不能点
}

LOCAL void SetTimeAdd(void){
	if(set_question_time == 20)return;
	set_question_time++;
	MMK_SendMsg(MATH_COUNT_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL void SetTimeReduce(void){
	if(set_question_time == 3)return;
	set_question_time--;
	MMK_SendMsg(MATH_COUNT_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL void SetRangeAdd(void){
	switch(set_question_range){
		case 1000:
			return;
		break;
		case 500:
			set_question_range = 1000;
		break;
		case 200:
			set_question_range = 500;
		break;
		case 100:
			set_question_range = 200;
		break;
		case 50:
			set_question_range = 100;
		break;
		case 25:
			set_question_range = 50;
		break;
		case 15:
			set_question_range = 25;
		break;
		case 10:
			set_question_range = 15;
		break;
	}
	MMK_SendMsg(MATH_COUNT_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL void SetRangeReduce(void){
		switch(set_question_range){
		case 1000:
			set_question_range = 500;
		break;
		case 500:
			set_question_range = 200;
		break;
		case 200:
			set_question_range = 100;
		break;
		case 100:
			set_question_range = 50;
		break;
		case 50:
			set_question_range = 25;
		break;
		case 25:
			set_question_range = 15;
		break;
		case 15:
			set_question_range = 10;
		break;
		case 10:
			return;
		break;
	}
	MMK_SendMsg(MATH_COUNT_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL);
}


LOCAL MMI_RESULT_E HandleMathCountWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
			{
				set_question_time = 15;
				set_question_range = 100;
				choose_add_symbol=1;
				choose_minus_symbol=0;
				choose_multi_symbol=0;
				choose_division_symbol=0;
			}
			break;
		case MSG_GET_FOCUS:
			{
				set_question_time = 15;
				set_question_range = 100;
				choose_add_symbol=1;
				choose_minus_symbol=0;
				choose_multi_symbol=0;
				choose_division_symbol=0;
			}
			break;
		case MSG_FULL_PAINT:
			{
				MMI_STRING_T text_string = {0};				
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
				GUISTR_STYLE_T text_style = {0};
				uint16 set_time_and_range_tmp[100] = {0};
				MMI_STRING_T set_time_and_range_text_str = {0};
				char set_time_and_range_temp_char[200]={0};				
				GUI_RECT_T page_rect = {0};
				GUI_RECT_T action_rect = {0};

				MMIRES_GetText(MATH_COUNT_TITLE, win_id, &text_string);

				GUI_FillRect(&lcd_dev_info, math_win_rect, MMI_WHITE_COLOR);
				
				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = SONG_FONT_24;
				text_style.font_color = MMI_BLACK_COLOR;
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&math_title_rect,
					&math_title_rect,
					&text_string,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);
                
			#if MATCH_COUNT_USE_TWO_PAGE != 0
				if(math_count_page_index == 0){
					GUI_GBToWstr(set_time_and_range_tmp,"下一页",strlen("下一页"));
				}else{
					GUI_GBToWstr(set_time_and_range_tmp,"上一页",strlen("上一页"));
					memset(&set_time_add_btn_rect, 0, sizeof(GUI_RECT_T));
					memset(&set_time_reduce_btn_rect, 0, sizeof(GUI_RECT_T));
					memset(&set_question_range_add_btn_rect, 0, sizeof(GUI_RECT_T));
					memset(&set_question_range_reduce_btn_rect, 0, sizeof(GUI_RECT_T));			
				}
				
				set_time_and_range_text_str.wstr_ptr = set_time_and_range_tmp;
				set_time_and_range_text_str.wstr_len = MMIAPICOM_Wstrlen(set_time_and_range_tmp);
				LCD_DrawRect(&lcd_dev_info, math_count_page_rect, MMI_BLACK_COLOR);
				page_rect = math_count_page_rect;
				page_rect.top += 4; 
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&page_rect,
					&page_rect,
					&set_time_and_range_text_str,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);
			#endif
                
				GUIBUTTON_SetRect(MMI_MATH_COUNT_SET_TIME_ADD_BTN,&set_time_add_btn_rect);
				GUIBUTTON_SetCallBackFunc(MMI_MATH_COUNT_SET_TIME_ADD_BTN,SetTimeAdd);
				
				GUIBUTTON_SetRect(MMI_MATH_COUNT_SET_TIME_REDUCE_BTN,&set_time_reduce_btn_rect);
				GUIBUTTON_SetCallBackFunc(MMI_MATH_COUNT_SET_TIME_REDUCE_BTN,SetTimeReduce);

				GUIBUTTON_SetRect(MMI_MATH_COUNT_SET_RANGE_ADD_BTN,&set_question_range_add_btn_rect);
				GUIBUTTON_SetCallBackFunc(MMI_MATH_COUNT_SET_RANGE_ADD_BTN,SetRangeAdd);

				GUIBUTTON_SetRect(MMI_MATH_COUNT_SET_RANGE_REDUCE_BTN,&set_question_range_reduce_btn_rect);
				GUIBUTTON_SetCallBackFunc(MMI_MATH_COUNT_SET_RANGE_REDUCE_BTN,SetRangeReduce);				

				text_style.font = SONG_FONT_16;
				memset(&set_time_and_range_tmp, 0, 100);
				GUI_GBToWstr(set_time_and_range_tmp,"开始挑战",strlen("开始挑战"));
				set_time_and_range_text_str.wstr_ptr = set_time_and_range_tmp;
				set_time_and_range_text_str.wstr_len = MMIAPICOM_Wstrlen(set_time_and_range_tmp);
				LCD_DrawRect(&lcd_dev_info, math_count_action_rect, MMI_BLACK_COLOR);
				action_rect = math_count_action_rect;
				action_rect.top += 4;
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&action_rect,
					&action_rect,
					&set_time_and_range_text_str,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);
			#if MATCH_COUNT_USE_TWO_PAGE != 0
				if(math_count_page_index == 0)
                    #endif
				{		
					uint8 blank_range_total = MMI_MAINSCREEN_WIDTH-2*MATCH_COUNT_BLANK_START;
					uint8 blank_time_range_item = blank_range_total/20;
					uint8 blank_question_range_item = blank_range_total/7;
					LCD_DrawRect(&lcd_dev_info, set_time_ctl_rect, MMI_BLACK_COLOR);
					LCD_DrawRect(&lcd_dev_info, set_question_range_ctl_rect, MMI_BLACK_COLOR);

					if(set_question_time < 3)set_question_time = 3;
					if(set_question_time > 20)set_question_time = 20;
					
					if(set_question_time == 20){
						set_time_black_rect.right = MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START;
					}else if(set_question_time == 3){
						set_time_black_rect.right=MATCH_COUNT_BLANK_START;
					}else{
						set_time_black_rect.right=MATCH_COUNT_BLANK_START+blank_time_range_item*set_question_time;
					}
					if(set_question_range < 10)set_question_range = 10;
					if(set_question_range > 1000)set_question_range = 1000;
					switch(set_question_range){
						case 10:
							set_question_range_black_rect.right = MATCH_COUNT_BLANK_START;
						break;
						case 15:
							set_question_range_black_rect.right = blank_question_range_item*2;
						break;
						case 25:
							set_question_range_black_rect.right = blank_question_range_item*3;
						break;
						case 50:
							set_question_range_black_rect.right = blank_question_range_item*4;
						break;
						case 100:
							set_question_range_black_rect.right = blank_question_range_item*5;
						break;
						case 200:
							set_question_range_black_rect.right = blank_question_range_item*6;
						break;
						case 500:
							set_question_range_black_rect.right = blank_question_range_item*7;
						break;
						case 1000:
							set_question_range_black_rect.right = MMI_MAINSCREEN_WIDTH-MATCH_COUNT_BLANK_START;
						break;
					}
					GUI_FillRect(&lcd_dev_info, set_time_black_rect, MMI_BLACK_COLOR);
					GUI_FillRect(&lcd_dev_info, set_question_range_black_rect, MMI_BLACK_COLOR);

					text_style.align = ALIGN_BOTTOM;
					sprintf(set_time_and_range_temp_char,"答题时间:%d S",set_question_time);
					GUI_GBToWstr(set_time_and_range_tmp,set_time_and_range_temp_char,strlen(set_time_and_range_temp_char));
					set_time_and_range_text_str.wstr_ptr=set_time_and_range_tmp;
					set_time_and_range_text_str.wstr_len= MMIAPICOM_Wstrlen(set_time_and_range_tmp);

					GUISTR_DrawTextToLCDInRect(
						(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
						&set_time_txt_rect,
						&set_time_txt_rect,
						&set_time_and_range_text_str,
						&text_style,
						text_state,
						GUISTR_TEXT_DIR_AUTO
						);

					SCI_MEMSET(set_time_and_range_temp_char,0,200);
					SCI_MEMSET(set_time_and_range_tmp,0,200);
					sprintf(set_time_and_range_temp_char,"答题范围:%d以内",set_question_range);
					GUI_GBToWstr(set_time_and_range_tmp,set_time_and_range_temp_char,strlen(set_time_and_range_temp_char));
					set_time_and_range_text_str.wstr_ptr=set_time_and_range_tmp;
					set_time_and_range_text_str.wstr_len= MMIAPICOM_Wstrlen(set_time_and_range_tmp);

					GUISTR_DrawTextToLCDInRect(
						(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
						&set_range_txt_rect,
						&set_range_txt_rect,
						&set_time_and_range_text_str,
						&text_style,
						text_state,
						GUISTR_TEXT_DIR_AUTO
						);
				}
			#if MATCH_COUNT_USE_TWO_PAGE != 0
				else
			#endif
				{
					SCI_MEMSET(set_time_and_range_tmp,0,200);
					GUI_GBToWstr(set_time_and_range_tmp,"运算符号:(可多选)",strlen("运算符号:(可多选)"));
					set_time_and_range_text_str.wstr_ptr=set_time_and_range_tmp;
					set_time_and_range_text_str.wstr_len= MMIAPICOM_Wstrlen(set_time_and_range_tmp);

					GUISTR_DrawTextToLCDInRect(
						(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
						&set_symbol_txt_rect,
						&set_symbol_txt_rect,
						&set_time_and_range_text_str,
						&text_style,
						text_state,
						GUISTR_TEXT_DIR_AUTO
						);

					text_style.align = ALIGN_HVMIDDLE;
					SCI_MEMSET(set_time_and_range_tmp,0,200);
					text_state = GUISTR_STATE_ALIGN;
					GUI_GBToWstr(set_time_and_range_tmp,"加法",strlen("加法"));
					set_time_and_range_text_str.wstr_ptr=set_time_and_range_tmp;
					set_time_and_range_text_str.wstr_len= MMIAPICOM_Wstrlen(set_time_and_range_tmp);
					LCD_DrawRect(&lcd_dev_info, set_add_but_rect, MMI_BLACK_COLOR);
					GUISTR_DrawTextToLCDInRect(
						(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
						&set_add_txt_rect,
						&set_add_txt_rect,
						&set_time_and_range_text_str,
						&text_style,
						text_state,
						GUISTR_TEXT_DIR_AUTO
						);

					GUI_GBToWstr(set_time_and_range_tmp,"减法",strlen("减法"));
					set_time_and_range_text_str.wstr_ptr=set_time_and_range_tmp;
					set_time_and_range_text_str.wstr_len= MMIAPICOM_Wstrlen(set_time_and_range_tmp);
					LCD_DrawRect(&lcd_dev_info, set_minus_but_rect, MMI_BLACK_COLOR);
					GUISTR_DrawTextToLCDInRect(
						(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
						&set_minus_txt_rect,
						&set_minus_txt_rect,
						&set_time_and_range_text_str,
						&text_style,
						text_state,
						GUISTR_TEXT_DIR_AUTO
						);
				#if OPEN_CHENGFA_CHUFA
					GUI_GBToWstr(set_time_and_range_tmp,"乘法",strlen("乘法"));
					set_time_and_range_text_str.wstr_ptr=set_time_and_range_tmp;
					set_time_and_range_text_str.wstr_len= MMIAPICOM_Wstrlen(set_time_and_range_tmp);
					LCD_DrawRect(&lcd_dev_info, set_multi_but_rect, MMI_BLACK_COLOR);
					GUISTR_DrawTextToLCDInRect(
						(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
						&set_multi_txt_rect,
						&set_multi_txt_rect,
						&set_time_and_range_text_str,
						&text_style,
						text_state,
						GUISTR_TEXT_DIR_AUTO
						);

					GUI_GBToWstr(set_time_and_range_tmp,"除法",strlen("除法"));
					set_time_and_range_text_str.wstr_ptr=set_time_and_range_tmp;
					set_time_and_range_text_str.wstr_len= MMIAPICOM_Wstrlen(set_time_and_range_tmp);
					LCD_DrawRect(&lcd_dev_info, set_division_but_rect, MMI_BLACK_COLOR);
					GUISTR_DrawTextToLCDInRect(
						(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
						&set_division_txt_rect,
						&set_division_txt_rect,
						&set_time_and_range_text_str,
						&text_style,
						text_state,
						GUISTR_TEXT_DIR_AUTO
						);	
				#endif
					if(choose_add_symbol){
						GUI_FillRect(&lcd_dev_info, set_add_but_rect, MMI_BLACK_COLOR);
					}
					if(choose_minus_symbol){
						GUI_FillRect(&lcd_dev_info, set_minus_but_rect, MMI_BLACK_COLOR);
					}
				#if OPEN_CHENGFA_CHUFA
					if(choose_multi_symbol){
						GUI_FillRect(&lcd_dev_info, set_multi_but_rect, MMI_BLACK_COLOR);
					}

					if(choose_division_symbol){
						GUI_FillRect(&lcd_dev_info, set_division_but_rect, MMI_BLACK_COLOR);
					}
				#endif
				}
			}
			break;
		case MSG_CTL_PENOK:
			{
			}
			break;
		case MSG_TP_PRESS_UP:
			{
				GUI_POINT_T point = {0};
				point.x = MMK_GET_TP_X(param);
				point.y = MMK_GET_TP_Y(param);
				if(GUI_PointIsInRect(point, math_count_action_rect))
				{
					math_learn_count = 5;
					MMIZMT_CreateCountingWin();
				}
			#if MATCH_COUNT_USE_TWO_PAGE != 0      
				else if(GUI_PointIsInRect(point, math_count_page_rect))
				{
					math_count_page_index++;
					math_count_page_index %= 2;
					MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
				}              
				if(math_count_page_index > 0)
			#endif
				{
					if(GUI_PointIsInRect(point, set_add_but_rect))
					{
						if(choose_add_symbol){
							if(choose_minus_symbol||choose_multi_symbol||choose_division_symbol){
								choose_add_symbol = 0;
								MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
							}
						}else{
							choose_add_symbol = 1;
							MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
						}
					}
					else if(GUI_PointIsInRect(point, set_minus_but_rect))
					{
						if(choose_minus_symbol){
							if(choose_add_symbol||choose_multi_symbol||choose_division_symbol){
								choose_minus_symbol = 0;
								MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
							}
						}else{
							choose_minus_symbol = 1;
							MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
						}
					}
				#if OPEN_CHENGFA_CHUFA
					else if(GUI_PointIsInRect(point, set_multi_but_rect))
					{
						if(choose_multi_symbol){
							if(choose_add_symbol||choose_minus_symbol||choose_division_symbol){
								choose_multi_symbol = 0;
								MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
							}
						}else{
							choose_multi_symbol = 1;
							MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
						}
					}
					else if(GUI_PointIsInRect(point, set_division_but_rect))
					{
						if(choose_division_symbol){
							if(choose_add_symbol||choose_minus_symbol||choose_multi_symbol){
								choose_division_symbol = 0;
								MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
							}
						}else{
							choose_division_symbol = 1;
							MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
						}
					}
				#endif
				}
			}
			break;
		case MSG_KEYDOWN_RED:
			break;
		case MSG_KEYUP_RED:
		case MSG_APP_CANCEL:
			MMK_CloseWin(win_id);
			break;
		case MSG_TP_PRESS_DOWN:
			{
				main_tp_down_x = MMK_GET_TP_X(param);
				main_tp_down_y = MMK_GET_TP_Y(param);
			}
			break;
		case MSG_CLOSE_WINDOW:
			break;
		default:
			recode = MMI_RESULT_FALSE;
			break;
	}
	return recode;
}

WINDOW_TABLE(MMI_MATH_COUNT_WIN_TAB) = 
{
	WIN_ID(MATH_COUNT_MAIN_WIN_ID),
	WIN_FUNC((uint32)HandleMathCountWinMsg),
	CREATE_BUTTON_CTRL(MATH_COUNT_ADD_IMG, MMI_MATH_COUNT_SET_TIME_ADD_BTN),
	CREATE_BUTTON_CTRL(MATH_COUNT_REDUCE_IMG, MMI_MATH_COUNT_SET_TIME_REDUCE_BTN),
	CREATE_BUTTON_CTRL(MATH_COUNT_ADD_IMG, MMI_MATH_COUNT_SET_RANGE_ADD_BTN),
	CREATE_BUTTON_CTRL(MATH_COUNT_REDUCE_IMG, MMI_MATH_COUNT_SET_RANGE_REDUCE_BTN),
	WIN_HIDE_STATUS,
	END_WIN
};

WINDOW_TABLE(MMI_MATH_COUNT_COUNTING_WIN_TAB) = 
{
	WIN_ID(MATH_COUNT_COUNTING_WIN_ID),
	WIN_FUNC((uint32)HandleCountingWinMsg),
	WIN_HIDE_STATUS,
	END_WIN
};

WINDOW_TABLE(MMI_MATH_COUNT_SCORE_WIN_TAB) = 
{
	WIN_ID(MATH_COUNT_SCORE_WIN_ID),
	WIN_FUNC((uint32)HandleScoreWinMsg),
	WIN_HIDE_STATUS,
	END_WIN
};


PUBLIC void MMIZMT_CreateMathCountWin(void)
{
	MMI_WIN_ID_T win_id = MATH_COUNT_MAIN_WIN_ID;
	MMI_HANDLE_T win_handle = 0;

	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}
	if(GetCanntClick(1))
	{
		return;
	}
	win_handle = MMK_CreateWin((uint32*)MMI_MATH_COUNT_WIN_TAB, PNULL);
}

LOCAL void MMIZMT_CreateCountingWin(void)
{
	MMI_WIN_ID_T win_id = MATH_COUNT_COUNTING_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = {0, 30, 239, 359};

	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}
	if(GetCanntClick(1))
	{
		return;
	}
	win_handle = MMK_CreateWin((uint32*)MMI_MATH_COUNT_COUNTING_WIN_TAB, PNULL);
	MMK_SetWinRect(win_handle, &rect);
}

LOCAL void MMIZMT_CreateScoreWin(void)
{
	MMI_WIN_ID_T win_id = MATH_COUNT_SCORE_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = {0, 30, 239, 359};

	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}
	if(GetCanntClick(1))
	{
		return;
	}
	win_handle = MMK_CreateWin((uint32*)MMI_MATH_COUNT_SCORE_WIN_TAB, PNULL);
	MMK_SetWinRect(win_handle, &rect);
}
