#ifndef _MIFARE_H_
#define _MIFARE_H_

#include "misc.h"
#include "pn532.h"
#include <memory.h>
#include <stdio.h>
#include <stdint.h>
#include <fstream>

class PN532;


class MIFARE_1K {
	public:
		MIFARE_1K(PN532* _nfc);
		MIFARE_1K(uint8_t _keysA[0x10][0x06], PN532* _nfc);
		MIFARE_1K(const char* filename, PN532* _nfc);
		
		void magic();
		
		void dump();
		bool updateData();
		
		bool read();
		
		bool setBlock(uint8_t block, uint8_t in[0x10]);
		void getBlock(uint8_t block, uint8_t destination[0x10]);
		bool setKeyA(uint8_t sector, uint8_t key[6]);
		void getKeyA(uint8_t sector, uint8_t destination[6]);

		bool changeUID(uint8_t uid[4]);
		bool changeBlockZero(uint8_t in[0x0B]);
		bool wipe();
		bool wipe(bool preserve);
		void wipeSector(uint8_t sector);
		
		void makeFile(const char* filename);
		
	protected:
		PN532* nfc;
		bool isMagic;
	
		uint8_t keysA[0x10][0x06];
		uint8_t keysB[0x10][0x06];
		uint8_t accessBits[0x10][0x03];
		uint8_t UID[4];
		
		bool altered[0x40];
		
		uint8_t data[0x40][0x10];
		
		void setDefault();
		void paramsToData();
		void dataToParams();
		void calcBCC();
		
		bool flag(uint8_t block);









};

bool isTrailerBlock(uint8_t block);
bool isFirstBlock(uint8_t block);
uint8_t blockToSector(uint8_t block);
uint8_t sectorToBlock(uint8_t sector);





#endif