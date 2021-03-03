#include "lcd_i2c.h"

//---------------------------------------------------------------//
// Variables
//---------------------------------------------------------------//
uint8_t LCD_ADDR;
uint8_t LCD_ROW;
uint8_t LCD_COL;
/*-------------------------------------------------------------------*/
void lcd_init (uint8_t addr, uint8_t row, uint8_t col){
	LCD_ADDR = addr;
	LCD_ROW = row;
	LCD_COL = col;

	mode4bit ();
	FuntionSet(0x28);
	DisplayOnOff(0x0c);
	ClearDisplay();
	EntryModeSet(0x06);
	ReturnHome();
}
/*-------------------------------------------------------------------*/
void line_lcd(unsigned char *p,unsigned char r){
	if(r==1) r = 0;
	if(r==2) r = 0x40;
	if(r==3) r = 0x14;
	if(r==4) r = 0x54;

	SetDDRAMAddress(r);
	r = 0;
	while(r<LCD_COL){
		send_lcd(*p,k_LCDdat);
		p++;
		r++;
		if(*p == 0) r = LCD_COL;
	}
}
/*-------------------------------------------------------------------*/
void send (uint8_t dat){
	send_i2c (dat);
	delay_ms(2);
	send_i2c (dat|0x0c);	//Enable 1
	delay_ms(1);
	send_i2c (dat|0x08);	//Enable 0
}
/*-------------------------------------------------------------------*/
void send_lcd (uint8_t wb,uint8_t rs){
	uint8_t aux;
	byte dat;

	switch (rs){
		case k_LCDcom:			//RS = 0
			aux = wb << 4;
			aux |= 0x08;
			send (aux);
			delay_ms (5);
			break;
		case k_LCDdat:			//RS = 1
			dat = nibble (wb);
			dat.nb1 = dat.nb1 << 4;
			dat.nb2 = dat.nb2 << 4;
			dat.nb1|= 0x09;
			dat.nb2|= 0x09;
			send (dat.nb1);
			send (dat.nb2);
			delay_ms (1);
			break;
	}
}
/*-------------------------------------------------------------------*/
void send_i2c (uint8_t dat){
	int32_t r;
	uint8_t x[2];

	if (kStatus_Success == I2C_MasterStart(I2C1_BASE, LCD_ADDR, kI2C_Write))
    {
		x[0]= dat;
		r = I2C_MasterWriteBlocking(I2C1_BASE, x, 1, 0);
        r = I2C_MasterStop(I2C1_BASE);
    }
}
/*-------------------------------------------------------------------*/
byte nibble (unsigned char wb){
	byte aux;

	aux.nb1 = wb&0xf0;
	aux.nb1 = aux.nb1>>4;	//Primeros 4 bits
	aux.nb2 = wb&0x0f;		//Segundo 4bits

	return aux;
}
/*-------------------------------------------------------------------*/
void mode4bit (void){
	uint8_t i;

	send_lcd (0,k_LCDcom);

	delay_ms (15);

	for (i=0; i<3; i++)
		send_lcd (0x03,k_LCDcom);

	send_lcd (0x02,k_LCDcom);		//DL = 0 bus 4 bits
}
/*-------------------------------------------------------------------*/
void FuntionSet(unsigned char fs){
	byte aux;

	aux = nibble (fs);				// Funtion set: 8bit bus,N=1-1line,F=0-5x8dots
	send_lcd (aux.nb1,k_LCDcom);	// Primeros 4 bits
	send_lcd (aux.nb2,k_LCDcom);	// Ultimos 4 bits
	delay_ms(1);
}
/*-------------------------------------------------------------------*/
void DisplayOnOff(unsigned char dof){
	byte aux;

	aux = nibble (dof);				// Display ON/OFF:D=1 display on,C=0 cursor off,B=0,blink off
	send_lcd (aux.nb1,k_LCDcom);	// Primeros 4 bits
	send_lcd (aux.nb2,k_LCDcom);	// Ultimos 4 bits
	delay_ms(1);
}
/*-------------------------------------------------------------------*/
void ClearDisplay(void){
	byte aux;

	aux = nibble (0x01);			// Display clear
	send_lcd (aux.nb1,k_LCDcom);	// Primeros 4 bits
	send_lcd (aux.nb2,k_LCDcom);	// Ultimos 4 bits
	delay_ms(2);
}
/*-------------------------------------------------------------------*/
void EntryModeSet(unsigned char ems){
	byte aux;

	aux = nibble (ems);				// Entry mode set: I/D=1-increment mode,SH=0-shift off
	send_lcd (aux.nb1,k_LCDcom);	// Primeros 4 bits
	send_lcd (aux.nb2,k_LCDcom);	// Ultimos 4 bits
	delay_ms(1);
}
/*-------------------------------------------------------------------*/
void ReturnHome(void){
	byte aux;

	aux = nibble (0x02);			//Home
	send_lcd (aux.nb1,k_LCDcom);	// Primeros 4 bits
	send_lcd (aux.nb2,k_LCDcom);	// Ultimos 4 bits
	delay_ms(2);
}
/*-------------------------------------------------------------------*/
void SetDDRAMAddress(unsigned char sda){
	byte aux;

	sda |= 0x80;					//DB7 = 1 envio direccion ram
	aux = nibble (sda);
	send_lcd (aux.nb1,k_LCDcom);	// Primeros 4 bits
	send_lcd (aux.nb2,k_LCDcom);	// Ultimos 4 bits
	delay_ms(1);
}
/*-------------------------------------------------------------------*/
