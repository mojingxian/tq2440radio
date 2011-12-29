/*
 * COPYRIGHT (C) 2011 By ShenZhen Artel Technology CO., LTD.
 * All Rights Reserved.
 *
 * ������(320X240)LCD����������ʾģʽΪ24λɫ(For TQ2440)��
 *
 * File  : lcd_w35.c
 * Author: mok.jingxian@gmail.com
 * Date  : 2011-12-28
 */

// Includes
#include "lcd.h"
#include "s3c24x0.h"

// Private typedefs
// Private defines
typedef struct
{
    unsigned char red;
    unsigned char blank;
    unsigned char blue;
    unsigned char green;
}color_bit;

// Private enums

// Private macros
#define MVAL            13
#define MVAL_USED       0         //0=each frame   1=rate by MVAL
#define INVVDEN         1         //0=normal       1=inverted
#define BSWP            0         //Byte swap control
#define HWSWP           0         //Half word swap control
#define PNRMODE         3         //����ΪTFT��
#define BPPMODE         13        //����Ϊ24bppģʽ
//TFT_SIZE
#define LCD_XSIZE_TFT   320
#define LCD_YSIZE_TFT   240

#define SCR_XSIZE_TFT   320
#define SCR_YSIZE_TFT   240

//Timing parameter for 3.5' LCD
#define VBPD            12        //��ֱͬ���źŵĺ��
#define VFPD            4         //��ֱͬ���źŵ�ǰ��
#define VSPW            5         //��ֱͬ���źŵ�����
#define HBPD            22        //ˮƽͬ���źŵĺ��
#define HFPD            33        //ˮƽͬ���źŵ�ǰ��
#define HSPW            44        //ˮƽͬ���źŵ�����
#define CLKVAL_TFT      6

#define HOZVAL_TFT      (LCD_XSIZE_TFT-1)
#define LINEVAL_TFT     (LCD_YSIZE_TFT-1)

#define M5D(n)          ((n) & 0x1FFFFF)    // To get lower 21bits

#define LCD_BLANK       12
#define C_UP            (LCD_XSIZE_TFT - LCD_BLANK * 2)
#define C_RIGHT         (LCD_XSIZE_TFT - LCD_BLANK * 2)
#define V_BLACK         ((LCD_YSIZE_TFT - LCD_BLANK * 4) / 6)

#define GET_COLOR_R(x) (unsigned char)((x) >> 16)
#define GET_COLOR_G(x) (unsigned char)((x) >> 8)
#define GET_COLOR_B(x) (unsigned char)((x) >> 0)

// Private variables
// Global  variables
volatile color_bit LCD_BUFFER[SCR_YSIZE_TFT][SCR_XSIZE_TFT];

// Private functions

void lcd_init(void)
{
    GPCUP  = 0x00000000; //ʹ����������
    GPCCON = 0xAAAA02A9; //ʹ�����GPC���ŵ�LCD���ƹ���
    GPDUP  = 0x00000000; //ʹ����������
    GPDCON = 0xAAAAAAAA; //ʹ�� VD[15:8]

    LCDCON1 = (CLKVAL_TFT << 8) | (MVAL_USED << 7) | (PNRMODE << 5) | (BPPMODE << 1) | 0;
    LCDCON2 = (VBPD << 24) | (LINEVAL_TFT << 14) | (VFPD << 6) | (VSPW);
    LCDCON3 = (HBPD << 19) | (HOZVAL_TFT << 8) | (HFPD);
    LCDCON4 = (MVAL << 8) | (HSPW);
    LCDCON5 = (0 << 12) | (1 << 11) | (0 << 10) | (1 << 9) | (1 << 8) | (0 << 7) | (0 << 6) | (1 << 3) | (BSWP << 1) | (HWSWP);

    LCDSADDR1 = (((unsigned int) LCD_BUFFER >> 22) << 21) | M5D((unsigned int)LCD_BUFFER >> 1);
    LCDSADDR2 = M5D(((unsigned int)LCD_BUFFER + (SCR_XSIZE_TFT * LCD_YSIZE_TFT * 4)) >> 1 );
    LCDSADDR3 = LCD_XSIZE_TFT * 32 / 16;//(((SCR_XSIZE_TFT - LCD_XSIZE_TFT) / 1) << 11) | (LCD_XSIZE_TFT / 1);
    LCDINTMSK|= (3);   // MASK LCD Sub Interrupt
    LPCSEL   &= (~7);  // Disable LPC3480
    TPAL      = 0;     // Disable Temp Palette
}

/*!
 * @details LCD��Ƶ�Ϳ����ź��������ֹͣ��1������Ƶ�����
 *
 * @param   onoff 1������Ƶ�����0ֹͣ��Ƶ�����
 */
void lcd_envid_ctrl(int onoff)
{
    if (onoff == 1)
    {
        LCDCON1 |= 0x00001; // ENVID ON
    }
    else
    {
        LCDCON1 &= 0x3fffe; // ENVID Off
    }
}

