#include "interface.h"


interface::interface(const char* pname) : debug(false) {
	strcpy(portName, pname);
	memset(buffer, 0, 64);
}
	
int interface::begin(int baud) {
		
	debug = false;
	
	//Convert baudrate
	if (convertBaud(baud) == -1) {
		printf("Error setting baud rate: invalid baud rate.\n");
		return -1;
	} else {
		baudrate = convertBaud(baud);
	}
	

	int port = open(portName, O_RDWR); //Open port
	
	if (port < 0) { //Handle errors
   		printf("Error %i from open: %s\n", errno, strerror(errno));
	}
	
	struct termios tty; //For configuring port
	
	if(tcgetattr(port, &tty) != 0) { //Handle errors
    	printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
	}
	
	tty.c_cflag &= ~PARENB; //Clear parity bit
	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication
	tty.c_cflag &= ~CSIZE; // Clear all the size bits before setting
	tty.c_cflag |= CS8; // 8 bits per byte
	tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
	tty.c_lflag &= ~ICANON; //Disable canonical mode
	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); //Receive raw data
	
	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	
	tty.c_cc[VTIME] = 20;    // Wait for up to 2s (20 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;
	
	cfsetispeed(&tty, baudrate); //Set baud rate
	cfsetospeed(&tty, baudrate);
	
	if (tcsetattr(port, TCSANOW, &tty) != 0) { //Save and handle errors
    	printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
	}
	
	ID = port;
	
	return ID;
}


int interface::convertBaud(int baud)
{
    switch (baud) {
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    default: 
        return -1;
    }
}

bool interface::sendI2C(uint8_t i2caddr, uint8_t data[], uint8_t len) {
	
	buffer[0] = 0x00;
	buffer[1] = 0xff;
	buffer[2] = len + 3;
	buffer[3] = ~(buffer[2]) + 1;
	buffer[4] = len;
	buffer[5] = i2caddr;
	buffer[6] = 0x00;
	memcpy(buffer + 7, data, len);
	
	if (debug) {
		printf("\t\tSending %i bytes to I2C address %02X: ", len, i2caddr);
		printHexBytes(data, len);
	}
	
	return sendAck(buffer, len + 7);
	
}

bool interface::receiveI2C(uint8_t i2caddr, uint8_t* data, uint8_t len) {
	
	buffer[0] = 0x00;
	buffer[1] = 0xff;
	buffer[2] = 3;
	buffer[3] = ~(buffer[2]) + 1;
	buffer[4] = len;
	buffer[5] = i2caddr;
	buffer[6] = 0x01;


	if (debug) {
		printf("\t\tRequesting %i bytes from I2C address %02X\n", len, i2caddr);
	}
	
	if (!sendAck(buffer, 7)) {
		return false;
	}
	
	receive(data, len);
	return true;
}



void interface::toggleDebug() {
	debug = !debug;
}

int interface::send(uint8_t* data, int len) {
	write(ID, data, len);
	if (debug) {
		printf("\t\tSent %i bytes: ", len);
		printHexBytes(data, len);
	}
	return 0;
}

bool interface::sendAck(uint8_t* data, int len) {
	uint8_t response;
	
	write(ID, data, len);
	
	if (debug) {
		printf("\t\tSent %i bytes: ", len);
		printHexBytes(data, len);
	}
	
	receive(&response, 1);
	if (response == ACK) {
		return true;
	} else {
		if (debug) {
			printf("\t\tArduino did not ACK.\n");
		}
		return false;
	}
	
}

int interface::receive(uint8_t* destination, int len) {
	int n = read(ID, destination, len);
	if (debug) {
		printf("\t\tReceived %i bytes: ", len);
		printHexBytes(destination, len);
	}
	
	return n;
}


