#include "lvgl_app.h"

/**
 * ----------------------------------------------------------------------
 * @brief       ���⴦���ֵ��޸�
 * @param  str  
 * ----------------------------------------------------------------------
**/
void lvapp_con1_edit(char *str){
    lv_label_set_text(guider_ui.screen_tabview_1_tab_1_label, str);
}

/**
 * ----------------------------------------------------------------------
 * @brief       ��⴦���ֵ��޸�
 * @param  str  
 * ----------------------------------------------------------------------
**/
void lvapp_con2_edit(char *str){
    lv_label_set_text(guider_ui.screen_tabview_1_tab_2_label, str);
}