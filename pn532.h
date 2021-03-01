#ifndef _PN532_H_
#define _PN532_H_

#include <stdint.h>
#include <memory.h>
#include "interface.h"
#include "misc.h"
#include "mifare.h"
#include <fstream>



const uint8_t PN532_I2C = 0x48 >> 1;

const uint8_t PREAMBLE = 0x00;
const uint8_t START1 = 0x00;
const uint8_t START2 = 0xFF;
const uint8_t POSTAMBLE = 0x00;

const uint8_t Diagnose_CMD = 0x00;
const uint8_t GetFirmwareVersion_CMD = 0x02;
const uint8_t GetGeneralStatus_CMD = 0x04;
const uint8_t ReadRegister_CMD = 0x06;
const uint8_t WriteRegister_CMD = 0x08;
const uint8_t ReadGPIO_CMD = 0x0C;
const uint8_t WriteGPIO_CMD = 0x0E;
const uint8_t SetSerialBaudRate_CMD = 0x10;
const uint8_t SetParameters_CMD = 0x12;
const uint8_t SAMConfiguration_CMD = 0x14;
const uint8_t PowerDown_CMD = 0x16;

const uint8_t RFConfiguration_CMD = 0x32;
const uint8_t RFRegulationTest_CMD = 0x58;

const uint8_t InJumpForDEP_CMD = 0x56;
const uint8_t InJumpForPSL_CMD = 0x46;
const uint8_t InListPassiveTarget_CMD = 0x4A;
const uint8_t InATR_CMD = 0x50;
const uint8_t InPSL_CMD = 0x4E;
const uint8_t InDataExchange_CMD = 0x40;
const uint8_t InCommunicateThru_CMD = 0x42;
const uint8_t InDeselect_CMD = 0x44;
const uint8_t InRelease_CMD = 0x52;
const uint8_t InSelect_CMD = 0x54;
const uint8_t InAutoPoll_CMD = 0x60;

const uint8_t TgInitAsTarget_CMD = 0x8C;
const uint8_t TgSetGeneralBytes_CMD = 0x92;
const uint8_t TgGetData_CMD = 0x86;
const uint8_t TgSetData_CMD = 0x8E;
const uint8_t TgSetMetaData_CMD = 0x94;
const uint8_t TgGetInitiatorCommand_CMD = 0x88;
const uint8_t TgResponseToInitiator_CMD = 0x90;
const uint8_t TgGetTargetStatus_CMD = 0x8A;

const uint8_t PN532_ACK[6] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
const uint8_t PN532_NACK[6] = {0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00};

/*
Communication frames: (example)

Preamble 	0x00
Startcode	0x00
			0xFF
Length		0x04
Length CS	0x1C	"Negates" Length
TFI			0xD4	D4 host to PN532, D5 other way
Data		0x03	First is cmd code
			0x01
			0x02
Data CS		0x06	"Negates" TFI + sum of data
Postamble 	0x00

poly 0x8408
0x6363

"Useful data" starts at the cmd code.

*/
class MIFARE_1K;

class PN532 {
	public:
		PN532(interface* _port);
		
		void toggleDebug();
		
		void getFirmwareVersion();
		bool SAMConfig();
				
		bool setMuteTimeout(uint8_t timeout);
		bool setCommunicationRetries(uint8_t retries);
		bool setPassiveActivationRetries(uint8_t retries);
		
		bool detectMifare1K(uint8_t uid[4]);
		bool select(uint8_t tag);
		
		bool MifareClassic_AuthenticateBlock(uint8_t block, uint8_t uid[4], bool keyType, uint8_t key[6]);
		bool MifareClassic_ReadBlock(uint8_t block, uint8_t* destination);		
		bool MifareClassic_WriteBlock(uint8_t block, uint8_t data[16]);

		void dumpFactoryMifare();
		bool readMifare(uint8_t destination[0x40][0x10], uint8_t keys[0x10][0x06]);
		
		void skyInfo();
		
		bool loadMagicMifare(const char* filename, uint8_t keys[0x10][0x06]);
		
	private:
		interface* port;
		uint8_t frameBuffer[64];
		bool debug;
		
		bool checkAck();
		
		bool writeCommand(uint8_t len);
		bool readData(uint8_t len);
		bool decodeError(uint8_t error);



};










#endif
