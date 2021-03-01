#include "CRC.h"

CRC::CRC(uint8_t wdth, uint64_t poly, uint64_t init) : 
width(wdth), polynomial(poly), initial(init) {}

void CRC::compute(uint8_t* message, int nBytes, uint8_t* destination) {
  //computes the CRC of the message, which should be passed as a byte array.
  
  uint64_t trim = 0xffffffffffffffff >> (0x40 - width); //Trim to correct width
  uint64_t msbcheck = 0x8000000000000000 >> (0x40 - width); //And check at the right position
  uint64_t crc = initial;  //Initialise register

  for (int i = 0; i < nBytes; i++) {
    uint64_t byte64 = message[i]; //Expand size of byte to allow left shifts to work properly
    crc = crc ^ (byte64 << (width - 8)); //Extended to required length
    for (uint8_t k = 0; k < 8; k++) { //Do for each bit
      if (crc & msbcheck) { //If first bit is 1 do the XOR
        crc = (crc << 1) ^ polynomial;  
      } 
      else {
        crc = crc << 1;
      }
      crc = crc & trim; //Keep CRC in required length
    }
  }

  uint8_t* temp = (uint8_t*)&crc; //Used to convert uint64_t to byte array
  uint8_t bytesOut = width/8;
  for (uint8_t i = 0; i < bytesOut; i++) {
    destination[i] = temp[bytesOut - 1 - i];
  }
  
  return;
}