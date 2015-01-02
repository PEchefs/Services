
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


void drawMenu(menu_st menuToDisplay) {
	uint8_t i,count, h;
	u8g_uint_t w, d;

	//  u8g.setFont(u8g_font_7x13B);
	u8g.setFont(u8g_font_7x14);
	u8g.setFontRefHeightText();
	u8g.setFontPosTop();
	h = u8g.getFontAscent()-u8g.getFontDescent();

	w = u8g.getWidth();
        
        // Heading for Menu
        unsigned short mid = (w-u8g.getStrWidth(menuToDisplay.heading))/2;
//        u8g.drawStr(mid, 0*h+1, menuToDisplay.heading);
  /*      if (currentMenuIndex == 2)
          return; */
	if (( menuToDisplay.numOfMenuListItems - displayStartIndex ) <= MAX_NUM_DISPLAY_LINES )
	{
		if(currentMenuIndex>=menuToDisplay.numOfMenuListItems)
		{
			currentMenuIndex=0;
			displayStartIndex=0;
		}
		else if(currentMenuIndex<0)
		{
			currentMenuIndex=menuToDisplay.numOfMenuListItems-1;
			displayStartIndex=0;
		}
		if(currentMenuIndex < displayStartIndex)
		{
			displayStartIndex--;
		}


        
		//loop(from displayStartIndex to numOfMenuListItems)                
		for( i = displayStartIndex,count=1; i < menuToDisplay.numOfMenuListItems; i++,count++ ) 
                {
			d=0;
			u8g.setDefaultForegroundColor();
                        // Display Heading
                        u8g.drawStr(mid, 0*h+1, menuToDisplay.heading);
			if ( i == currentMenuIndex ) 
                        {
				u8g.drawBox(0, count*h+1, w, h);
				//    u8g.drawBox(0, i*h+1, w, h+2);
				u8g.setDefaultBackgroundColor();
			}
			//   u8g.setFontPosBaseline();
			u8g.drawStr(d, count*h+1, menuToDisplay.List[i]);
		}
	}
	else
	{
		if(DEBUG)
		{ 
			Serial.print("currentMenuIndex=");Serial.println(currentMenuIndex);
			Serial.print("displayStartIndex=");Serial.println(displayStartIndex);
		}
		if(currentMenuIndex>=displayStartIndex+MAX_NUM_DISPLAY_LINES)
		{
			displayStartIndex++;
		}
		else if(currentMenuIndex<0)
		{
			currentMenuIndex=menuToDisplay.numOfMenuListItems-1;
			displayStartIndex=menuToDisplay.numOfMenuListItems-MAX_NUM_DISPLAY_LINES;
		}
		if(currentMenuIndex < displayStartIndex)
		{
			displayStartIndex--;
		}
		//loop(from displayStartIndex to displayStartIndex+maxNumOfDisplayLines-1)

		for( i = displayStartIndex,count=1; i < displayStartIndex+MAX_NUM_DISPLAY_LINES; i++,count++ ) 
                {
			d=0;
			u8g.setDefaultForegroundColor();
                        //Display Heading
                        u8g.drawStr(mid, 0*h+1, menuToDisplay.heading);
			if ( i == currentMenuIndex ) {
				u8g.drawBox(0, count*h+1, w, h);
				//    u8g.drawBox(0, i*h+1, w, h+2);
				u8g.setDefaultBackgroundColor();
			}
			//   u8g.setFontPosBaseline();
			u8g.drawStr(d, count*h+1, menuToDisplay.List[i]);
		}
	}




	if(DEBUG)
	{ 
		Serial.print("currentMenuIndex=");Serial.println(currentMenuIndex);
		Serial.print("displayStartIndex=");Serial.println(displayStartIndex);
	}
}





void displayMenu()
{
	// put your main code here, to run repeatedly:
	initializeMenus();
	u8g.firstPage();  
	do {

		drawMenu(Menu[currentMenu]);
	} while( u8g.nextPage() );

}

void scrolldown()
{
	currentMenuIndex++; 
}

void scrollup()
{
	currentMenuIndex--; 
}

