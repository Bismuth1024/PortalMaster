#include "misc.h"

void swapEndian(uint8_t* data, int len) {
	uint8_t temp;
	for (int i = 0; i < len/2; i++) {
		temp = data[i]; //Hold byte
		data[i] = data[len - i - 1];
		data[len - i - 1] = temp;
	}
}

void printHexBytes(uint8_t* data, int len) {
	for (int i = 0; i < len; i++) {
		printf("%02hhX ", data[i]);
	}
	printf("\n\n");
}

void printHexBytes(uint8_t* data, int len, bool format) {
	for (int i = 0; i < len; i++) {
		if (i % 0x10 == 0x00) {
			printf("\n\nBlock 0x%02x: ", i >> 4);
			
		}
		printf("%02hhX ", data[i]);

	}
	printf("\n\n");
}

void textToBytes(const char* text, uint8_t* destination, int nBytes) {
	for (int i = 0; i < nBytes; i++) {
		destination[i] = (charToHex(text[2 * i]) << 4) + (charToHex(text[2 * i + 1]));
	}	

}

uint8_t charToHex(unsigned char input) {
	if (input >= 48 && input <= 57) {
		return (input - '0');
	} else if (input >= 65 && input <= 70) {
		return (input - 'A' + 10);
	} else if (input >= 97 && input <= 102) {
		return (input - 'a' + 10);
	} else {
		return 0;
	}
}

bool inRange(uint64_t x, uint64_t lower, uint64_t upper) {
	return (x >= lower && x <= upper);
}



uint64_t bytesToInt(uint8_t* data, uint8_t len) {
	uint64_t out = 0;
	uint64_t temp;
	for (uint8_t i = 0; i < len; i++) {
		out = out << 8;
		out |= data[len - i - 1];
	}

	return out;
	
}

void readFile(const char* filename, uint8_t destination[], int nBytes) {
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	
	char* buffer;
	buffer = (char *)malloc(nBytes);
	
	file.read(buffer, nBytes);
	file.close();
	
	memcpy(destination, buffer, nBytes);

	free(buffer);

}

void writeFile(const char* filename, uint8_t data[], int nBytes) {
	std::ofstream file(filename, std::ios::out | std::ios::binary);
	
	file.write((char *)data, nBytes);
	file.close();

}

void compareBytes(uint8_t* array1, uint8_t* array2, int nBytes) {
	printf("Data 1:\n");
	printHexBytes(array1, nBytes, true);
	
	printf("\n\n\nData 2:\n");
	printHexBytes(array2, nBytes, true);
	
	uint8_t* difference = (uint8_t *)malloc(nBytes);
	memset(difference, 0x00, nBytes);
	
	for (int i = 0; i < nBytes; i++) {
		if (array1[i] != array2[i]) {
			printf("Data differs at byte 0x%04X, Data 1 contains 0x%02X and Data 2 contains 0x%02X.\n", i, array1[i], array2[i]);
			difference[i] = 0xff;	
		}
	} 
	
	printf("\n\n\nDifferences:\n");
	printHexBytes(difference, nBytes, true);
	
	free(difference);
}



