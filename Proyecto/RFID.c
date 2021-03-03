#include "RFID.h"

#define BUFFER_SIZE (64)

spi_master_config_t userConfig = {0};
uint32_t srcFreq               = 0U;

static uint8_t tx_buffer[BUFFER_SIZE];
static uint8_t rx_buffer[BUFFER_SIZE];

/*-------------------------------------------------------------------*/
void config_spi (void){
	SPI_MasterGetDefaultConfig(&userConfig);
	userConfig.baudRate_Bps           = 847500;
	userConfig.sselNumber             = kSPI_SselDeAssertAll;
	userConfig.clockPolarity		  = kSPI_ClockPolarityActiveLow;
	userConfig.clockPhase             = kSPI_ClockPhaseSecondEdge;
	userConfig.delayConfig.frameDelay = 0x0U;
	srcFreq                           = CLOCK_GetFreq(kCLOCK_MainClk);
}
/*-------------------------------------------------------------------*/
void clear_buff (void){
	uint8_t i;

	for (i = 0; i < BUFFER_SIZE; i++){
		tx_buffer [i] = 0;
		rx_buffer [i] = 0;
	}
}
/*-------------------------------------------------------------------*/
uint8_t MFRC522_Rd (uint8_t addr){
	uint8_t aux_addr;

	SPI_MasterInit(SPI0, &userConfig, srcFreq);

	aux_addr = ((addr<<1) & 0x7E) | 0x80;

	clear_buff ();

	tx_buffer [0] = aux_addr;
	tx_buffer [1] = 0x00;

    spi_transfer_t xfer = {0};
	xfer.txData      = tx_buffer;
	xfer.rxData      = rx_buffer;
	xfer.dataSize    = 2;
	xfer.configFlags = kSPI_EndOfTransfer | kSPI_EndOfFrame;

	MFRC522_CS(0);								//Habilito el CS

	SPI_MasterTransferBlocking(SPI0, &xfer);	//Realizo la comunicacion

	MFRC522_CS(1);								//Dehabilito el CS

	SPI_Deinit(SPI0);

	return rx_buffer[1];
}
/*-------------------------------------------------------------------*/
void MFRC522_Rd_Buff (uint8_t addr, uint8_t cant, unsigned char *values){
	uint8_t i;

	if (cant == 0)
		return;

	i = 0;

	while (i < cant){
		values [i] = MFRC522_Rd(addr);
		i++;
	}
}
/*-------------------------------------------------------------------*/
void MFRC522_Wr (uint8_t addr, uint8_t value){
	uint8_t aux_addr;

	SPI_MasterInit(SPI0, &userConfig, srcFreq);

	aux_addr = ((addr<<1) & 0x7E);

	clear_buff ();

	tx_buffer [0] = aux_addr;
	tx_buffer [1] = value;

    spi_transfer_t xfer = {0};
	xfer.txData      = tx_buffer;
	xfer.rxData      = rx_buffer;
	xfer.dataSize    = 2;
	xfer.configFlags = kSPI_EndOfTransfer | kSPI_EndOfFrame;

	MFRC522_CS(0);								//Habilito el CS

	SPI_MasterTransferBlocking(SPI0, &xfer);	//Realizo la comunicacion

	SPI_Deinit(SPI0);

	MFRC522_CS(1);								//Dehabilito el CS
}
/*-------------------------------------------------------------------*/
void MFRC522_Wr_Buff (uint8_t addr, uint8_t cant, uint8_t *value){
	uint8_t aux_addr, i;

	SPI_MasterInit(SPI0, &userConfig, srcFreq);

	aux_addr = ((addr<<1) & 0x7E);

	clear_buff ();

	tx_buffer [0] = aux_addr;

	for (i=0; i < cant; i++){
		tx_buffer [i+1] = value[i];
	}

    spi_transfer_t xfer = {0};
	xfer.txData      = tx_buffer;
	xfer.rxData      = rx_buffer;
	xfer.dataSize    = cant+1;
	xfer.configFlags = kSPI_EndOfTransfer | kSPI_EndOfFrame;

	MFRC522_CS(0);								//Habilito el CS

	SPI_MasterTransferBlocking(SPI0, &xfer);	//Realizo la comunicacion

	SPI_Deinit(SPI0);

	MFRC522_CS(1);								//Dehabilito el CS
}
/*-------------------------------------------------------------------*/
void MFRC522_Clear (char addr, char mask){
	uint8_t aux = 0;

	aux = MFRC522_Rd (addr);
	MFRC522_Wr (addr, aux&~mask);
}
/*-------------------------------------------------------------------*/
void MFRC522_Set (char addr, char mask){
	uint8_t aux = 0;

	aux = MFRC522_Rd (addr);
	MFRC522_Wr (addr, aux|mask);

	aux = MFRC522_Rd (addr);
}
/*-------------------------------------------------------------------*/
void MFRC522_Reset (void){
	uint8_t aux;

	MFRC522_RST(1);
	delay_ms (1);
	MFRC522_RST(0);
	delay_ms (1);
	MFRC522_RST(1);
	delay_ms (1);

	MFRC522_Wr (COMMANDREG,PCD_RESETPHASE);

	//Verifico el cambio de estado del COMMANDREG

	do{
		aux = MFRC522_Rd(COMMANDREG);
	}while (aux & 0x10);
}
/*-------------------------------------------------------------------*/
void MFRC522_AntenaOn (void){
	MFRC522_Set (TXCONTROLREG,0x03);
}
/*-------------------------------------------------------------------*/
void MFRC522_AntenaOff (void){
	MFRC522_Clear (TXCONTROLREG,0x03);
}
/*-------------------------------------------------------------------*/
void MFRC522_Init (void){
	MFRC522_Reset ();
	MFRC522_Wr (TXMODEREG,0x00);
	MFRC522_Wr (RXMODEREG,0x00);
	MFRC522_Wr (TMODEREG,0x80);			//Tauto = 1 ; TPrescaler_Hi = 0x0
	MFRC522_Wr (TPRESCALERREG,0xA9);	//TPrescaler_Lo = 0xA9; f(Timer) = 13.56 Mhz / (2*Tprescaler + 1) = 40 Khz
	MFRC522_Wr (TRELOADREGH,0x03);
	MFRC522_Wr (TRELOADREGL,0xE8);		//Reload Timer con 0x3E8 = 1000, son 25ms antes que el timeout
	MFRC522_Wr (TXASKREG,0x40);			//100% ASK
	MFRC522_Wr (MODEREG,0X3D);			//CRC valor inicial 0x6363
	MFRC522_AntenaOff ();
	MFRC522_AntenaOn ();
}
/*-------------------------------------------------------------------*/
unsigned char MFRC522_CALC_CRC (unsigned char *data, unsigned char length, unsigned char *result){
	uint16_t i;
	uint8_t n;

	MFRC522_Wr (COMMANDREG, PCD_IDLE);				//Detiene todo comando activo
	MFRC522_Wr (DIVIRQREG, 0x04);					//Limpia el CRCIRQ
	MFRC522_Wr (FIFOLEVELREG, 0x80);				//FlushBuffer = 1 , inicializacion FIFO
	MFRC522_Wr_Buff (FIFODATAREG, length, data);	//Escribo data en FIFO
	MFRC522_Wr (COMMANDREG, PCD_CALCCRC);			//Realiza el calculo

	for (i = 5000; i > 0; i--){
		n = MFRC522_Rd (DIVIRQREG);

		if (n & 0x04){
			MFRC522_Wr (COMMANDREG, PCD_IDLE);

			result [0] = MFRC522_Rd (CRCRESULTREGL);
			result [1] = MFRC522_Rd (CRCRESULTREGM);

			return STATUS_OK;
		}
	}

	return STATUS_TIMEOUT;
}
/*-------------------------------------------------------------------*/
unsigned char MFRC522_Request (unsigned char *bufferATQA, unsigned char *bufferSize){
	unsigned char validBits;
	unsigned char status;
	unsigned char command = PICC_REQIDL;

	if (bufferATQA == NULL || (*bufferSize <2)){
		return STATUS_NO_ROOM;
	}

	MFRC522_Clear (COLLREG, 0x80); //No esta en el codigo

	validBits = 7;

	status = MFRC522_TrasceiveData (&command, 1, bufferATQA, bufferSize, &validBits, 0x0, false);

	if (status != STATUS_OK)
		return status;
	if (*bufferSize != 2 || validBits != 0)
		return STATUS_ERROR;

	return STATUS_OK;
}
/*-------------------------------------------------------------------*/
unsigned char MFRC522_TrasceiveData (unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned char *backLen, unsigned char *validBits, unsigned char rxAlign, bool checkCRC){
	unsigned char waitIrq = 0x30;

	return MFRC522_CommuncicateWithPICC (PCD_TRANSCEIVE, waitIrq, sendData, sendLen, backData, backLen, validBits, rxAlign, checkCRC);
}
/*-------------------------------------------------------------------*/
unsigned char  MFRC522_CommuncicateWithPICC (unsigned char command, unsigned char waitIrq, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned char *backLen, unsigned char *validBits, unsigned char rxAlign, bool checkCRC){
	unsigned char txLastBits = validBits ? *validBits : 0;
	unsigned char bitFraming = (rxAlign << 4) + txLastBits;
	unsigned char errorVal;
	unsigned char _validBits = 0;
	unsigned char controlBuffer [2];

	uint8_t status;
	uint16_t i;
	uint8_t n;

	MFRC522_Wr (COMMANDREG,PCD_IDLE);						//Detiene todo comando activo
	MFRC522_Wr (COMMIRQREG, 0x7F);							//Limpia las interrupciones
	MFRC522_Wr (FIFOLEVELREG, 0x80);						//Limpia el buffer del FIFO
	MFRC522_Wr_Buff (FIFODATAREG, sendLen, sendData);		//Escribo sendData en FIFO
	MFRC522_Wr (BITFRAMINGREG, bitFraming);					//Ajuste de bits
	MFRC522_Wr (COMMANDREG, command);						//Ejecuto el comando

	if (command == PCD_TRANSCEIVE)
		MFRC522_Set (BITFRAMINGREG, 0x80);					//Inicia la transmision

	errorVal = MFRC522_Rd (ERRORREG);

	for (i = 2000; i > 0; i--){
		n = MFRC522_Rd (COMMIRQREG);

		if (n & waitIrq)
			break;
		if (n & 0x01)
			return STATUS_TIMEOUT;
	}

	if (i == 0)
		return STATUS_TIMEOUT;

	if (i != 0){
		errorVal = MFRC522_Rd (ERRORREG);

		if (errorVal == 0x13)					//BufferOvfl ParityErr ProtocolErr
			return STATUS_ERROR;

		if (errorVal & 0x08)
			return STATUS_COLL;

		if (backData && backLen){
			n = MFRC522_Rd (FIFOLEVELREG);		//Leo cantidad de bits almacenados en FIFO

			if (n > *backLen)
				return STATUS_NO_ROOM;

			*backLen = n;

			MFRC522_Rd_Buff (FIFODATAREG, n, backData);
			_validBits = MFRC522_Rd (CONTROLREG) & 0x07;

			if (validBits)
				*validBits = _validBits;
		}

		if (backData && backLen && checkCRC){
			if (*backLen == 1 && _validBits == 4)
				return STATUS_MIFARE_NACK;
			if (*backLen < 2 || _validBits != 0)
				return STATUS_CRC_WRONG;

			status = MFRC522_CALC_CRC (&backData [0], *backLen -2, &controlBuffer [0]);

			if (status != STATUS_OK)
				return status;

			if ((backData [*backLen -2] != controlBuffer [0]) || (backData [*backLen -1] != controlBuffer [1]))
				return STATUS_CRC_WRONG;
		}
	}
	return STATUS_OK;
}
/*-------------------------------------------------------------------*/
uint8_t MFRC522_PICC_Select (uid_t *uid, uint8_t validBits){
	bool uidComplete, selectDone, useCascadeTag;
	uint8_t result, count, checkBit, index, uidIndex;
	uint8_t currentLevelKnownBits;								//The number of know UID bits in the Current Cascade
	uint8_t cascadeLevel = 1;
	uint8_t buffer [9], bufferUsed, rxAlign, txLastBits;
	uint8_t *responseBuffer, responseLength;
	uint8_t bytesToCopy, maxBytes;
	uint8_t valueOfCollReg, collisionPos;

	//Descripcion de la estructura del buffer
	//Byte 0: SEL 			Indicates the Cascade Level: PICC_SELECTTAG_1 or PICC_SELECTTAG_2
	//Byte 1: NVB			Number of valid bits
	//Byte 2: UID-data
	//Byte 3: UID-data
	//Byte 4: UID-data
	//Byte 5: UID-data
	//Byte 6: BCC			Block check Character
	//Byte 7: CRC_A

	if (validBits > 80)
		return STATUS_INVALID;

	MFRC522_Clear (COLLREG, 0x80);

	uidComplete = false;

	while (!uidComplete){
		switch (cascadeLevel){
			case 1:
				buffer [0] = PICC_SELECTTAG_1;
				uidIndex = 0;
				useCascadeTag = validBits && uid->size > 4;		//When we know that the UID has more than 4 bytes

				break;
			case 2:
				buffer [0] = PICC_SELECTTAG_2;
				uidIndex = 3;
				useCascadeTag = validBits && uid->size > 7;		//When we know that the UID has more than 7 bytes

				break;
			default:
				return STATUS_INT_ERR;
		}

		//How many UID bits are known in this Cascade Level ?
		currentLevelKnownBits = validBits - (8 * uidIndex);

		if (currentLevelKnownBits < 0)
			currentLevelKnownBits = 0;

		//Copy the known bits from uid->uidByte [] to buffer []

		index = 2; 												//Destination index in buffer []

		if (useCascadeTag)
			buffer [index++] = PICC_CT;

		bytesToCopy = currentLevelKnownBits / 8 + (currentLevelKnownBits % 8 ? 1:0);	//The number of bytes needed to represent the known bits for this level

		if (bytesToCopy){
			maxBytes = useCascadeTag ? 3:4;						//Max 4 bytes in each Cascade Level. Only 3 left if we use the Cascade Tag

			if (bytesToCopy > maxBytes)
				bytesToCopy = maxBytes;

			for (count = 0; count < bytesToCopy; count++){
				buffer [index++] = uid->uidByte [uidIndex + count];
			}
		}

		if (useCascadeTag)
			currentLevelKnownBits += 8;

		selectDone = false;

		while (!selectDone){
			if (currentLevelKnownBits >= 32){					//All UID bits in this Cascade Level are known
				buffer [1] = 0x70;								//NVB - Number of Valid Bits
				//Calculo BCC - Block Check Character
				buffer [6] = buffer [2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
				//Calculo CRC_A
				result = MFRC522_CALC_CRC (buffer, 7, &buffer [7]);

				if (result != STATUS_OK)
					return result;

				txLastBits = 0;
				bufferUsed = 9;

				responseBuffer = &buffer [6];
				responseLength = 3;
			}
			else{
				txLastBits = currentLevelKnownBits % 8;
				count = currentLevelKnownBits / 8;
				index = 2 + count;
				buffer [1] = (index << 4) + txLastBits;
				bufferUsed = index + (txLastBits ? 1:0);

				responseBuffer = &buffer [index];
				responseLength = sizeof (buffer) - index;
			}

			rxAlign = txLastBits;

			MFRC522_Wr (BITFRAMINGREG, (rxAlign << 4) + txLastBits);

			result = MFRC522_TrasceiveData (buffer, bufferUsed, responseBuffer, &responseLength, &txLastBits, rxAlign, false);

			if (result == STATUS_COLL){
				valueOfCollReg = MFRC522_Rd (COLLREG);

				if (valueOfCollReg & 0x20)
					return STATUS_COLL;

				collisionPos = valueOfCollReg & 0x1F;

				if (collisionPos == 0)
					collisionPos = 32;

				if (collisionPos <= currentLevelKnownBits)
					return STATUS_INT_ERR;

				//Chose the PICC with the bit set
				currentLevelKnownBits = collisionPos;
				count = currentLevelKnownBits % 8;
				checkBit = (currentLevelKnownBits- 1) % 8;
				index = 1 + (currentLevelKnownBits /8) + (count ? 1:0);

				buffer [index] |= (1 << checkBit);
			}
			else {
				if (result != STATUS_OK)
					return result;
				else {
					if (currentLevelKnownBits >= 32){
						selectDone = true;
					}
					else
						currentLevelKnownBits = 32;
				}
			}
		}

		index = (buffer [2] == PICC_CT)? 3:2;
		bytesToCopy = (buffer [2] == PICC_CT)? 3:4;

		for (count = 0; count < bytesToCopy; count++){
			uid->uidByte [uidIndex + count] = buffer [index++];
		}

		//Check Response SAK
		if (responseLength != 3 || txLastBits != 0)
			return STATUS_ERROR;

		//Verify CRC_A
		result = MFRC522_CALC_CRC (responseBuffer, 1, &buffer [2]);

		if (result != STATUS_OK)
			return result;

		if ((buffer[2] != responseBuffer[1]) || (buffer[3] != responseBuffer[2]))
			return STATUS_CRC_WRONG;

		if (responseBuffer[0] & 0x04)
			cascadeLevel++;
		else {
			uidComplete = true;
			uid->sak = responseBuffer[0];
		}
	}

	uid->size = 3 * cascadeLevel + 1;

	return STATUS_OK;
}
/*-------------------------------------------------------------------*/
unsigned char MFRC522_HaltA (void){
	unsigned char result;
	unsigned char buffer [4];

	buffer [0] = PICC_HALT;
	buffer [1] = 0;

	result = MFRC522_CALC_CRC (buffer, 2, &buffer[2]);

	if (result == STATUS_OK)
		return result;

	result = MFRC522_TrasceiveData (buffer, sizeof (buffer), NULL, 0, NULL, 0, false);

	if (result == STATUS_TIMEOUT)
		return STATUS_OK;

	if (result == STATUS_OK)
		return STATUS_ERROR;

	return result;
}
/*-------------------------------------------------------------------*/
void MFRC522_StopCrypto (void){
	MFRC522_Clear (STATUS2REG, 0x08);
}
/*-------------------------------------------------------------------*/
bool MFRC522_IsNewCardPresent (void){
	unsigned char bufferATQA [2];
	unsigned char bufferSize = sizeof (bufferATQA);
	unsigned char result;

	//Reseteo baud rates
	MFRC522_Wr (TXMODEREG,0x00);
	MFRC522_Wr (RXMODEREG,0x00);

	//Reseteo el registro ModWidthReg
	MFRC522_Wr (MODWIDTHREG,0x26);

	result = MFRC522_Request (bufferATQA, &bufferSize);

	if ((result == STATUS_OK || result == STATUS_COLL))
		return true;
	else
		return false;
}
/*-------------------------------------------------------------------*/
bool MFRC522_ReadCardSerial (uid_t *uid){
	uint8_t result;

	result = MFRC522_PICC_Select (uid,0);

	return (result==STATUS_OK);
}
/*-------------------------------------------------------------------*/
void MFRC522_AutoTest (void){
	uint8_t bytes [25] = {0x00};
	uint8_t i, n;

    //Self Test

    PRINTF ("INICIO EL SELF-TEST\n");

    n = MFRC522_Rd (VERSIONREG);

    PRINTF ("LA VERSION DE LA PLACA ES: %x \n", n);

    //1. Soft Reset

    MFRC522_Reset ();

    //2. Limpiar el buffer interno

    MFRC522_Wr (FIFOLEVELREG, 0x80);			//Limpio el buffer enviandole un "1" en el bit 7
    MFRC522_Wr_Buff (FIFODATAREG, 25, bytes);	//Cargo todo el buffer con "0"
    MFRC522_Wr (COMMANDREG, PCD_MEM);			//Cargo los 25 bytes en el buffer interno

    n = MFRC522_Rd (COMMANDREG);				//Lectura de prueba

    //3. Habilito la prueba

    MFRC522_Wr (AUTOTESTREG, 0x09);				//Inicializo la prueba
    n = MFRC522_Rd (AUTOTESTREG);
    //4. Escribir 0 en el FIFOBUFFER

    MFRC522_Wr (FIFODATAREG, 0x00);

    //5. Escribo el comando CALCCRC

    MFRC522_Wr (COMMANDREG, PCD_CALCCRC);

    //6. Espero a que termine

	for (i = 0; i < 0xFF; i++) {
		//Leo el buffer para saber cuando tiene los 64 bytes
		n = MFRC522_Rd (FIFOLEVELREG);
		if (n >= 64) {
			break;
		}
	}

	MFRC522_Wr (COMMANDREG, PCD_IDLE);			//Detengo todos los procesos

	if (n >= 64){

		//7. Leo el buffer resultante

		uint8_t result[64];

		MFRC522_Rd_Buff (FIFODATAREG, 64, result);

		//8. Termino la prueba

		MFRC522_Wr (AUTOTESTREG, 0x00);

		PRINTF ("TERMINO LA PRUEBA");
	}
	else
		PRINTF ("FALLO LA PRUEBA");
}
/*-------------------------------------------------------------------*/
