#ifndef _SKYLANDER_H_
#define _SKYLANDER_H_

#include "mifare.h"
#include <stdint.h>
#include "misc.h"
#include "md5.h"
#include "AES.h"
#include "CRC.h"
#include "toynames.h"


class Skylander : public MIFARE_1K {

	public:
		Skylander(const char* filename, PN532* nfc);		
		Skylander(PN532* nfc);
		Skylander(PN532* nfc, bool isMagic);
		
		void printInfo();
		
		void readSectorZero();
		
		bool loadBackup(const char* filename);

		bool decrypt();
		bool encrypt();
		
		void calcKeysA();
		
		void setCharacter(uint16_t charCode, uint16_t typeCode);
		
		void updateChecksums();
		
		uint16_t getCharCode();
		uint16_t getTypeCode();
		uint16_t getGold();
		uint32_t getXP();
		
		bool setXP(uint32_t XP);
		bool setLevel(uint8_t level);
		
		uint8_t XPtoLevel(uint32_t XP);
		uint32_t levelToXP(uint8_t level);
		
		
		uint16_t getPlaytime();
		void getFirstPlayed(uint8_t destination[6]);
		void getLastPlayed(uint8_t destination[6]);

		
		
		bool getName();
		

		
		
	protected:
		bool encrypted;
		uint8_t saveBlock;
		std::string Name;
		
		bool shouldEncryptBlock(uint8_t block);
		void calcAESKey(uint8_t destination[16], uint8_t block);
		void decryptBlock(uint8_t block);
		void encryptBlock(uint8_t block);
		
		void calcKeyA(uint8_t destination[6], uint8_t sector);

		bool checksum(uint8_t type, uint8_t area);
		
		uint8_t areaBlock(uint8_t area);
		void getArea();
		
		void getEncryption();
		

};

#endif