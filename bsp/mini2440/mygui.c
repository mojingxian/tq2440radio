/*
 * COPYRIGHT (C) 2011 By *** Technology CO., LTD.
 * All Rights Reserved.
 *
 * File  : mygui.c
 * Author: AreMok
 * Date  : 2012-1-4
 */
#define MYGUI_C_

// Includes
#include "mygui.h"

#include <rtgui/rtgui.h>
#include <rtgui/rtgui_system.h>

#include <rtgui/widgets/view.h>
#include <rtgui/widgets/workbench.h>

// Private typedefs
// Private defines
// Private enums
// Private macros
// Private variables
// Global  variables
// Private functions

/*!
 * @details Describe function here.
 *
 * @param   none
 *
 * @return  none
 * @retval  none
 */
static void myworkbench_entry(void *p_arg)
{

}

/*!
 * @details Describe function here.
 *
 * @param   none
 *
 * @return  none
 * @retval  none
 */
void mygui_startup(void)
{
    rtgui_rect_t rect;
    rt_thread_t tid;

    /* GUI系统初始化 */
    rtgui_system_server_init();

    //面板初始化
    rect.x1 = 0;
    rect.y1 = 0;
    rect.x2 = 320;
    rect.y2 = 240;
    rtgui_panel_register("main", &rect);
    rtgui_panel_set_default_focused("main");

    tid = rt_thread_create("wb", myworkbench_entry, RT_NULL, 2048 * 2, 25, 10);

    if (tid != RT_NULL) rt_thread_startup(tid);
}
