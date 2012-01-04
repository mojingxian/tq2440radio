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
    /* ����Ŀǰֻ�԰����¼�����Ȥ�������ǰworkbench����ģʽ��ʾ״̬��������  */
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

    /* ������ǻ����¼���ʹ��viewԭ�����¼����������� */
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

    /* ������ͼ������ */
    view = rtgui_view_create("sample");
    if (view == RT_NULL) return RT_NULL;

    /* ��ӵ���workbench�� */
    rtgui_workbench_add_view(workbench, view);

    /* �����ͼ��λ����Ϣ(�ڼ��뵽workbench��ʱ��workbench���Զ�������ͼ�Ĵ�С) */
    rtgui_widget_get_rect(RTGUI_WIDGET(view), &rect);
    rtgui_widget_rect_to_device(RTGUI_WIDGET(view), &rect);
    rect.x1 += 5;
    rect.y1 += 5;
    rect.x2 -= 5;
    rect.y2 = rect.y1 + 20;

    /* ���������õı�ǩ */
    label = rtgui_label_create("��ǰʱ��");
    /* ���ñ�ǩλ����Ϣ */
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    /* ��ӱ�ǩ����ͼ�� */
    rtgui_container_add_child(RTGUI_CONTAINER(view), RTGUI_WIDGET(label));

    /* ���ش�������ͼ */
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

    /* ����GUIӦ����Ҫ����Ϣ���� */
    mq = rt_mq_create("workbench", 256, 32, RT_IPC_FLAG_FIFO);

    /* ע�ᵱǰ�߳�ΪGUI�߳� */
    rtgui_thread_register(rt_thread_self(), mq);

    /* ����һ������̨ */
    workbench = rtgui_workbench_create("main", "workbench");
    if (workbench == RT_NULL) return;

    rtgui_widget_set_event_handler(RTGUI_WIDGET(workbench), myworkbench_event_handler);

    view = sample_view(workbench);

    /* ��ʾ��ͼ */
    if (view != RT_NULL) rtgui_view_show(view, RT_FALSE);

    /* ִ�й���̨�¼�ѭ�� */
    rtgui_workbench_event_loop(workbench);

    /* ȥע��GUI�߳� */
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

    /* GUIϵͳ��ʼ�� */
    rtgui_system_server_init();

    //����ʼ��
    rect.x1 = 0;
    rect.y1 = 0;
    rect.x2 = 320;
    rect.y2 = 240;
    rtgui_panel_register("main", &rect);
    rtgui_panel_set_default_focused("main");

    tid = rt_thread_create("wb", myworkbench_entry, RT_NULL, 2048 * 2, 25, 10);

    if (tid != RT_NULL) rt_thread_startup(tid);
}
