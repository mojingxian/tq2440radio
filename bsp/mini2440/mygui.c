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

static rt_bool_t myworkbench_event_handler(struct rtgui_widget* widget, struct rtgui_event* event)
{
    /* 我们目前只对按键事件感兴趣。如果当前workbench处于模式显示状态，忽略它  */
    if ((event->type == RTGUI_EVENT_KBD) && !RTGUI_WORKBENCH_IS_MODAL_MODE(RTGUI_WORKBENCH(widget)))
    {
        struct rtgui_event_kbd* ekbd = (struct rtgui_event_kbd*)event;

        if (ekbd->type == RTGUI_KEYDOWN)
        {
            if (ekbd->key == RTGUIK_RIGHT)
            {
                //demo_view_next(RT_NULL, RT_NULL);
                return RT_TRUE;
            }
            else if (ekbd->key == RTGUIK_LEFT)
            {
                //demo_view_prev(RT_NULL, RT_NULL);
                return RT_TRUE;
            }
        }
    }

    /* 如果不是绘制事件，使用view原来的事件处理函数处理 */
    return rtgui_workbench_event_handler(widget, event);
}

/*!
 * @details Describe function here.
 *
 * @param   none
 *
 * @return  none
 * @retval  none
 */
rtgui_view_t *sample_view(rtgui_workbench_t *workbench)
{
    struct rtgui_view* view;
    struct rtgui_rect rect;
    struct rtgui_label *label;

    /* 设置视图的名称 */
    view = rtgui_view_create("sample");
    if (view == RT_NULL) return RT_NULL;

    /* 添加到父workbench中 */
    rtgui_workbench_add_view(workbench, view);

    /* 获得视图的位置信息(在加入到workbench中时，workbench会自动调整视图的大小) */
    rtgui_widget_get_rect(RTGUI_WIDGET(view), &rect);
    rtgui_widget_rect_to_device(RTGUI_WIDGET(view), &rect);
    rect.x1 += 5;
    rect.y1 += 5;
    rect.x2 -= 5;
    rect.y2 = rect.y1 + 20;

    /* 创建标题用的标签 */
    label = rtgui_label_create("当前时间");
    /* 设置标签位置信息 */
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    /* 添加标签到视图中 */
    rtgui_container_add_child(RTGUI_CONTAINER(view), RTGUI_WIDGET(label));

    /* 返回创建的视图 */
    return view;
}

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
    rt_mq_t mq;
    rtgui_view_t *view;
    struct rtgui_workbench* workbench;

    /* 创建GUI应用需要的消息队列 */
    mq = rt_mq_create("workbench", 256, 32, RT_IPC_FLAG_FIFO);

    /* 注册当前线程为GUI线程 */
    rtgui_thread_register(rt_thread_self(), mq);

    /* 创建一个工作台 */
    workbench = rtgui_workbench_create("main", "workbench");
    if (workbench == RT_NULL) return;

    rtgui_widget_set_event_handler(RTGUI_WIDGET(workbench), myworkbench_event_handler);

    view = sample_view(workbench);

    /* 显示视图 */
    if (view != RT_NULL) rtgui_view_show(view, RT_FALSE);

    /* 执行工作台事件循环 */
    rtgui_workbench_event_loop(workbench);

    /* 去注册GUI线程 */
    rtgui_thread_deregister(rt_thread_self());
    rt_mq_delete(mq);
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
