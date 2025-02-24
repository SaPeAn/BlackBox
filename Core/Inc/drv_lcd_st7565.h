/*
 * drv_lcd_st7565.h
 *
 *  Created on: 24 февр. 2025 г.
 *      Author: SaPA
 */

#ifndef INC_DRV_LCD_ST7565_H_
#define INC_DRV_LCD_ST7565_H_

#include <stdint.h>

void lcd_init(void);
void lcd_sendcommands(uint8_t*, uint8_t);
void lcd_bufupload(void);
void buf_writesmb8x5(const uint8_t, uint8_t, uint8_t);
uint8_t buf_writestr8x5(uint8_t*, uint8_t, uint8_t);
void LCDbuf_erase(void);

#endif /* INC_DRV_LCD_ST7565_H_ */
