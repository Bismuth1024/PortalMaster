#include "mifare.h"

const uint8_t defaultZero[0x0B] = {0x08, 0x04, 0x00, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69};

MIFARE_1K::MIFARE_1K(PN532* _nfc) : nfc(_nfc), isMagic(false) {
	nfc->detectMifare1K(UID);
	
	//Puts in all the default values for a factory chip
	setDefault();
	memset(altered, 0x00, 0x40);
}

MIFARE_1K::MIFARE_1K(uint8_t _keysA[0x10][0x06], PN532* _nfc) : nfc(_nfc), isMagic(false) {
	//Reads in all the data using the keys
	nfc->detectMifare1K(UID);
	memcpy(keysA, _keysA, 0x60);
	read();
	
	//Puts in all the stuff into seperate variables
	dataToParams();
	memset(altered, 0x00, 0x40);

}

MIFARE_1K::MIFARE_1K(const char* filename, PN532* _nfc) : nfc(_nfc), isMagic(false) {
	readFile(filename, &data[0][0], 0x400);
	dataToParams();
	memset(altered, 0x00, 0x40);

}

/*

Description: Flags the card as being a magic card, allowing writing to block zero.

Arguments:	

Returns: 

*/

void MIFARE_1K::magic() {
	isMagic = true;
}

/*

Description: Writes all the 1kB of data to a file.

Arguments:	filename - Name of the file.

Returns: 

*/

void MIFARE_1K::makeFile(const char* filename) {
	writeFile(filename, &data[0][0], 0x400);
}

/*

Description: Prints the entire contents of the card for human viewing.

Arguments:	

Returns: 

*/

void MIFARE_1K::dump() {
	printHexBytes(data[0], 0x400, true);
}

/*

Description: Wipes all of the data on the card (automatically done to physical card)

Arguments:	

Returns: Success boolean 

*/

bool MIFARE_1K::wipe() {
	for (uint8_t sector = 0x00; sector < 0x10; sector++) {
		wipeSector(sector);
	}
	return updateData();
}

bool MIFARE_1K::wipe(bool preserve) {
	if (preserve) {
		for (uint8_t sector = 0x01; sector < 0x10; sector++) {
			wipeSector(sector);
		}
	return updateData();
	} else {
		return wipe();
	}
}

/*

Description: Fills the data blocks of a sector with zeroes.

Arguments:	sector - which sector to wipe

Returns: 

*/

void MIFARE_1K::wipeSector(uint8_t sector) {

	//Don't wipe block zero
	if (sector == 0x00) {
		memset(data[1], 0x00, 0x20);
		memset(altered + 1, 0xff, 0x02);
	} else {
		uint8_t block = sectorToBlock(sector) - 3;
		memset(data[block], 0x00, 0x30);
		memset(&altered[block], 0xff, 0x03);
	}
}

/*

Description: Sets a block of the card's data (not sector trailer), and flags it as altered.

Arguments:	block - which block to change
			in - what data to write

Returns: Success boolean

*/

bool MIFARE_1K::setBlock(uint8_t block, uint8_t in[0x10]) {
	if (isTrailerBlock(block)) return false;
	if (block == 0) return false; //This should be done with the special function for magic cards.
	
	memcpy(data[block], in, 0x10);
	flag(block);
	return true;
}

/*

Description: Flags a block as being altered (to inform the updateData function)

Arguments:	block - which block to flag

Returns: Success boolean

*/

bool MIFARE_1K::flag(uint8_t block) {
	if (isTrailerBlock(block)) return false;
	if (block == 0) return false; //This should be done with the special function for magic cards.

	altered[block] = true;
	return true;
}

/*

Description: Copies a block of data from the card into a destination byte array

Arguments:	block - which block to get
			destination - destination for data

Returns: 

*/

void MIFARE_1K::getBlock(uint8_t block, uint8_t destination[0x10]) {
	memcpy(destination, data[block], 0x10);
}

/*

Description: Reads the entire contents of the card using the keys on the object.

Arguments:	

Returns: Success boolean

*/

bool MIFARE_1K::read() {
	if (!nfc->select(0x01)) return false;
	
	for (uint8_t block = 0; block < 0x40; block++) {
		if (isFirstBlock(block)) {
			if (!nfc->MifareClassic_AuthenticateBlock(block, UID, true, keysA[block/4])) return false;
		}

		if (!nfc->MifareClassic_ReadBlock(block, data[block])) return false;	
		
		if (isTrailerBlock(block)) {
			//since keyA not readable, the card will return all zeroes so we must fill in the real data
			memcpy(data[block], keysA[block/4], 0x06);
		}
	}
	
	return true;
}

/*

Description: Sets all the data and sector trailers to factory values.

Arguments:	

Returns:

*/

void MIFARE_1K::setDefault() {
	memset(&data[1][0], 0x00, 0x3F0);
	memset(keysA, 0xFF, 0x60);
	memset(keysB, 0xFF, 0x60);
	memcpy(data[0] + 0x05, defaultZero, 0x0B);
	
	for (uint8_t sector = 0; sector < 0x10; sector++) {
		accessBits[sector][0] = 0xFF;
		accessBits[sector][1] = 0x07;
		accessBits[sector][2] = 0x80;
		data[sector * 4 + 3][0x09] = 0x69;

	}
		
	paramsToData();
	
}

