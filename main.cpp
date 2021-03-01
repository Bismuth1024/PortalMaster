#include "AES.h"
#include "CRC.h"
#include "interface.h"
#include "md5.h"
#include "mifare.h"
#include "misc.h"
#include "pn532.h"
#include "skylander.h"
#include "toynames.h"

#include <stdio.h>
#include <stdint.h>
#include <getopt.h>

using namespace std; 

int main(int argc, char** argv) {
	interface port("/dev/cu.usbserial-AR0KL3OY");
	port.begin(115200);
	PN532 pn532(&port);
	loadNames();
	uint8_t dummy[4];
	
	int decrypt = 0;
	
	struct option longoptions[] = {
		{"encrypt", no_argument, &decrypt, 0},
		{"decrypt", no_argument, &decrypt, 1},
		{"setup", no_argument, 0, 's'},
		{"read", optional_argument, 0, 'r'},
		{"write", optional_argument, 0, 'w'},
		{"file", required_argument, 0, 'f'},
		{"view", no_argument, 0, 'v'},
		{"magic", no_argument, 0, 'm'},
		{"info", no_argument, 0, 'i'},
		{"prepare", no_argument, 0, 'p'},
		{"test", no_argument, 0, 't'},
		{"clone", no_argument, 0, 'c'},
		{"reset", no_argument, 0, 'R'},
		{"compare", required_argument, 0, 'C'},
		{"XP", optional_argument, 0, 'X'},
		{0,0,0,0}
		};
	
	char* filename;
	char* filename2;
	const char* legal_flags = "isrwvf:mptcRC:dDX";
	int optindex, opt;
	bool setup, read, write, file, view, magic, info, prepare, test, clone, reset, compare, XP, debugPort, debugPN532;
	setup = read = write = file = view = magic = info = prepare = test = clone = reset = compare = XP = debugPort = debugPN532 = false;

	while ((opt = getopt_long(argc, argv, legal_flags, longoptions, &optindex)) != -1) {
		
		switch (opt) {
			
			case 'd':
				debugPN532 = true;
				break;
			case 'D':
				debugPort = true;
				break;

			case 's':
				setup = true;
				break;
					
			case 'r':
				read = true;
				break;
				
			case 'w':
				write = true;
				break;
				
			case 'v':
				view = true;
				break;
				
			case 'f':
				file = true;
				filename = optarg;
				break;
				
			case 'm':
				magic = true;
				break;
				
			case 'i':
				info = true;
				break;
				
			case 'p':
				prepare = true;
				break;
				
			case 't':
				test = true;
				break;
				
			case 'c':
				clone = true;
				break;
				
			case 'R':
				reset = true;
				break;
				
			case 'C':
				compare = true;
				filename2 = optarg;
				break;
				
			case 'X':
				XP = true;
				break;
				
			case 0:
				break;
				
			default:
				printf(	"\n\n"
						"Usage:\n"
						"\t-s: Set up the PN532.  Use this at the start of every session.\n"
						"\t-t: Test if RF communication is working by detecting a nearby card.\n"
						"\t-e: Decrypt data when saving or printing.\n"
						"\t-r: Read a Skylander from the PN532 and print its contents.\n"
						"\t-c: Update the checksums on a skylander.\n"
						"\n"
						"\t-d: Enable debugging for the PN532.\n"
						"\t-D: Enable debugging for the Serial to I2C interface."
						"\n\n"
						);
		}
	}
	
	if (debugPort) {
		port.toggleDebug();
	}
	
	if (debugPN532) {
		pn532.toggleDebug();
	}
	
	if (setup) {
		pn532.getFirmwareVersion();
		pn532.SAMConfig();
		printf("Setup Complete.\n");
	}
	
	if (read) {
		Skylander skylander(&pn532);
		skylander.read();
		
		if (file) {
			if (decrypt) skylander.decrypt();
			skylander.makeFile(filename);
		} else {
		
		}
	}
	
	if (write) {
		if (file) {
			Skylander skylander(&pn532);
			if (magic) skylander.magic();
			skylander.loadBackup(filename);
		} else {
			
		}
	}
	
	if (view) {
		if (file) {
			Skylander skylander(filename, &pn532);
			if (decrypt) skylander.decrypt();
			skylander.dump();
			skylander.printInfo();
			
		} else {
			Skylander skylander(&pn532);
			skylander.read();
			if (decrypt) skylander.decrypt();
			skylander.dump();
			skylander.printInfo();
		}
	}
	
	if (info) {
		Skylander skylander(&pn532);

		uint8_t blockZero[0x10];
		skylander.getBlock(0x00, blockZero);
		
		char temp[10];
		
		std::ofstream file("ZeroInfo.txt", std::ios::out | std::ios::app | std::ios::ate);
		file << getCharName(skylander.getCharCode()) << ":\t";
		for (uint8_t i = 0; i < 0x10; i++) {
			snprintf(temp, 10, " %02X", blockZero[i]);
			file << temp;
		}
		
		file << "\n\n";
		file.close();
	}

	if (prepare) {
		Skylander skylander(&pn532, true);
	}

	if (test) {
		uint8_t sectorZero[11] = {0x81, 0x01, 0x0f, 0xc4, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14};
		Skylander skylander(&pn532);
		skylander.readSectorZero();
		skylander.changeBlockZero(sectorZero);

	}
	
	if (clone) {
		Skylander skylander(&pn532);
		skylander.read();
		skylander.makeFile("temp.bin");
		printf("Press enter when ready.\n");
		char c;
		scanf("%c", &c);
		Skylander skylander2(&pn532);
		skylander2.magic();
		skylander2.loadBackup("temp.bin");
		remove("temp.bin");
	}
	
	if (compare) {
		uint8_t file1[0x400], file2[0x400];
		readFile(filename, file1, 0x400);
		readFile(filename2, file2, 0x400);
		compareBytes(file1, file2, 0x400);
	}
	
	if (reset) {
		Skylander skylander(&pn532);
		skylander.wipe(true);
	}
	
}

/*
file to screen
file to figure
figure to screen
figure to file

05 B1 65 52 BF 0C C2 13 E0




*/


