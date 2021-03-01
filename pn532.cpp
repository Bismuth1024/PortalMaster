#include "pn532.h"


PN532::PN532(interface* _port) : port(_port), debug(false) { }

/*

Description: Toggles debugging messages

Arguments:

Returns: 

*/

void PN532::toggleDebug() {
	debug = !debug;
}

/*

Description: Gets the firmware version of the PN532.  Mainly used to check communication.

Arguments:

Returns:

*/

void PN532::getFirmwareVersion() {
	frameBuffer[0] = GetFirmwareVersion_CMD;
	
	if (!writeCommand(1)) {
		return;
	}
		
	if (!readData(5)) {
		return;
	}
	
	printf("A PN5%x was found, version %i.%i.\n", frameBuffer[1], frameBuffer[2], frameBuffer[3]);
	printf("Supports: %x\n", frameBuffer[4]);
}

/*

Description: Configures the SAM to communicate with cards (must be done before any r/w).

Arguments:

Returns: Success boolean

*/

bool PN532::SAMConfig() {

	frameBuffer[0] = SAMConfiguration_CMD;
	frameBuffer[1] = 0x01;
	frameBuffer[2] = 0x14;
	frameBuffer[3] = 0x01;
	
	if (!writeCommand(4)) return false;
	
	return readData(1);
}

/*

Description: Sets how long the PN532 will wait for a card response.

Arguments:	Timeout value (see datasheet for conversions)

Returns: Success boolean

*/


bool PN532::setMuteTimeout(uint8_t timeout) {

	if (timeout > 0x10) timeout = 0x10;
		
	frameBuffer[0] = RFConfiguration_CMD;
	frameBuffer[1] = 0x02; //Config Item
	frameBuffer[2] = 0x00; //RFU
	frameBuffer[3] = 0x0B; //Dont alter
	frameBuffer[4] = timeout;
	
	if (!writeCommand(5)) {
		return false;
	}

	return readData(1);
}

/*

Description: Sets how many times the PN532 will attempt to communicate with target before giving up.

Arguments:	Number of retries (NOT ATTEMPTS)

Returns: Success boolean

*/

bool PN532::setCommunicationRetries(uint8_t retries) {
		
	frameBuffer[0] = RFConfiguration_CMD;
	frameBuffer[1] = 0x04; //Config Item
	frameBuffer[2] = retries; 
	
	if (!writeCommand(3)) {
		return false;
	}

	return readData(1);
}

/*

Description: Sets how many times the PN532 will attempt to active passive targets.

Arguments:	Number of retries (NOT ATTEMPTS)

Returns: Success boolean

*/

bool PN532::setPassiveActivationRetries(uint8_t retries) {
		
	frameBuffer[0] = RFConfiguration_CMD;
	frameBuffer[1] = 0x05; //Config Item
	frameBuffer[2] = 0xFF; //Dont alter
	frameBuffer[3] = 0x01; //Dont alter
	frameBuffer[4] = retries;
	
	if (!writeCommand(5)) {
		return false;
	}

	return readData(1);
}

/*

Description: Detects a MIFARE 1K card in the RF field and saves its UID.

Arguments:	uid - destination for the UID.

Returns: Success boolean

*/

bool PN532::detectMifare1K(uint8_t uid[4]) {
	
	frameBuffer[0] = InListPassiveTarget_CMD;
	frameBuffer[1] = 0x01; //1 Tag
	frameBuffer[2] = 0x00; //106 kbps type A
	
	if (!writeCommand(3)) return false;
	
	if (!readData(11)) return false;
	
	//Currently only supports one tag
	if (frameBuffer[1] != 0x01) return false;
	
	memcpy(uid, frameBuffer + 7, 4);
	
	printf("Found a tag with UID ");
	printHexBytes(uid, 4);
	printf("ATQA of %02X %02X, SAK of %02X\n", frameBuffer[3], frameBuffer[4], frameBuffer[5]);
	
	return true;
}

/*

Description: Selects a target.

Arguments:	Tag number (as defined by PN532)

Returns: Success boolean

*/

bool PN532::select(uint8_t tag) {

	frameBuffer[0] = InSelect_CMD;
	frameBuffer[1] = tag;
	
	if (!writeCommand(2)) return false;
	
	if (!readData(2)) return false;
	
	return decodeError(frameBuffer[1]);
}

/*

Description: Authenticates a sector for a MIFARE Classic 1K card.

Arguments:	block - The block (NOT SECTOR) to authenticate (but it will authenticate the whole sector)
			uid - The UID of the target to authenticate
			keyType - Which authentication type: Key A is true, Key B is false
			key - The key to use

Returns: Success boolean

*/

