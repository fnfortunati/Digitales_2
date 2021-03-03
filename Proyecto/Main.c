#include "header.h"

uint32_t count_mseg = 0;

//---------------------------------------------------------------//
// main
//---------------------------------------------------------------//

int main(void)
{
	uint8_t i;
	uid_t uid;


	init ();								//configuro los perifericos
	lcd_init (0x27,2,16);					//configuro el lcd
	config_spi ();							//Carga la configuracion del spi

    MFRC522_Init ();

    PRINTF ("REINICIO COMPLETO\n");

    while (1){
    	if (MFRC522_IsNewCardPresent()){
    		PRINTF ("Tarjeta Detectada\n");

			if (!MFRC522_ReadCardSerial(&uid))
				return 0;

			PRINTF ("Card UID: ");

			for  (i = 0; i < uid.size; i++) {
				if (uid.uidByte[i] < 0x10)  		// si el byte leido es menor a 0x10
					PRINTF (" 0");     				// imprime espacio en blanco y numero cero
				else
					PRINTF (" ");      				// imprime un espacio en blanco

				PRINTF ("%x", uid.uidByte[i]);		// imprime el byte del UID leido en hexadecimal
			}

			MFRC522_HaltA ();

			MFRC522_StopCrypto ();

			PRINTF ("\n\n");						//cambio de linea
    	}
    }

/*
	char dat1[16] = "Linea 1";
	char dat2[16] = "Linea 2";


	while (1){
		line_lcd (dat1,1);
		line_lcd (dat2,2);
	}
*/
}
//---------------------------------------------------------------//

// Interrupciones

/*-------------------------------------------------------------------*/
void SysTick_Handler(void){
	if (count_mseg != 0) count_mseg--;
}
/*-------------------------------------------------------------------*/

// Funciones

/*-------------------------------------------------------------------*/
void delay_ms(uint32_t ms){
    count_mseg = ms;
    while(count_mseg != 0);
}
/*-------------------------------------------------------------------*/

