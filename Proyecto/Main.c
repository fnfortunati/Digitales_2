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
/*
	config_spi ();							//carga la configuracion del spi

    MFRC522_Init ();						//inicializo el lector

    while (1){
		while (!leer_tarjeta (uid)){
			addr = buscar_uid (uid);

			if (addr != 0){
				buscar_nombre (addr, &nom);
				abrir (&nom);
			}
			else{
				line_lcd ("TARJETA", 1);
				line_lcd ("NO ENCONTRADA", 2);
				delay_ms (5000);
			}
		}
    }

*/
/*
	char dat1[16] = "Fila 1";
	char dat2[16] = "Fila 2";


	while (1){
		line_lcd (dat1,1);
		line_lcd (dat2,2);
	}
*/
	cargar_nombre (nom);
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