bool PN532::MifareClassic_AuthenticateBlock(uint8_t block, uint8_t uid[4], bool keyType, uint8_t key[6]) {
	
	//True is for keyA, false is for keyB
	frameBuffer[0] = InDataExchange_CMD;
	frameBuffer[1] = 0x01; //Tag Number
	frameBuffer[2] = (keyType ? 0x60 : 0x61); //Key A or Key B Auth
	frameBuffer[3] = block;
	memcpy(frameBuffer + 4, key, 6);
	memcpy(frameBuffer + 10, uid, 4);
	
	if (!writeCommand(14)) return false;
	
	if (!readData(2)) return false;
	
	return decodeError(frameBuffer[1]);
}

/*

Description: Reads a block for for a MIFARE Classic 1K card.

Arguments:	block - Number of the block to be read
			destination - Destination for the data read

Returns: Success boolean

*/

bool PN532::MifareClassic_ReadBlock(uint8_t block, uint8_t* destination) {
	
	frameBuffer[0] = InDataExchange_CMD;
	frameBuffer[1] = 0x01; //Tag Number
	frameBuffer[2] = 0x30; //Mifare read
	frameBuffer[3] = block;
	
	if (!writeCommand(4)) return false;
	
	if (!readData(18)) return false;
		
	memcpy(destination, frameBuffer + 2, 16);
	
	return decodeError(frameBuffer[1]);
}

/*

Description: Writes a block for a MIFARE Classic 1K card.

Arguments:	block - Number of the block to be written
			data - The data to write

Returns: Success boolean

*/

bool PN532::MifareClassic_WriteBlock(uint8_t block, uint8_t data[16]) {

	frameBuffer[0] = InDataExchange_CMD;
	frameBuffer[1] = 0x01;
	frameBuffer[2] = 0xA0; //Mifare write
	frameBuffer[3] = block;
	
	memcpy(frameBuffer + 4, data, 16);
	
	if (!writeCommand(20)) return false;
	
	if (!readData(2)) return false;

	return decodeError(frameBuffer[1]);
}

/*

Description: Reads an entire MIFARE Classic card (given the keys)

Arguments:	destination - Destination for the read data.
			keys - Keys to use.

Returns: Success boolean

*/

bool PN532::readMifare(uint8_t destination[0x40][0x10], uint8_t keys[0x10][0x06]) {
	uint8_t uid[4];
	
	if (!detectMifare1K(uid)) return false;
	if (!select(0x01)) return false;
	
	for (uint8_t block = 0; block < 0x40; block++) {
		if (isFirstBlock(block)) {
			if (!MifareClassic_AuthenticateBlock(block, uid, true, keys[block/4])) return false;
		}

		if (!MifareClassic_ReadBlock(block, destination[block])) return false;	
		
		if (isTrailerBlock(block)) {
			//since keyA not readable, the card will return all zeroes so we must fill in the real data
			memcpy(destination[block], keys[block/4], 0x06);
		}
	}
	
	return true;
}

/*

Description: Attempts to read an acknowledge frame from the PN532

Arguments:	

Returns: Success boolean

*/

bool PN532::checkAck() {
	static uint8_t ackbuff[7];

	if (!port->receiveI2C(PN532_I2C, ackbuff, 7)) return false;
	
	if (debug) {
		if (memcmp(ackbuff + 1, PN532_ACK, 6) == 0) {
			printf("PN532 acknowledged.\n");
		} else {
			printf("Incorrect ACK received.\n");
		}
	}
	
	return (memcmp(ackbuff + 1, PN532_ACK, 6) == 0);
}

/*

Description: Sends a command (TFI to PDN) to the PN532 (the command is in the frameBuffer)

Arguments:	len - How many bytes are in the command

Returns: Success boolean - If the PN532 ack'd or not

*/

bool PN532::writeCommand(uint8_t len) {

	static uint8_t cmdBuffer[64];

	cmdBuffer[0] = PREAMBLE;
	cmdBuffer[1] = START1;
	cmdBuffer[2] = START2;
	cmdBuffer[3] = len + 1; //TFI to PDn
	cmdBuffer[4] = ~(cmdBuffer[3]) + 1; //LCS
	cmdBuffer[5] = 0xD4; //Direction
	memcpy(cmdBuffer + 6, frameBuffer, len);
	
	uint8_t dataChecksum = 0xD4;

	for (uint8_t i = 0; i < len; i++) {
		dataChecksum += cmdBuffer[6 + i];
	}
	
	dataChecksum = ~dataChecksum;
	dataChecksum++;
	
	cmdBuffer[6 + len] = dataChecksum;
	cmdBuffer[7 + len] = POSTAMBLE;
	
	if (debug) {
		printf("\nSending the following command to PN532: ");
		printHexBytes(frameBuffer, len);
	}
	
	if (!port->sendI2C(PN532_I2C, cmdBuffer, len + 8)) return false;
	
	return checkAck();
}

