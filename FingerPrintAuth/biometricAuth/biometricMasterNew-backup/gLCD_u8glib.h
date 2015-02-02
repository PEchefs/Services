
#include "Arduino.h"


#define DEBUG 0

# define MAX_NUM_DISPLAY_LINES 4

U8GLIB_KS0108_128 u8g(47, 45, 43, 41, 39, 37, 35, 33, 49, 31, 29, 53, 51); 		// 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, cs1=14, cs2=15,di=17,rw=16


uint8_t menu_redraw_required = 0;

/*

void drawMenu(menu_st MenuToDisplay) 
{
uint8_t i, h;
u8g_uint_t w, d;

//  u8g.setFont(u8g_font_7x13B);
u8g.setFont(u8g_font_7x14);
u8g.setFontRefHeightText();
u8g.setFontPosTop();
h = u8g.getFontAscent()-u8g.getFontDescent();

w = u8g.getWidth();

for( i = 0; i < MAX_NUM_OF_MENU_LIST_ITEMS; i++ ) 
{
//    d = (w-u8g.getStrWidth(Main_MenuList[i]))/2;
	d=0;
	u8g.setDefaultForegroundColor();
	if ( i == menu_current ) {
	u8g.drawBox(0, i*h+1, w, h);
//    u8g.drawBox(0, i*h+1, w, h+2);
	u8g.setDefaultBackgroundColor();
	}
//   u8g.setFontPosBaseline();
	u8g.drawStr(d, i*h+1, MenuToDisplay.List[i]);
}  
}
*/




