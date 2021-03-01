#include "skylander.h"

CRC keycrc(0x30, 0x42f0e1eba9ea3693, 0x9ae903260cc4);  
CRC checkcrc(0x10, 0x1021, 0xffff);

uint8_t sectorZero[11] = {0x81, 0x01, 0x0f, 0xc4, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15};

uint32_t minXPforLevel[21] = {	
								0,
								0,
								1000,
								2200,
								3800,
								6000,
								9000,
								13000,
								18200,
								24800,
								33000,
								42700,
								53900,
								66600,
								80800,
								96500,
								113700,
								132400,
								152600,
								174300,
								197500
								};

struct dataInfo {
	uint8_t offset;
	uint8_t size;
};

dataInfo charCode = {0x10, 0x02};
dataInfo typeCode = {0x1C, 0x02};

dataInfo crc[5] = {
					{0x1E, 0x02},
					{0x0E, 0x02},
					{0x0C, 0x02},
					{0x0A, 0x02},
					{0x90, 0x02}	
				};	

dataInfo xp[3] = {
					{0x00, 0x03},
					{0x93, 0x02},
					{0x98, 0x03}
				};	

dataInfo gold = {0x03, 0x02};

dataInfo playtime = {0x05, 0x02};

dataInfo save = {0x09, 0x01};

dataInfo upgrades = {0x10, 0x02};

dataInfo platforms = {0x13, 0x01};

dataInfo hats[4] = {
					{0x14, 0x01},
					{0x95, 0x01},
					{0x9C, 0x01},
					{0x9E, 0x01}
				};	

dataInfo ownership = {0x18, 0x08};

dataInfo name[2] = {
					{0x20, 0x10},
					{0x40, 0x10},
				};	
				
dataInfo history[2] = {
					{0x50, 0x06},
					{0x60, 0x06},
				};	

Skylander::Skylander(const char* filename, PN532* _nfc) : MIFARE_1K(filename, _nfc), encrypted(true) {
	getEncryption();
	dataToParams();
}

Skylander::Skylander(PN532* _nfc) : MIFARE_1K(_nfc), encrypted(true) {
	calcKeysA();
	readSectorZero();
}

Skylander::Skylander(PN532* _nfc, bool _isMagic) : MIFARE_1K(_nfc), encrypted(false) {
	if (_isMagic) {
		magic();
		uint8_t key[6];
		for (uint8_t sector = 0; sector < 0x10; sector++) {
			calcKeyA(key, sector);
			
			setKeyA(sector, key);
		}
		changeBlockZero(sectorZero);
	}
}

void Skylander::readSectorZero() {
	nfc->MifareClassic_AuthenticateBlock(0x00, UID, true, keysA[0]);
	nfc->MifareClassic_ReadBlock(0x00, data[0]);
	printHexBytes(data[0], 0x10);
	nfc->MifareClassic_ReadBlock(0x01, data[1]);
	printHexBytes(data[1], 0x10);
}

bool Skylander::loadBackup(const char* filename) {
	uint8_t buffer[0x400];
	readFile(filename, buffer, 0x400);

	if (isMagic) {
		calcKeysA();
		changeUID(buffer);
		
		uint8_t newKey[0x06];
		for (uint8_t sector = 0; sector < 0x10; sector++) {
			calcKeyA(newKey, sector);
			setKeyA(sector, newKey);
		}
		
		uint8_t newBlockZero[0x0B];
		memcpy(newBlockZero, buffer + 0x05, 0x0B);
		changeBlockZero(newBlockZero);
	
	
		flag(0x01);
		flag(0x02);
	}


	if (memcmp(buffer, UID, 0x04)) return false;
	memcpy(data, buffer, 0x400);
	
	for (uint8_t block = 0x04; block < 0x40; block++) {
		if (!isTrailerBlock(block)) flag(block);
	}
	
	updateData();
	
	return true;
	
}