/*

Description: Reads a response frame from the PN532 and stores TFI to PDN in the frameBuffer.

Arguments:	len - The number of bytes to read

Returns: Success boolean

*/

bool PN532::readData(uint8_t len) {
	static uint8_t responseBuffer[64];
	
	//Leading 0x01 if ready for I2C!
	if (!port->receiveI2C(PN532_I2C, responseBuffer, len + 8)) return false;
	
	if (debug) {
		printf("Received this data from the PN532: ");
		printHexBytes(responseBuffer + 7, len);
	}
	
	if (responseBuffer[6] != 0xD5) {
		//If this is not the TFI, then its probably an error
		decodeError(responseBuffer[6]);
		return false;
	}
	
	memcpy(frameBuffer, responseBuffer + 7, len);
	
	return true;
}


bool PN532::loadMagicMifare(const char* filename, uint8_t keys[0x10][0x06]) {
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	char buffer[0x400];
	uint8_t data[0x40][0x10];
	file.read(buffer, 0x400);
	file.close();
	
	uint8_t uid[4];
	detectMifare1K(uid);
	select(0x01);
	
	memcpy(data, buffer, 0x400);

	for (uint8_t block = 0; block < 0x40; block++) {
		if (isFirstBlock(block)) {
			if (!MifareClassic_AuthenticateBlock(block, uid, true, keys[block/4])) return false;
		}
		MifareClassic_WriteBlock(block, data[block]);
		
	}
	
	return true;
}

bool PN532::decodeError(uint8_t error) {
	if (error == 0x00) return true;

	printf("Error %02x:", error);
	
	switch (error) {
		case 0x01:
			printf("Time Out, the target has not answered\n");
			break;
		case 0x02:
			printf("A CRC error has been detected by the CIU");
			break;
		case 0x03:
			printf("A Parity error has been detected by the CIU\n");
			break;
		case 0x04:
			printf("During an anti-collision/select operation (ISO/IEC14443-3 Type A and ISO/IEC18092 106 kbps passive mode), an erroneous Bit Count has been detected\n");
			break;
		case 0x05:
			printf("Framing error during Mifare operation\n");
			break;
		case 0x06:
			printf("An abnormal bit-collision has been detected during bit wise anti-collision at 106 kbps\n");
			break;
		case 0x07:
			printf("Communication buffer size insufficient\n");
			break;
		case 0x09:
			printf("RF Buffer overflow has been detected by the CIU (bit BufferOvfl of the register CIU_Error)\n");
			break;
		case 0x0A:
			printf("In active communication mode, the RF field has not been switched on in time by the counterpart (as defined in NFCIP-1 standard)\n");
			break;
		case 0x0B:
			printf("RF Protocol error (cf. Error! Reference source not found., description of the CIU_Error register)\n");
			break;
		case 0x0D:
			printf("Temperature error: the internal temperature sensor has detected overheating, and therefore has automatically switched off the antenna drivers\n");
			break;
		case 0x0E:
			printf("Internal buffer overflow\n");
			break;
		case 0x10:
			printf("Invalid parameter (range, format, ...)\n");
			break;
		case 0x12:
			printf("DEP Protocol: The PN532 configured in target mode does not support the command received from the initiator (the command received is not one of the following: ATR_REQ, WUP_REQ, PSL_REQ, DEP_REQ, DSL_REQ, RLS_REQ)\n");
			break;
		case 0x13:
			printf("DEP Protocol, Mifare or ISO/IEC14443-4: The data format does not match to the specification.  Depending on the RF protocol used, it can be: Bad length of RF received frame, Incorrect value of PCB or PFB, Invalid or unexpected RF received frame, NAD or DIDincoherence.\n");
			break;
		case 0x14:
			printf("MIFARE Authentication error.\n");
			break;
		case 0x23:
			printf("\n");
			break;
		case 0x25:
			printf("\n");
			break;
		case 0x26:
			printf("\n");
			break;
		case 0x27:
			printf("\n");
			break;
		case 0x29:
			printf("\n");
			break;
		case 0x2A:
			printf("\n");
			break;
		case 0x2B:
			printf("\n");
			break;
		case 0x2C:
			printf("\n");
			break;
		case 0x2D:
			printf("\n");
			break;
		case 0x2E:
			printf("\n");
			break;
		

	}
	
	return false;
}