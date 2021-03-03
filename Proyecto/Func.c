#include "header.h"

//---------------------------------------------------------------//
// Variables
//---------------------------------------------------------------//
uint32_t baudRate = 100000;
uint32_t frecuency = 12000000;

/*-------------------------------------------------------------------*/
void init (void){
    BOARD_InitDebugConsole();
	BOARD_BootClockFRO24M();
    (void) SysTick_Config(SystemCoreClock/1000);		//SystemCoreClock/Tiempo en us = Genera interrupciones cada 1 ms

    CLOCK_Select(kI2C1_Clk_From_MainClk);
    CLOCK_EnableClock(kCLOCK_Swm);

    //PINES SPI

    SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_SCK, kSWM_PortPin_P0_21);
    SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_MOSI, kSWM_PortPin_P0_22);
    SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_MISO, kSWM_PortPin_P0_23);

    //PINES I2C

    SWM_SetMovablePinSelect(SWM0, kSWM_I2C1_SCL, kSWM_PortPin_P0_14);
    SWM_SetMovablePinSelect(SWM0, kSWM_I2C1_SDA, kSWM_PortPin_P0_15);

    CLOCK_DisableClock(kCLOCK_Swm);

    CLOCK_Select(kSPI0_Clk_From_MainClk);

    //CONFIGURACION GPIO

    gpio_pin_config_t out_config = {kGPIO_DigitalOutput,1};
	gpio_pin_config_t in_config	= {kGPIO_DigitalInput};

    GPIO_PortInit (GPIO,0);
    GPIO_PinInit (GPIO,0,8,&in_config);		//BOTON ARRIBA
    GPIO_PinInit (GPIO,0,9,&in_config);		//BOTON ABAJO
    GPIO_PinInit (GPIO,0,10,&in_config);	//BOTON IZQUIERDA
    GPIO_PinInit (GPIO,0,11,&in_config);	//BOTON DERECHA
    GPIO_PinInit (GPIO,0,12,&in_config);	//BOTON ENTER
    GPIO_PinInit (GPIO,0,14,&out_config);	//LED PUERTA

    GPIO_PinInit (GPIO,0,20,&out_config);	//SPI CHIP SELECT
	GPIO_PinInit (GPIO,0,24,&out_config);	//SPI RESET

    i2c_master_config_t masterConfig;
    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = baudRate;
    I2C_MasterInit(I2C1_BASE, &masterConfig, frecuency);
}
/*-------------------------------------------------------------------*/
void menu (void){
	uint8_t i = 0;
	uint8_t opc = 0;

	do{
		line_lcd ("MENU",1);

		if (UP == 0)
			i++;
		if (DOWN == 0)
			i--;

		if (i < 0)
			i = 0;
		if (i > 3)
			i = 3;

		switch (i){
			case 0:
				line_lcd ("ING. MANUAL   <-",2);
				if (ENTER == 0)
					ing_manual ();
				break;
			case 1:
				line_lcd ("ALTA PERSONA  <-",2);
				if (ENTER == 0)
					alta_pers ();
				break;
			case 2:
				line_lcd ("BAJA PERSONA  <-",2);
				if (ENTER == 0)
					baja_pers ();
				break;
			case 3:
				line_lcd ("SALIR         <-",2);
				if (ENTER == 0)
					opc = 1;
				break;
		}
	}while (opc != 1);
}
/*-------------------------------------------------------------------*/
void ing_manual (void){
	char clave [4];
	char nom [16];
	uint32_t dir;

	cargar_clave (clave);
	dir = buscar_psw (clave);

	line_lcd ("BUSCANDO...", 1);
	line_lcd ("", 2);
	delay (5000);

	if (dir != 0){
		buscar_nombre (dir, &nom);
		abrir (&nom);
	}
	else{
		line_lcd ("CLAVE", 1);
		line_lcd ("INCORRECTA", 2);
		delay (5000);
	}
}
/*-------------------------------------------------------------------*/
void alta_pers (void){

}
/*-------------------------------------------------------------------*/
void baja_pers (void){

}
/*-------------------------------------------------------------------*/
void cargar_nombre (char *nombre){
	uint8_t i = 0, j = 0;
	uint8_t opc = 0;

	for (i = 0; i < 16; i++){
		nombre[i] = 0;
	}

	i = 0;

	do{
		line_lcd ("INGRESE NOMBRE:",1);

		if (UP == 0){
			j++;
			if (j != 0)
				nombre[i] = 65 + (j-1);
			if (j > 26){
				j = 0;
				nombre [i] = 0;
			}
		}
		if (DOWN == 0){
			j--;
			if (j != 0)
				nombre[i] = 65 + (j-1) ;
			if (j < 0){
				j = 0;
				nombre [i] = 0;
			}
		}
		if (LEFT == 0){
			i--;
			if (i < 0)
				i = 0;
		}
		if (RIGHT == 0){
			i++;
			if (i > 15)
				i = 15;
		}

		line_lcd (nombre,2);

		if (ENTER == 0)
			opc = 1;
	}while (opc != 0);
}
/*-------------------------------------------------------------------*/
void cargar_clave (char *clave){
	uint8_t i = 0, j = 0;
	uint8_t opc = 0;

	for (i = 0; i < 4; i++){
		clave[i] = 0;
	}

	i = 0;

	do{
		line_lcd ("INGRESE CLAVE:",1);

		if (UP == 0){
			j++;
			if (j != 0)
				clave[i] = 48 + (j-1);
			if (j > 26){
				j = 0;
				clave [i] = 0;
			}
		}
		if (DOWN == 0){
			j--;
			if (j != 0)
				clave[i] = 48 + (j-1) ;
			if (j < 0){
				j = 0;
				clave [i] = 0;
			}
		}
		if (LEFT == 0){
			i--;
			if (i < 0)
				i = 0;
		}
		if (RIGHT == 0){
			i++;
			if (i > 3)
				i = 3;
		}

		line_lcd (clave,2);

		if (ENTER == 0)
			opc = 1;
	}while (opc != 0);
}
/*-------------------------------------------------------------------*/
void abrir (char *nombre){
	line_lcd ("BIENVENIDO",1);
	line_lcd (nombre,2);

	OPEN(0);
	delay (5000);
	OPEN (1);
}
/*-------------------------------------------------------------------*/