/*!
 * @details TFT LCD ��Դ��������ʹ��.
 *
 * @param   invpwren
 * @param   pwren
 */
void lcd_power_enable(int invpwren, int pwren)
{
    //GPG4 is setted as LCD_PWREN
    GPGUP  = (GPGUP & (~(1 << 4))) | (1 << 4); // Pull-up disable
    GPGCON = (GPGCON & (~(3 << 8))) | (3 << 8); //GPG4=LCD_PWREN
    GPGDAT = GPGDAT | (1 << 4);

    //Enable LCD POWER ENABLE Function
    LCDCON5 = (LCDCON5 & (~(1 << 3))) | (pwren << 3);    // PWREN
    LCDCON5 = (LCDCON5 & (~(1 << 5))) | (invpwren << 5); // INVPWREN
}

/*!
 * @details TFT LCD�������ص���ʾ�������.
 *
 * @param   x x����
 * @param   y y����
 * @param   c ��ɫ
 */
void put_pixel(unsigned int x, unsigned int y, unsigned int c)
{
    unsigned char r, g, b;

    r = GET_COLOR_R(c) ;
    g = GET_COLOR_G(c) ;
    b = GET_COLOR_B(c) ;

    if ((x < SCR_XSIZE_TFT) && (y < SCR_YSIZE_TFT))
    {
        LCD_BUFFER[y][x].red   = r;
        LCD_BUFFER[y][x].green = g;
        LCD_BUFFER[y][x].blue  = b;
    }
}

/*!
 * @details TFT LCDȫ������ض���ɫ��Ԫ������.
 *
 * @param   c ��ɫ
 */
void lcd_clear_scr(unsigned int c)
{
    unsigned int  x, y;
    unsigned char r, g, b;

    r = GET_COLOR_R(c) ;
    g = GET_COLOR_G(c) ;
    b = GET_COLOR_B(c) ;

    for (y = 0; y < SCR_YSIZE_TFT; y++)
    {
        for (x = 0; x < SCR_XSIZE_TFT; x++)
        {
            LCD_BUFFER[y][x].red   = r;
            LCD_BUFFER[y][x].green = g;
            LCD_BUFFER[y][x].blue  = b;
        }
    }
}

/*!
 * @details ��һ��ֱ��.
 *
 * @param   x1 ��ʼλ��x����
 * @param   y1 ��ʼλ��y����
 * @param   x2 ����λ��x����
 * @param   y2 ����λ��y����
 * @param   c  ֱ����ɫ
 */
void lcd_draw_line(int x1,int y1,int x2,int y2,int c)
{
    int dx, dy, e;
    dx = x2 - x1;
    dy = y2 - y1;

    if (dx >= 0)
    {
        if (dy >= 0) // dy>=0
        {
            if (dx >= dy) // 1/8 octant
            {
                e = dy - dx / 2;
                while (x1 <= x2)
                {
                    put_pixel(x1, y1, c);
                    if (e > 0)
                    {
                        y1 += 1;
                        e -= dx;
                    }
                    x1 += 1;
                    e += dy;
                }
            }
            else // 2/8 octant
            {
                e = dx - dy / 2;
                while (y1 <= y2)
                {
                    put_pixel(x1, y1, c);
                    if (e > 0)
                    {
                        x1 += 1;
                        e -= dy;
                    }
                    y1 += 1;
                    e += dx;
                }
            }
        }
        else // dy<0
        {
            dy = -dy; // dy=abs(dy)

            if (dx >= dy) // 8/8 octant
            {
                e = dy - dx / 2;
                while (x1 <= x2)
                {
                    put_pixel(x1, y1, c);
                    if (e > 0)
                    {
                        y1 -= 1;
                        e -= dx;
                    }
                    x1 += 1;
                    e += dy;
                }
            }
            else // 7/8 octant
            {
                e = dx - dy / 2;
                while (y1 >= y2)
                {
                    put_pixel(x1, y1, c);
                    if (e > 0)
                    {
                        x1 += 1;
                        e -= dy;
                    }
                    y1 -= 1;
                    e += dx;
                }
            }
        }
    }
    else //dx<0
    {
        dx = -dx; //dx=abs(dx)
        if (dy >= 0) // dy>=0
        {
            if (dx >= dy) // 4/8 octant
            {
                e = dy - dx / 2;
                while (x1 >= x2)
                {
                    put_pixel(x1, y1, c);
                    if (e > 0)
                    {
                        y1 += 1;
                        e -= dx;
                    }
                    x1 -= 1;
                    e += dy;
                }
            }
            else // 3/8 octant
            {
                e = dx - dy / 2;
                while (y1 <= y2)
                {
                    put_pixel(x1, y1, c);
                    if (e > 0)
                    {
                        x1 -= 1;
                        e -= dy;
                    }
                    y1 += 1;
                    e += dx;
                }
            }
        }
        else // dy<0
        {
            dy = -dy; // dy=abs(dy)

            if (dx >= dy) // 5/8 octant
            {
                e = dy - dx / 2;
                while (x1 >= x2)
                {
                    put_pixel(x1, y1, c);
                    if (e > 0)
                    {
                        y1 -= 1;
                        e -= dx;
                    }
                    x1 -= 1;
                    e += dy;
                }
            }
            else // 6/8 octant
            {
                e = dx - dy / 2;
                while (y1 >= y2)
                {
                    put_pixel(x1, y1, c);
                    if (e > 0)
                    {
                        x1 -= 1;
                        e -= dy;
                    }
                    y1 -= 1;
                    e += dx;
                }
            }
        }
    }
}

