#include "header.h"

//---------------------------------------------------------------//
// Variables
//---------------------------------------------------------------//
uint8_t admin[4] = {0x4, 0x92, 0x90, 0x2b};
uint32_t baudRate = 100000;
uint32_t frecuency = 12000000;

//---------------------------------------------------------------//
void init (void){
    BOARD_InitDebugConsole();
	BOARD_BootClockFRO24M();
    (void) SysTick_Config(SystemCoreClock/1000);		//SystemCoreClock/Tiempo en us = Genera interrupciones cada 1 ms

    CLOCK_Select(kI2C1_Clk_From_MainClk);
    CLOCK_EnableClock(kCLOCK_Swm);

    //PINES SPI

    SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_SCK, kSWM_PortPin_P0_28);
    SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_MOSI, kSWM_PortPin_P0_27);
    SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_MISO, kSWM_PortPin_P0_26);

    //PINES I2C

    SWM_SetMovablePinSelect(SWM0, kSWM_I2C1_SCL, kSWM_PortPin_P0_14);
    SWM_SetMovablePinSelect(SWM0, kSWM_I2C1_SDA, kSWM_PortPin_P0_15);

    CLOCK_DisableClock(kCLOCK_Swm);

    CLOCK_Select(kSPI0_Clk_From_MainClk);

    //CONFIGURACION GPIO

    gpio_pin_config_t out_config = {kGPIO_DigitalOutput,1};
	gpio_pin_config_t in_config	= {kGPIO_DigitalInput};

    GPIO_PortInit (GPIO,0);
    GPIO_PinInit (GPIO,0,8,&in_config);		//BOTON IZQUIERDA
    GPIO_PinInit (GPIO,0,9,&in_config);		//BOTON ABAJO
    GPIO_PinInit (GPIO,0,10,&in_config);	//BOTON ARRIBA
    GPIO_PinInit (GPIO,0,11,&in_config);	//BOTON DERECHA
    GPIO_PinInit (GPIO,0,13,&in_config);	//BOTON ENTER
    GPIO_PinInit (GPIO,0,22,&out_config);	//LED PUERTA

	GPIO_PinInit (GPIO,0,25,&out_config);	//PIN RST
	GPIO_PinInit (GPIO,0,29,&out_config);	//PIN CHIP SELECT

    i2c_master_config_t masterConfig;
    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = baudRate;
    I2C_MasterInit(I2C1_BASE, &masterConfig, frecuency);
}
//---------------------------------------------------------------//
void menu (void){
	uint8_t i = 0;
	uint8_t opc = 0;

	ClearDisplay ();

	do{
		line_lcd ("MENU",1);

		if (UP == 0){
			delay_ms (500);
			i--;
		}
		if (DOWN == 0){
			delay_ms (500);
			i++;
		}
		if (i == 0xFF)
			i = 0;
		if (i > 3)
			i = 3;

		switch (i){
			case 0:
				line_lcd ("ING. MANUAL   <-",2);
				if (ENTER == 0)
					opc = ing_manual ();
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
	ClearDisplay ();
}
//---------------------------------------------------------------//
bool ing_manual (void){
	char clave [4];
	char nom [16];
	uint32_t dir;

	ClearDisplay ();

	cargar_clave (&clave);
	dir = buscar_psw (clave);

	ClearDisplay ();

	line_lcd ("BUSCANDO...", 1);
	delay_ms (2000);

	if (dir != 0){
		buscar_nombre (dir, &nom);
		abrir (&nom);
		return 1;
	}
	else{
		ClearDisplay ();
		line_lcd ("CLAVE", 1);
		line_lcd ("INCORRECTA", 2);
		delay_ms (2000);
		return 0;
	}
}
//---------------------------------------------------------------//
void alta_pers (void){
	uint8_t uid [4], aux = 0;
	user_t usuario = {0x0};

	ClearDisplay ();

	line_lcd ("ING. TARJ. ADM.", 1);

	do{
		while (!leer_tarjeta (uid)){							//Leo la tarjeta admin
			if ((compare (admin, uid)) == 0){					//Busco coincidencia con la tarj. admin
				ClearDisplay ();
				line_lcd ("INGRESO EXITOSO",1);
				delay_ms (2000);

				cargar_nombre (usuario.nombre);
				cargar_clave (usuario.clave);

				ClearDisplay ();
				line_lcd ("Esperando Tarj.", 1);

				do{
					aux = leer_tarjeta (usuario.uid);
				}while (aux == 1);

				alta (&usuario);

				ClearDisplay ();

				line_lcd ("CARGA EXITOSA",1);
				delay_ms (2000);

			}
			else{
				ClearDisplay ();
				line_lcd ("USUARIO NO",1);
				line_lcd ("ENCONTRADO",2);
				delay_ms (2000);
			}
			aux = 1;
		}
	}while (aux == 0);
	ClearDisplay ();
}
//---------------------------------------------------------------//
void baja_pers (void){
	uint8_t uid [4],aux = 0;
	uint16_t pag;
	uint32_t addr;

	ClearDisplay ();

	line_lcd ("ING. TARJ. ADM.", 1);
	delay_ms (2000);
	do{
		while (!leer_tarjeta (uid)){							//Leo la tarjeta admin
			if ((compare (admin, uid)) == 0){					//Busco coincidencia con la tarj. admin
				ClearDisplay ();
				line_lcd ("INGRESO EXITOSO",1);
				delay_ms (2000);
				ClearDisplay ();
				line_lcd ("TARJ. A BORRAR",1);
				do{
					while (!leer_tarjeta (uid)){
						addr = buscar_uid (uid);					//Obtengo la direccion donde se encontro el uid

						if (addr != 0){
							addr = ((addr - 4) - addr_pg) / 0x40;	//Obtengo el nro de pagina
							pag = addr + page;

							baja (pag);								//Borro la pagina
							ClearDisplay ();
							line_lcd ("BORRADO EXITOSO",1);
						}
						else{
							ClearDisplay ();
							line_lcd ("USUARIO NO",1);
							line_lcd ("ENCONTRADO",2);
						}
						aux = 1;
					}
				}while (aux == 0);
			}
			else{
				ClearDisplay ();
				line_lcd ("USUARIO NO",1);
				line_lcd ("ENCONTRADO",2);
				aux = 1;
			}
		}
	}while (aux == 0);

	ClearDisplay ();
}
//---------------------------------------------------------------//
void cargar_nombre (char *nombre){
	uint8_t i = 0, j = 0;
	uint8_t opc = 0;

	ClearDisplay ();

	for (i = 0; i < 16; i++){
		nombre[i] = 0;
	}

	i = 0;

	do{
		line_lcd ("INGRESE NOMBRE:",1);

		if (DOWN == 0){
			delay_ms (500);
			j++;
			if (j != 0)
				nombre[i] = 65 + (j-1);
			if (j > 26){
				j =0;
				nombre [i] = '\0';
			}
		}
		if (UP == 0){
			delay_ms (500);
			j--;
			if (j != 0)
				nombre[i] = 65 + (j-1) ;
			if (j == 0xFF){
				j = 0;
				nombre [i] = '\0';
			}
		}
		if (LEFT == 0){
			delay_ms (500);
			i--;
			if (i < 0)
				i = 0;
		}
		if (RIGHT == 0){
			delay_ms (500);
			i++;
			if (i > 15)
				i = 15;
		}

		line_lcd (nombre,2);

		if (ENTER == 0){
			delay_ms (500);
			opc = 1;
		}
	}while (opc == 0);
}
//---------------------------------------------------------------//
void cargar_clave (char *clave){
	uint8_t i = 0, j = 0;
	uint8_t opc = 0;

	ClearDisplay ();

	for (i = 0; i < 4; i++){
		clave[i] = '\0';
	}

	i = 0;

	do{
		line_lcd ("INGRESE CLAVE:",1);

		if (DOWN == 0){
			j++;
			if (j != 0)
				clave[i] = 48 + (j-1);
			if (j > 26){
				j = 0;
				clave [i] = '\0';
			}
		}
		if (UP == 0){
			j--;
			if (j != 0)
				clave[i] = 48 + (j-1) ;
			if (j == 0xFF){
				j = 0;
				clave [i] = '\0';
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
	}while (opc == 0);
}
//---------------------------------------------------------------//
bool leer_tarjeta (uint8_t *uid){
	uint8_t i;
	uid_t aux;

	if (MFRC522_IsNewCardPresent()){
		ClearDisplay();
		line_lcd ("Leyendo Tarj.",1);
		PRINTF ("Tarj. Detectada\n");
		if (!MFRC522_ReadCardSerial(&aux))
			return 0;

		PRINTF ("Card UID: ");

		for  (i = 0; i < aux.size; i++) {
			uid[i] = aux.uidByte [i];
			if (aux.uidByte[i] < 0x10)  		// si el byte leido es menor a 0x10
				PRINTF (" 0");     				// imprime espacio en blanco y numero cero
			else
				PRINTF (" ");      				// imprime un espacio en blanco

			PRINTF ("%x", aux.uidByte[i]);		// imprime el byte del UID leido en hexadecimal
		}

		MFRC522_HaltA ();

		MFRC522_StopCrypto ();

		PRINTF ("\n\n");						//cambio de linea

		delay_ms (1000);
		return 0;
	}

	return 1;
}
//---------------------------------------------------------------//
void abrir (char *nombre){
	ClearDisplay();
	line_lcd ("BIENVENID@",1);
	line_lcd (nombre,2);

	OPEN(0);
	delay_ms (2000);
	OPEN (1);
	ClearDisplay();
}
//---------------------------------------------------------------//
bool compare (uint8_t *a, uint8_t *b){
	uint8_t i, result;

	for (i = 0; i < 4; i++){
		if (a[i] == b [i])
			result = 0;
		else
			result = 1;
	}

	return result;
}
//---------------------------------------------------------------//
