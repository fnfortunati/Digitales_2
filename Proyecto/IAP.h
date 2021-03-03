#ifndef IAP_H_
#define IAP_H_

#include "fsl_iap.h"
#include "fsl_debug_console.h"

#define frec		12000000
#define addr_pg		0x00007000
#define sector		28
#define page		448

typedef struct{
	uint8_t uid [4];
	char nombre [16];
	char clave [4];
	uint32_t basura[10];
}user_t;

void limpiar (void);
void alta (user_t *usuario);
void baja (uint8_t *uid);
uint32_t buscar_uid (uint8_t *uid);
void buscar_nombre (uint32_t dir, char *name);
uint32_t buscar_psw (uint8_t *psw);
bool buscar_libre (uint32_t dir);

#endif
