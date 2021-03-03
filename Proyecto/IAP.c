#include "IAP.h"

/*-------------------------------------------------------------------*/
void limpiar (void){
    if( IAP_PrepareSectorForWrite(sector,sector) != kStatus_IAP_Success){
    	PRINTF("ERROR IAP_PrepareSectorForWrite \n");
    }

    if (IAP_EraseSector (sector,sector,frec) != kStatus_IAP_Success){
    	PRINTF("ERROR IAP_EraseSector \n");
    }
}
/*-------------------------------------------------------------------*/
void alta (user_t *usuario){
    uint16_t pag;
    uint32_t addr;

    addr = addr_pg;
    pag = page;

    while (!buscar_libre (addr)){
    	addr += 0x40;
    	pag++;
    }

    PRINTF("Escritura de Memoria Flash\n");

    if(IAP_PrepareSectorForWrite(sector,sector)!=kStatus_IAP_Success){
    	PRINTF("ERROR IAP_PrepareSectorForWrite \n");
    }

    status_t resp = IAP_CopyRamToFlash(addr, &usuario->uid[0], sizeof (user_t), frec);

    if(resp!=kStatus_IAP_Success){
    	PRINTF("ERROR IAP_CopyRamToFlash \n");
    }
}
/*-------------------------------------------------------------------*/
void baja (uint8_t *uid){
	uint16_t pag;
	uint32_t addr;

	addr = buscar_uid (uid);					//Obtengo la direccion donde se encontro el uid

	if (addr != 0){
		addr = ((addr - 4) - addr_pg) / 0x40;	//Obtengo el nro de pagina
		pag = addr + page;

	    if( IAP_PrepareSectorForWrite(sector,sector) != kStatus_IAP_Success){
	    	PRINTF("ERROR IAP_PrepareSectorForWrite \n");
	    }

	    if (IAP_ErasePage (pag,pag,frec) != kStatus_IAP_Success){
	    	PRINTF("ERROR IAP_ErasePage \n");
	    }
	}
	else
		PRINTF ("NO SE ENCONTRO REGISTRO");
}
/*-------------------------------------------------------------------*/
uint32_t buscar_uid (uint8_t *uid){
	uint8_t *addr;
	uint8_t i, flag = 0;

	addr = addr_pg;

	do{
		for (i = 0; i < 4; i++){
			if (*addr != uid [i]){
				flag = 1;
				break;
			}

			flag = 0;
			addr++;
		}
	}while (!buscar_libre (addr+=0x40) && flag);

	addr -= 0x40;		//Correccion de la direccion

	if (flag != 0)
		return 0;

	return addr;
}
/*-------------------------------------------------------------------*/
uint32_t buscar_psw (uint8_t *psw){
	uint8_t *addr;
	uint8_t i, flag = 0;

	addr = addr_pg + 0x14;

	do{
		for (i = 0; i < 4; i++){
			if (*addr != psw [i]){
				flag = 1;
				break;
			}

			flag = 0;
			addr++;
		}
	}while (!buscar_libre (addr+=0x40) && flag);

	addr -= 0x54;		//Correccion de la direccion

	if (flag != 0)
		return 0;

	return addr;
}
/*-------------------------------------------------------------------*/
void buscar_nombre (uint32_t dir, char *name){
	uint8_t *addr, i;

	for (i = 0; i < 16; i++)
		name [i] = 0x0;

    addr = dir;

    for (i = 0; i < 16; i++){
    	if (*addr == NULL)
    		break;
    	name [i] = *addr;
    	addr++;
    }
}
/*-------------------------------------------------------------------*/
bool buscar_libre (uint32_t dir){
	uint32_t *addr;

	addr = dir;

	if (*addr != 0xFFFFFFFF)
		return false;
	else
		return true;
}
/*-------------------------------------------------------------------*/
