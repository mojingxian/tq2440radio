#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>

/*
 * Panel demo for 240x320
 * info panel: (0,  0) - (240, 25)
 * main panel: (0, 25) - (240, 320)
 */
void panel_init(void)
{
    rtgui_rect_t rect;

    /* register dock panel */
//    rect.x1 = 0;
//    rect.y1 = 0;
//    rect.x2 = 320;
//    rect.y2 = 0;
//    rtgui_panel_register("info", &rect);

    /* register main panel */
    rect.x1 = 20;
    rect.y1 = 20;
    rect.x2 = 300;
    rect.y2 = 220;
    rtgui_panel_register("main", &rect);
    rtgui_panel_set_default_focused("main");
}