void Skylander::printInfo() {
	
	uint16_t CharCode = getCharCode();
	std::string CharName = getCharName(CharCode);
	uint16_t TypeCode = getTypeCode();
	
	printf("Character code: %04x\n", CharCode);
	printf("Character: %s\n", CharName.c_str());
	printf("Type Code: %04x\n", TypeCode);
	
	if (encrypted) {
		printf("The rest of the information is still encrypted.\n");
		return;
	}
	
	
	getArea();
	
	uint16_t Gold = getGold();
	uint32_t XP = getXP();
	uint8_t Level = XPtoLevel(XP);
	
	uint16_t Playtime = getPlaytime();
		
	getName();
	
	uint8_t FirstPlayed[6], LastPlayed[6];
	
	getFirstPlayed(FirstPlayed);
	getLastPlayed(LastPlayed);
	
	uint16_t YearFirst = bytesToInt(FirstPlayed + 0x04, 0x02);
	uint16_t YearLast = bytesToInt(LastPlayed + 0x04, 0x02);


	printf("Gold: %u\n", Gold);
	printf("XP: %u, level: %u\n", XP, Level);
	printf("Playtime: %i minutes %i seconds\n", (Playtime - Playtime % 60)/60, Playtime % 60);
	
	printf("Last Played: %02i/%02i/%04i %02i:%02i, First Played: %02i/%02i/%04i %02i:%02i\n",
	LastPlayed[2], LastPlayed[3], YearLast, LastPlayed[1], LastPlayed[0], FirstPlayed[2], FirstPlayed[3], YearFirst, FirstPlayed[1], FirstPlayed[0]);

	printf("Name: %s\n", Name.c_str());




}

void Skylander::setCharacter(uint16_t charCode, uint16_t typeCode) {
	
	littleEndian(charCode, 2, &data[1][0]);
	littleEndian(typeCode, 2, &data[1][0] + 0x0C);

	checksum(0, 1);
	flag(0x01);
	updateData();
	dump();


}



bool Skylander::decrypt() {

	if (!encrypted) return false;

	for (uint8_t block = 0; block < 0x40; block++) {
		if (shouldEncryptBlock(block)) {
			decryptBlock(block);
		}
	}
	
	encrypted = false;
	getArea();
	return true;
}

bool Skylander::encrypt() {

	if (encrypted) return false;
	
	for (uint8_t block = 0; block < 0x40; block++) {
		if (shouldEncryptBlock(block)) {
			encryptBlock(block);
		}
	}
	
	encrypted = true;
	return true;
}

bool Skylander::shouldEncryptBlock(uint8_t block) {
	return ((!isTrailerBlock(block)) && (inRange(block, 0x08, 0x15) || inRange(block, 0x24, 0x31)));
}

void Skylander::calcAESKey(uint8_t destination[16], uint8_t block) {

	//Key is MD5 hash of a constant, first two blocks, and block number
	uint8_t md5seed[0x56] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x20, 0x43, 0x6F, 0x70, 0x79, 0x72, 0x69, 0x67, 0x68, 0x74, 0x20, 0x28, 0x43, 0x29, 0x20, // 0x00 "Copyright (C) "
        0x32, 0x30, 0x31, 0x30, 0x20, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E, 0x2E, // "2010 Activision."
        0x20, 0x41, 0x6C, 0x6C, 0x20, 0x52, 0x69, 0x67, 0x68, 0x74, 0x73, 0x20, 0x52, 0x65, 0x73, 0x65, // " All Rights Rese"
        0x72, 0x76, 0x65, 0x64, 0x2E, 0x20}; // "rved. "

	memcpy(md5seed, data, 0x20);
	md5seed[0x20] = block;
	
	computeMD5(destination, md5seed, 0x56);
	return;
}

void Skylander::decryptBlock(uint8_t block) {
	AES aes(128);
	uint8_t key[16];
	calcAESKey(key, block);
	
	aes.DecryptECB(data[block], 16, key);
}

void Skylander::encryptBlock(uint8_t block) {
	AES aes(128);
	uint8_t key[16];
	calcAESKey(key, block);
	
	aes.EncryptECB(data[block], 16, key, 0);
}

void Skylander::calcKeyA(uint8_t destination[6], uint8_t sector) {
  if (sector == 0) {
    destination[0] = 0x4b;
    destination[1] = 0x0b;
    destination[2] = 0x20;
    destination[3] = 0x10;
    destination[4] = 0x7c;
    destination[5] = 0xcb;
    return;
  }

  uint8_t seed[5] = {UID[0], UID[1], UID[2], UID[3], sector};
  
  keycrc.compute(seed, 5, destination);
  swapEndian(destination, 6);
  
  return;
}

void Skylander::calcKeysA() {
	for (uint8_t sector = 0; sector < 0x10; sector++) {
		calcKeyA(keysA[sector], sector);
	}

	return;
}

uint8_t Skylander::areaBlock(uint8_t area) {
	return (area ? 0x24 : 0x08);
}

void Skylander::updateChecksums() {
	for (uint8_t area = 0; area <= 1; area++) { //Do for each data area
		for (uint8_t type = 0; type <= 4; type++) {//Do each type
			checksum(type, area);
		}
	}
}

