#ifndef _TOYNAMES_H_
#define _TOYNAMES_H_

#include <map>
#include <string>

void loadNames();
void fillCodes(uint16_t* startPtr, uint16_t val, uint16_t n);
uint16_t getCode(const char* name);
std::string getCharName(uint16_t code);








#endif