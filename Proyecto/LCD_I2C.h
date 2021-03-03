#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include "stdint.h"
#include "fsl_i2c.h"

//PINOUT ENTRE PCF8574 Y LCD

//  PCF8574      LCD
//  ========     ======
//     P0        RS
//     P1        RW
//     P2        Enable
//     P3        Led Backlight
//     P4        D4
//     P5        D5
//     P6        D6
//     P7        D7

//---------------------------------------------------------------//
// Define
//---------------------------------------------------------------//

#define k_LCDcom 0
#define k_LCDdat 1
//---------------------------------------------------------------//
// Variables
//---------------------------------------------------------------//
typedef struct {
	uint8_t nb1,nb2;
}byte;

//---------------------------------------------------------------//
// Prototipos
//---------------------------------------------------------------//
void lcd_init (uint8_t addr, uint8_t row, uint8_t col);
void line_lcd(unsigned char *p,unsigned char r);
void send (uint8_t dat);
void send_lcd (uint8_t wb,uint8_t rs);
void send_i2c (uint8_t dat);
byte nibble (unsigned char wb);
void mode4bit (void);
void FuntionSet(unsigned char fs);
void DisplayOnOff(unsigned char dof);
void EntryModeSet(unsigned char ems);
void ClearDisplay(void);
void ReturnHome(void);
void SetDDRAMAddress(unsigned char sda);

#endif /* LCD_I2C_H_ */
