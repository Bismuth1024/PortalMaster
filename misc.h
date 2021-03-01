//A bunch of miscellaneous helpful functions.
#ifndef _MISC_H_
#define _MISC_H_

#include <stdint.h>
#include <memory.h>
#include <stdio.h>
#include <fstream>



void swapEndian(uint8_t* data, int len);
void printHexBytes(uint8_t* data, int len);
void printHexBytes(uint8_t* data, int len, bool format);

void textToBytes(const char* text, uint8_t* destination, int nBytes);
uint8_t charToHex(unsigned char input);
bool inRange(uint64_t x, uint64_t lower, uint64_t upper);
uint64_t bytesToInt(uint8_t* data, uint8_t len);
void compareBytes(uint8_t array1[], uint8_t array2[], int nBytes);
void readFile(const char* filename, uint8_t destination[], int nBytes);
void writeFile(const char* filename, uint8_t data[], int nBytes);

uint8_t inline readBit(uint8_t x, uint8_t n) {
	return ((x >> n) & 0x01);
}

void inline setBit(uint8_t* x, uint8_t n) {
	*x |= (0x01 << n);
}

void inline clearBit(uint8_t* x, uint8_t n) {
	*x &= ~(0x01 << n);
}
void inline littleEndian(int x, uint8_t nBytes, uint8_t* destination) {
	memcpy(destination, &x, nBytes);
}



#endif