#ifndef _SerialIO_h_
#define _SerialIO_h_

//
int DevicePart(unsigned char code);

float ValuePart(unsigned char code);

HANDLE ComInit(char* comport, int baudrate );
void ComEnd(HANDLE hComm);
int ReadData(HANDLE hComm, char* buff, unsigned int nread, unsigned int max_size);
bool WriteData(HANDLE hComm, char* buff, unsigned int data_size);

#endif