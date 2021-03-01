#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>



class CRC {

  public:
    CRC(uint8_t wdth, uint64_t poly, uint64_t init);
    
    void compute(uint8_t* message, int nBytes, uint8_t* destination);
    
  private:
    uint8_t width;
    uint64_t polynomial;
    uint64_t initial;
    



  
};








#endif