/*

Description: Gets the UID and sector trailers and saves them into the data.

Arguments:	

Returns: 

*/

void MIFARE_1K::paramsToData() {
	memcpy(data, UID, 4);
	calcBCC();
	
	uint8_t block;
	for (uint8_t sector = 0; sector < 0x10; sector++) {
		block = sectorToBlock(sector);
		memcpy(data[block], keysA[sector], 0x06);
		memcpy(data[block] + 0x06, accessBits[sector], 0x03);
		memcpy(data[block] + 0x0A, keysB[sector], 0x06);
	}
}

/*

Description: Gets the UID and sector trailers from the data and stores them in seperate variables for use.

Arguments:	

Returns: 

*/

void MIFARE_1K::dataToParams() {
	memcpy(UID, data, 4);
	calcBCC();
	
	uint8_t block;
	for (uint8_t sector = 0; sector < 0x10; sector++) {
		block = sectorToBlock(sector);
		memcpy(keysA[sector], data[block], 0x06);
		memcpy(accessBits[sector], data[block] + 0x06, 0x03);
		memcpy(keysB[sector], data[block] + 0x0A, 0x06);
	}
}

/*

Description: Updates the physical card with changes to its data on the MIFARE_1K object.  Sector trailers are not changed.

Arguments:	

Returns: Success boolean

*/

bool MIFARE_1K::updateData() {

	//We write to the actual card in this way so that authentication operations are minimised.
	//You shouldn't change keys this way; only data.  Also block zero should only be changed with the dedicated function.
	
	bool authenticated[0x10];
	uint8_t sector;
	memset(authenticated, 0x00, 0x10);
	
	for (uint8_t block = 0x01; block < 0x40; block++) {
		if (!isTrailerBlock(block)) {
			if (altered[block]) {
				sector = blockToSector(block);
				if (!authenticated[sector]) {
					if (!nfc->MifareClassic_AuthenticateBlock(block, UID, true, keysA[sector])) return false;
					authenticated[sector] = true;
				}
				if (!nfc->MifareClassic_WriteBlock(block, data[block])) return false;
			}
		}
		
	}
	
	memset(altered, 0x00, 0x40);
	return true;
}

/*

Description: Changes a key A on the card.

Arguments:	sector - which sector to change the key for
			key - new key

Returns: Success boolean

*/

bool MIFARE_1K::setKeyA(uint8_t sector, uint8_t key[6]) {
	uint8_t block = sectorToBlock(sector);

	memcpy(data[block], key, 0x06);
	memset(data[block] + 0x0A, 0x00, 0x06);
	
	if (!nfc->MifareClassic_AuthenticateBlock(block, UID, true, keysA[sector])) return false;

	if (!nfc->MifareClassic_WriteBlock(block, data[block])) return false;
	
	
	//This must be done AFTER the transaction with the card since the old key is used to authenticate first
	memcpy(keysA[sector], key, 0x06);
	
	return true;
}

/*

Description: Copies a key A into a byte array.

Arguments:	sector - which sector to get the key of 
			destination - destination for the key

Returns: 

*/

void MIFARE_1K::getKeyA(uint8_t sector, uint8_t destination[6]) {
	memcpy(destination, keysA[sector], 0x06);
}

/*

Description: Calculates the BCC for the card's UID and stores it in the relevant location.  Note that this uses block zero 0-3, not the UID variable.

Arguments:	

Returns: 

*/

void MIFARE_1K::calcBCC() {
	data[0][4] = data[0][0] ^ data[0][1] ^ data[0][2] ^ data[0][3];
}

/*

Description: Changes the UID of the card.  Only works with magic cards.

Arguments:	uid - New UID

Returns: 

*/

bool MIFARE_1K::changeUID(uint8_t uid[4]) {
	if (!isMagic) return false;
	
	if (!nfc->MifareClassic_AuthenticateBlock(0x00, UID, true, keysA[0])) return false;
	
	//Just a precautionary step to make sure we have all the proper values for the rest of block zero
	if (!nfc->MifareClassic_ReadBlock(0x00, data[0x00])) return false;
	
	memcpy(data, uid, 0x04);
	calcBCC();
	
	if (!nfc->MifareClassic_WriteBlock(0x00, data[0x00])) return false;
	
	//Again, this must be done after as the old UID is used for the transaction.
	memcpy(UID, uid, 0x04);
	return true;
}

/*

Description: Changes the non-UID/BCC bytes of block zero.  Only works with magic cards.

Arguments:	in - 0x0B bytes of new data

Returns: success boolean

*/

bool MIFARE_1K::changeBlockZero(uint8_t in[0x0B]) {
	if (!isMagic) return false;

	memcpy(&data[0][5], in, 0x0B);
	
	if (!nfc->MifareClassic_AuthenticateBlock(0x00, UID, true, keysA[0])) return false;
	if (!nfc->MifareClassic_WriteBlock(0x00, data[0x00])) return false;
	
	return true;
}



bool isTrailerBlock(uint8_t block) {
	return (block % 4 == 3);
}

bool isFirstBlock(uint8_t block) {
	return (block % 4 == 0);
}

uint8_t blockToSector(uint8_t block) {
	return (block - (block % 4))/4;
}

uint8_t sectorToBlock(uint8_t sector) {
	return (sector * 4 + 3);
}