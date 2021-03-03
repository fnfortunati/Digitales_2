#ifndef RFID_H_
#define RFID_H_

#include "stdint.h"
#include "fsl_gpio.h"
#include "fsl_spi.h"
#include "fsl_debug_console.h"

//---------------------------------------------------------------//
// Define
//---------------------------------------------------------------//
//------------------Comandos---------------
#define PCD_IDLE              0x00               //NO action; Cancel the current command
#define PCD_MEM				  0x01				 //Stores 25 bytes into internal buffer
#define PCD_CALCCRC           0x03               //CRC Calculate
#define PCD_TRANSMIT          0x04               //Transmit data
#define PCD_RECEIVE           0x08               //Receive Data
#define PCD_TRANSCEIVE        0x0C               //Transmit and receive data,
#define PCD_AUTHENT           0x0E               //Authentication Key
#define PCD_RESETPHASE        0x0F               //Reset


#define PICC_REQIDL          0x26               // find the antenna area does not enter hibernation
#define PICC_REQALL          0x52               // find all the cards antenna area
#define PICC_ANTICOLL_1      0x93               // anti-collision
#define PICC_SELECTTAG_1	 0x93               // election card
#define PICC_ANTICOLL_2      0x95               // anti-collision
#define PICC_SELECTTAG_2	 0x95               // election card
#define PICC_CT				 0x88				// Cascade Tag; Not really a command, but used during anticollision
#define PICC_AUTHENT1A       0x60               // authentication key A
#define PICC_AUTHENT1B       0x61               // authentication key B
#define PICC_READ            0x30               // Read Block
#define PICC_WRITE           0xA0               // write block
#define PICC_DECREMENT       0xC0               // debit
#define PICC_INCREMENT       0xC1               // recharge
#define PICC_RESTORE         0xC2               // transfer block data to the buffer
#define PICC_TRANSFER        0xB0               // save the data in the buffer
#define PICC_HALT            0x50               // Sleep
//------------------MFRC522 Register---------------
//PAGE 0:Command & Status
#define     RESERVED00            0x00
#define     COMMANDREG            0x01
#define     COMMIENREG            0x02
#define     DIVLENREG             0x03
#define     COMMIRQREG            0x04
#define     DIVIRQREG             0x05
#define     ERRORREG              0x06
#define     STATUS1REG            0x07
#define     STATUS2REG            0x08
#define     FIFODATAREG           0x09
#define     FIFOLEVELREG          0x0A
#define     WATERLEVELREG         0x0B
#define     CONTROLREG            0x0C
#define     BITFRAMINGREG         0x0D
#define     COLLREG               0x0E
#define     RESERVED01            0x0F
//PAGE 1:Command
#define     RESERVED10            0x10
#define     MODEREG               0x11
#define     TXMODEREG             0x12
#define     RXMODEREG             0x13
#define     TXCONTROLREG          0x14
#define     TXASKREG			  0x15
#define     TXSELREG              0x16
#define     RXSELREG              0x17
#define     RXTHRESHOLDREG        0x18
#define     DEMODREG              0x19
#define     RESERVED11            0x1A
#define     RESERVED12            0x1B
#define     MIFAREREG             0x1C
#define     RESERVED13            0x1D
#define     RESERVED14            0x1E
#define     SERIALSPEEDREG        0x1F
//PAGE 2:CFG
#define     RESERVED20            0x20
#define     CRCRESULTREGM         0x21
#define     CRCRESULTREGL         0x22
#define     RESERVED21            0x23
#define     MODWIDTHREG           0x24
#define     RESERVED22            0x25
#define     RFCFGREG              0x26
#define     GSNREG                0x27
#define     CWGSPREG              0x28
#define     MODGSPREG             0x29
#define     TMODEREG              0x2A
#define     TPRESCALERREG         0x2B
#define     TRELOADREGH           0x2C
#define     TRELOADREGL           0x2D
#define     TCOUNTERVALUEREGH     0x2E
#define     TCOUNTERVALUEREGL     0x2F
//PAGE 3:TEST REGISTER
#define     RESERVED30            0x30
#define     TESTSEL1REG           0x31
#define     TESTSEL2REG           0x32
#define     TESTPINENREG          0x33
#define     TESTPINVALUEREG       0x34
#define     TESTBUSREG            0x35
#define     AUTOTESTREG           0x36
#define     VERSIONREG            0x37
#define     ANALOGTESTREG         0x38
#define     TESTDAC1REG           0x39
#define     TESTDAC2REG           0x3A
#define     TESTADCREG            0x3B
#define     RESERVED31            0x3C
#define     RESERVED32            0x3D
#define     RESERVED33            0x3E
#define     RESERVED34            0x3F

#define MFRC522_CS(x)		GPIO_PinWrite(GPIO, 0, 20, x)
#define MFRC522_RST(x)		GPIO_PinWrite(GPIO, 0, 24, x)

#define STATUS_OK			0x00	//Correcto
#define STATUS_ERROR		0x01	//Error en comunicacion
#define STATUS_COLL			0x02	//Colision Detectada
#define STATUS_TIMEOUT		0x03	//TimeOut
#define STATUS_NO_ROOM		0x04	//El buffer no tiene el tamaño suficiente
#define STATUS_INT_ERR		0x05	//Error en el codigo
#define STATUS_INVALID		0x06	//Argumento invalido
#define STATUS_CRC_WRONG	0x07	//El CRC_A no concuerda
#define STATUS_MIFARE_NACK	0xFF	//El PICC reponde con NAK

typedef struct {
	uint8_t size;			//Numero de bytes en el UID
	uint8_t uidByte [10];	//Buffer donde se cargara el UID
	uint8_t sak;
}uid_t;

//---------------------------------------------------------------//
// Prototipos
//---------------------------------------------------------------//

void config_spi (void);
void clear_buff (void);
uint8_t MFRC522_Rd (uint8_t addr);
void MFRC522_Rd_Buff (uint8_t addr, uint8_t cant, unsigned char *values);
void MFRC522_Wr (uint8_t addr, uint8_t value);
void MFRC522_Wr_Buff (uint8_t addr, uint8_t cant, uint8_t *value);
void MFRC522_Clear (char addr, char mask);
void MFRC522_Set (char addr, char mask);
void MFRC522_Reset (void);
void MFRC522_AntenaOn (void);
void MFRC522_AntenaOff (void);
void MFRC522_Init (void);
unsigned char MFRC522_CALC_CRC (unsigned char *data, unsigned char length, unsigned char *result);
unsigned char MFRC522_Request (unsigned char *bufferATQA, unsigned char *bufferSize);
unsigned char MFRC522_TrasceiveData (unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned char *backLen, unsigned char *validBits, unsigned char rxAlign, bool checkCRC);
unsigned char MFRC522_CommuncicateWithPICC (unsigned char command, unsigned char waitIrq, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned char *backLen, unsigned char *validBits, unsigned char rxAlign, bool checkCRC);
uint8_t MFRC522_PICC_Select (uid_t *uid, uint8_t validBits);
unsigned char MFRC522_HaltA (void);
void MFRC522_StopCrypto (void);
bool MFRC522_IsNewCardPresent (void);
bool MFRC522_ReadCardSerial (uid_t *uid);
void MFRC522_AutoTest (void);

#endif /* RFID_H_ */
