/*
 ******************************************************************************
 * drv_lcd_st7565.c
 *
 *  Created on: 24 февр. 2025 г.
 *      Author: SaPA
 ******************************************************************************
*/

#include <display_data.h>
#include "drv_lcd_st7565.h"
#include "main.h"

#define   DISP_INIT_SEND_COM      HAL_GPIO_WritePin(DISP_RS_GPIO_Port, DISP_RS_Pin, RESET)   // 0-cmd
#define   DISP_INIT_SEND_DAT      HAL_GPIO_WritePin(DISP_RS_GPIO_Port, DISP_RS_Pin, SET)     // 1-data
#define   DISP_RESET_ON           HAL_GPIO_WritePin(DISP_RSE_GPIO_Port, DISP_RSE_Pin, RESET)
#define   DISP_RESET_OFF          HAL_GPIO_WritePin(DISP_RSE_GPIO_Port, DISP_RSE_Pin, SET)

uint8_t dispbuffer[8][128] = {0};

uint8_t str_null[] = "NULL";

extern SPI_HandleTypeDef hspi2;

void lcd_init(void)
{
  DISP_RESET_OFF;
  DISP_INIT_SEND_DAT;
  HAL_Delay(2);
  DISP_RESET_ON;
  HAL_Delay(2);
  DISP_RESET_OFF;
  DISP_INIT_SEND_COM;
  uint8_t init_data_array[13] = {
	(0xA2 | 0),       // LCD Bias Set -- x=0 : 1/9 (default); x=1 :  1/7
	(0xA0 | 0),       // Segment Driver Direction Select 0->131(0);  Segment Driver Direction Select 131->0(1))
	(0xC0 | 8),       // Common Output Mode Select 0->63(0)); Common Output Mode Select 63->0(8)
	(0x20 | 0x6),     // V5 Voltage Regulator Internal Resistor Ratio Set 0:3.0; 1:3.5; 2:4; 3:4.5; 4:5.0(default); 5:5.5; 6:6; 7:6.4;
	(0x28 | 0b111),   // Power Controller Set a=1 :  Booster circuit on; b=1 :  Voltage regulator circuit on; c=1 : Voltage follower circuit on; default: 000, must be 111
	(0x81),           // The Electronic Volume Mode Set (contrast) (default 0010 0000) - first byte (command id)
	(0x20),           // The Electronic Volume Mode Set (contrast) (default 0010 0000) - second byte (value)
	(0x40 | 0),       // Display Start Line Set (0-63)
	(0xB0 | 0),       // Page Address Set (0-7))
	(0x10 | 0),       // Column Address Set - first byte (0-15)
	(0x00 | 0),       // Column Address Set - second byte (0-15)
	(0xAE | 1),       // Display on(1) / Display off (0)
	(0xA6 | 0),       // Display Normal(0) / Display Reverse(1)
  };
  HAL_SPI_Transmit(&hspi2, init_data_array, 13, 20);
  lcd_buferase();
  lcd_bufupload();
  DISP_INIT_SEND_DAT;
}

void lcd_sendcommands(uint8_t* data, uint8_t N)
{
  DISP_INIT_SEND_COM;
  HAL_SPI_Transmit(&hspi2, data, N, 10);
  DISP_INIT_SEND_DAT;
}

void lcd_bufupload(void)
{
  for(uint8_t j = 0; j < 8; j++)
  {
	uint8_t tmp_arr[] = {(0xB0 + j), 0x10, 0x00};
    lcd_sendcommands(tmp_arr, 3);
    HAL_SPI_Transmit(&hspi2, dispbuffer[j], 128, 10);
  }
}


/******************************************/
void lcd_bufwsmb8x5(const uint8_t ch, uint8_t pg, uint8_t cl)
{
	for(uint8_t i = 0; i < 5; i++)
	  {
	    dispbuffer[pg][cl + i] = char_8x5[ch][i];
	  }
	cl = cl+5;
	dispbuffer[pg][cl] = 0x00;
	cl++;
}

uint8_t lcd_bufwstr8x5(uint8_t *str, uint8_t pg, uint8_t cl, uint8_t newline_fl)
{
  if(str == NULL) str = str_null;
  uint8_t i = 0;
  while(str[i])
  {
	lcd_bufwsmb8x5(str[i], pg, cl);
    cl += 6;
    if(cl > 122)
    {
      if(newline_fl)
      {
        pg++;
        cl = 0;
      }
      else return i;
    }
    if(pg > 7) return 0;
    i++;
  }
  return i;
}

void lcd_bufstrerase(uint8_t pg)
{
	for(uint8_t i = 0; i < 128; i++)
		  {
		    dispbuffer[pg][i] = 0;
		  }
}

void lcd_buferase(void)
{
  for(uint8_t j = 0; j < 8; j++)
    for(uint8_t i = 0; i < 128; i++) dispbuffer[j][i] = 0;
}
