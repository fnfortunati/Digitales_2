#ifndef HEADER_H_
#define HEADER_H_

#include <stdio.h>
#include <string.h>
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_iocon.h"
#include "fsl_swm.h"
#include "pin_mux.h"
#include "fsl_i2c.h"
#include "fsl_spi.h"

#include "LCD_I2C.h"
#include "RFID.h"

#define UP			GPIO_PinRead(GPIO,0,8)
#define DOWN		GPIO_PinRead(GPIO,0,9)
#define LEFT		GPIO_PinRead(GPIO,0,10)
#define RIGHT		GPIO_PinRead(GPIO,0,11)
#define ENTER		GPIO_PinRead(GPIO,0,12)
#define OPEN(x)		GPIO_PinWrite(GPIO,0,14,x)

//---------------------------------------------------------------//
// Prototipos
//---------------------------------------------------------------//
void init (void);
void menu (void);
void ing_manual (void);
void alta_pers (void);
void baja_pers (void);
void cargar_nombre (char *nombre);
void cargar_clave (char *clave);
void abrir (char *nombre);

#endif /* HEADER_H_ */