/*!
 * @details ��LCD��Ļ�ϻ�һ������.
 *
 * @param   x1 ��ʼλ��x����
 * @param   y1 ��ʼλ��y����
 * @param   x2 ����λ��x����
 * @param   y2 ����λ��y����
 * @param   c  ֱ����ɫ
 */
void lcd_draw_rectangle(int x1,int y1,int x2,int y2,int c)
{
    lcd_draw_line(x1, y1, x2, y1, c);
    lcd_draw_line(x2, y1, x2, y2, c);
    lcd_draw_line(x1, y2, x2, y2, c);
    lcd_draw_line(x1, y1, x1, y2, c);
}

/*!
 * @details ��LCD��Ļ������ɫ���һ������.
 *
 * @param   x1 ��ʼλ��x����
 * @param   y1 ��ʼλ��y����
 * @param   x2 ����λ��x����
 * @param   y2 ����λ��y����
 * @param   c  ֱ����ɫ
 */
void lcd_fill_rectangle(int x1, int y1, int x2, int y2, int c)
{
    int i;

    for (i = y1; i <= y2; i++)
    {
        lcd_draw_line(x1, i, x2, i, c);
    }
}

/*!
 * @details ��LCD��Ļ��ָ������㻭һ��ָ����С��ͼƬ.
 *
 * @param   x0   ��ʼλ��x����
 * @param   y0   ��ʼλ��y����
 * @param   h    ͼ��߶�
 * @param   l    ͼ����
 * @param   *bmp bmpͼ������ָ��
 */
void lcd_paint_bmp(int x0, int y0, int h, int l, unsigned char *bmp)
{
    int x, y, p = 0;

    for (y = y0; y < l; y++)
    {
        for (x = x0; x < h; x++)
        {
            if ((x < SCR_XSIZE_TFT) && (y < SCR_YSIZE_TFT))
            {
                LCD_BUFFER[y][x].red   = bmp[p + 2];
                LCD_BUFFER[y][x].green = bmp[p + 1];
                LCD_BUFFER[y][x].blue  = bmp[p + 0];
            }

            p = p + 3;
        }
    }
}

void lcd_tft_init(void)
{
    lcd_init();
    lcd_power_enable(0, 1);
    lcd_envid_ctrl(1);     //turn on vedio

    lcd_clear_scr((0x00 << 11) | (0x00 << 5) | (0x00));
    lcd_fill_rectangle(LCD_BLANK, LCD_BLANK, (LCD_XSIZE_TFT - LCD_BLANK ), ( LCD_YSIZE_TFT - LCD_BLANK ),0x0000); //fill a Rectangle with some color

    lcd_fill_rectangle((LCD_BLANK * 2), (LCD_BLANK*2 + V_BLACK*0), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*1),0x001f); //fill a Rectangle with some color
    lcd_fill_rectangle((LCD_BLANK * 2), (LCD_BLANK*2 + V_BLACK*1), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*2),0x07e0); //fill a Rectangle with some color
    lcd_fill_rectangle((LCD_BLANK * 2), (LCD_BLANK*2 + V_BLACK*2), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*3),0xf800); //fill a Rectangle with some color
    lcd_fill_rectangle((LCD_BLANK * 2), (LCD_BLANK*2 + V_BLACK*3), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*4),0xffe0); //fill a Rectangle with some color
    lcd_fill_rectangle((LCD_BLANK * 2), (LCD_BLANK*2 + V_BLACK*4), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*5),0xf81f); //fill a Rectangle with some color
    lcd_fill_rectangle((LCD_BLANK * 2), (LCD_BLANK*2 + V_BLACK*5), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*6),0x07ff); //fill a Rectangle with some color

    lcd_draw_line(LCD_BLANK, LCD_BLANK, (LCD_XSIZE_TFT - LCD_BLANK), (LCD_YSIZE_TFT-LCD_BLANK), 0x0000 );
    lcd_draw_line(LCD_BLANK, (LCD_YSIZE_TFT - LCD_BLANK), (LCD_XSIZE_TFT-LCD_BLANK), LCD_BLANK, 0x0000 );
    lcd_draw_line((LCD_XSIZE_TFT / 2), (LCD_BLANK*2 + V_BLACK*0), (LCD_XSIZE_TFT/2), (LCD_BLANK*2 + V_BLACK*6), 0x0000);
}