bool Skylander::checksum(uint8_t type, uint8_t area) {
	
	
	uint8_t header, offset, *destination, src[0x200];	
	uint16_t nBytes;
	
	header = areaBlock(area);
	
	//Block and offset are used to find the storage destination variable
	//Src and nbytes specify what data to feed into the CRC
	//Type zero are absolute block/sector values, other types are relative to the two headers

	switch (type) {
		case 0:
			nBytes = 0x1E;
			memcpy(src, data, nBytes);
			break;
		case 1:
			nBytes = 0x10;
			memcpy(src, data[area], nBytes);
			src[0x0E] = 0x05;
			src[0x0F] = 0x00;
			
			break;
		case 2:
			nBytes = 0x30;
			memcpy(src, data[area + 1], 0x20);
			memcpy(src + 0x20, data[area + 4], 0x10);
			
			 break;
		case 3:
			nBytes = 0x110;
			memcpy(src, data[area + 5], 0x20);
			memcpy(src + 0x20, data[area + 8], 0x10);
			memset(src + 0x30, 0x00, 0xE0);
			break;
			
		case 4:
			nBytes = 0x40;
			memcpy(src, data[area + 0x09], 0x20);
			memcpy(src + 0x20, data[area + 0x0C], 0x20);
			src[0x00] = 0x06;
			src[0x01] = 0x01;
			break;
			
		default:
			return false;
	}
	
	destination = (type ? data[header] : data[0x00]) + crc[type].offset;
	
	//Checkcrc is defined at the top of the file
	checkcrc.compute(src, nBytes, destination);
	//All data is stored little endian
	swapEndian(destination, 2);
	
	if (block != 0x01) {
		flag(block);
	}
	
	return true;
}

void Skylander::getArea() {
	saveBlock = areaBlock(data[0x24][0x09] > data[0x08][0x09] ? 1 : 0);
}

uint32_t Skylander::getXP() {

	uint32_t XP = 0;
	for (uint8_t i = 0; i < 3; i++) {
		XP += bytesToInt(data[saveBlock] + xp[i].offset, xp[i].size);
	}
	return XP;	
}

bool Skylander::setXP(uint32_t XP) {
	if (XP <= 33000) {
		littleEndian(XP, xp[0].size, data[saveBlock] + xp[0].offset);
	} else {
		littleEndian(33000, xp[0].size, data[saveBlock] + xp[0].offset);
		XP -= 33000;
		if (XP <= 63500) {
			littleEndian(XP, xp[1].size, data[saveBlock] + xp[1].offset);
		} else {
			littleEndian(63500, xp[1].size, data[saveBlock] + xp[1].offset);
			XP -= 63500;
			if (XP > 0xffffff) return false;
			littleEndian(XP, xp[2].size, data[saveBlock] + xp[2].offset);
		}
	}
	updateChecksums();
	return true;
}

bool Skylander::setLevel(uint8_t level) {
	return setXP(levelToXP(level));
}

uint8_t Skylander::XPtoLevel(uint32_t XP) {
	for (uint8_t level = 1; level < 50; level++) {
		if (XP < minXPforLevel[level]) return (level - 1);
	}
	return 0;
}

uint32_t Skylander::levelToXP(uint8_t level) {
	return (minXPforLevel[level]);
}

uint16_t Skylander::getGold() {
	uint16_t Gold = bytesToInt(data[saveBlock] + gold.offset, gold.size);
	return Gold;
}

uint16_t Skylander::getPlaytime() {
	uint16_t Playtime = bytesToInt(data[saveBlock] + playtime.offset, playtime.size);
	return Playtime;
}

void Skylander::getLastPlayed(uint8_t* destination) {
	memcpy(destination, data[saveBlock] + history[0].offset, history[0].size);
}

void Skylander::getFirstPlayed(uint8_t* destination) {
	memcpy(destination, data[saveBlock] + history[1].offset, history[1].size);
}

uint16_t Skylander::getCharCode() {
	uint16_t CharCode = bytesToInt(data[0] + charCode.offset, charCode.size);
	return CharCode;
}

uint16_t Skylander::getTypeCode() {
	uint16_t TypeCode = bytesToInt(data[0] + typeCode.offset, typeCode.size);
	return TypeCode;
}

bool Skylander::getName() {
	Name.clear();
	uint16_t nextChar;
	
	for (uint8_t i = 0; i < name[0].size; i += 2) {
		nextChar = bytesToInt(data[0x08] + name[0].offset + i, 2);
		if (nextChar > 0x7f) return false;
		Name += (char)nextChar;
		if (nextChar == 0) return true;
	}
	
	for (uint8_t i = 0; i < name[1].size; i += 2) {
		nextChar = bytesToInt(data[0x08] + name[1].offset + i, 2);
		if (nextChar > 0x7f) return false;
		Name += (char)nextChar;
		if (nextChar == 0) return true;
	}
	return true;
}

void Skylander::getEncryption() {
	encrypted = !getName();
}
