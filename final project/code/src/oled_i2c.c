#include "oled_i2c.h"
#include "stdutils.h"
#include "i2c.h"
#include "table.h"

/**************************************************************************************************
                                void OLED_Init()
 ***************************************************************************************************
 * I/P Arguments:  none
 * Return value : none

 * description  :This function is used to initialize the OLED in the normal mode.
                After initializing the OLED, It clears the OLED and sets the cursor to first line first position. .

 **************************************************************************************************/

void OLED_Init(void)
{ 
    oledSendCommand(0xa8);  //Set MUX Ratio
    oledSendCommand(0x3f);  
    oledSendCommand(0xd3);  //Set Display Offset
    oledSendCommand(0x00);  
    oledSendCommand(0x40);  //Set Display Start Line
    oledSendCommand(0xa1);  //Set Segment re-map A0h/A1h
    oledSendCommand(0xc8);  //Set COM OUTput
    oledSendCommand(0xda); 
    oledSendCommand(0x12);
    oledSendCommand(0x81);
    oledSendCommand(0x7f);
    oledSendCommand(0xa4);
    oledSendCommand(0xa6);
    oledSendCommand(0xd5);
    oledSendCommand(0x80);
    oledSendCommand(0x8d);
    oledSendCommand(0x14);
    oledSendCommand(0xaf);

    OLED_Clear();  // Clear the complete LCD during init 
}

/***************************************************************************************************
                       void OLED_DisplayChar( char ch)
 ****************************************************************************************************
 * I/P Arguments: ASCII value of the char to be displayed.
 * Return value    : none

 * description  : This function sends a character to be displayed on LCD.
                  Any valid ascii value can be passed to display respective character

 ****************************************************************************************************/
void OLED_DisplayChar(uint8_t ch)
{
    uint8_t i=0;
    int index;

    if(ch!='\n') {  /* TODO */ 
        index = (ch - 0x20);
        index = index * 5; // As the lookup table starts from Space(0x20)

        for(i = 0; i < 5; i ++)
            oledSendData(OledFontTable[index + i]); /* Get the data to be displayed for LookUptable*/

        oledSendData(0x00); /* Display the data and keep track of cursor */
    }
}


/***************************************************************************************************
                       void OLED_DisplayString(char *ptr_stringPointer_u8)
 ****************************************************************************************************
 * I/P Arguments: String(Address of the string) to be displayed.
 * Return value    : none

 * description  :
               This function is used to display the ASCII string on the lcd.
                 1.The ptr_stringPointer_u8 points to the first char of the string
                    and traverses till the end(NULL CHAR)and displays a char each time.

 ****************************************************************************************************/
#if(Enable_OLED_DisplayString==1)
void OLED_DisplayString(uint8_t *ptr)
{
    while(*ptr)
        OLED_DisplayChar(*ptr++);
}
#endif

/***************************************************************************************************
                         void OLED_Clear()
 ****************************************************************************************************
 * I/P Arguments: none.
 * Return value    : none

 * description  :This function clears the LCD and moves the cursor to beginning of first line
 ****************************************************************************************************/
void OLED_Clear(void)
{	
    uint8_t oled_clean_col , oled_clean_page;
	for(oled_clean_page = 0 ; oled_clean_page < 8 ; oled_clean_page ++) {
        OLED_SetCursor(oled_clean_page,0);
		for(oled_clean_col= 0 ; oled_clean_col < 128 ; oled_clean_col ++) {
            oledSendData(0);
		}
	}
}

/***************************************************************************************************
                void OLED_SetCursor(char v_lineNumber_u8,char v_charNumber_u8)
 ****************************************************************************************************
 * I/P Arguments: char row,char col
                 row -> line number(line1=1, line2=2),
                        For 2line LCD the I/P argument should be either 1 or 2.
                 col -> char number.
                        For 16-char LCD the I/P argument should be between 0-15.
 * Return value    : none

 * description  :This function moves the Cursor to specified position

                   Note:If the Input(Line/Char number) are out of range
                        then no action will be taken
 ****************************************************************************************************/
void OLED_SetCursor(uint8_t lineNumber,uint8_t cursorPosition)
{
    cursorPosition = cursorPosition + 2;
    oledSendCommand(0x0f&cursorPosition); // Select SEG lower 4 bits
		oledSendCommand(0x10|(cursorPosition>>4)); // Select SEG higher 4 bits
		oledSendCommand(0xb0|lineNumber); // Select PAGE
}

/********************************************************************************
                Local FUnctions for sending the command/data
 ********************************************************************************/
void oledSendCommand(uint8_t cmd)
{
    Single_WriteI2C(OLED_SlaveAddress, SSD1306_COMMAND, cmd);
}

void oledSendData(uint8_t cmd)
{
    Single_WriteI2C(OLED_SlaveAddress, SSD1306_DATA_CONTINUE, cmd);
}

/*****************************************************************************************************/
