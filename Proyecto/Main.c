#include "header.h"

uint32_t count_mseg = 0;

//---------------------------------------------------------------//
// main
//---------------------------------------------------------------//

int main(void)
{

	uint8_t uid [4];
	uint32_t addr;
	char nom [16];

	init ();								//configuro los perifericos

	lcd_init (0x27,2,16);					//configuro el lcd

	config_spi ();							//carga la configuracion del spi

    MFRC522_Init ();						//inicializo el lector

    ClearDisplay ();

    while (1){
    	line_lcd ("Esperando Tarj.", 1);

		while (!leer_tarjeta (uid)){
			addr = buscar_uid (uid);

			if (addr != 0){
				buscar_nombre (addr, &nom);
				abrir (&nom);
			}
			else{
				ClearDisplay();
				line_lcd ("TARJETA", 1);
				line_lcd ("NO ENCONTRADA", 2);
				delay_ms (2000);
				ClearDisplay();
			}
		}
		if (ENTER == 0){
			delay_ms (500);
			menu ();
		}
    }
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

