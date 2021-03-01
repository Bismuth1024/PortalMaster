#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "misc.h"
#include <stdint.h>



#define ACK  0xaa
#define NACK 0x55
#define TEST 0xff

/*

I2C protocol: 
Host sends:
	Packet start marker
	Length of packet
	LCS
	number
	address
	r/w
	(data)
	end







*/


class interface {
	public:
		interface(const char* pname);
		
		int begin(int baud);
		
		void toggleDebug();

		bool sendI2C(uint8_t i2caddr, uint8_t data[], uint8_t len);
		bool receiveI2C(uint8_t i2caddr, uint8_t* data, uint8_t len);
		
		bool sendAck(uint8_t* data, int len);

		
	private:
		char portName[30];
		int ID;
		int baudrate;
		bool debug;
		uint8_t buffer[64];
		

		
		int send(uint8_t* data, int len);
		int receive(uint8_t* destination, int len);
		
		int convertBaud(int baud);
		
		bool sendKey(uint8_t key[6]);
};

#endif