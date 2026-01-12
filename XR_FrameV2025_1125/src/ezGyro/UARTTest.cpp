// UARTTest.cpp

#define _CRT_SECURE_NO_WARNINGS
//#include "Com.h"
#include "windows.h"
//#include "time.h"
#include "stdio.h"

#include "witmotion.h"

WitMotionT *wmdata;

int main(int argc, char* argv[])
{
	unsigned long ulBaund=9600, ulComNo=3;

	printf("Com = ");
	scanf("%ld",&ulComNo);
	printf("9600-115200 Baud = ");
	scanf("%ld",&ulBaund);

	wmdata = ConnectWitMotion( ulComNo, ulBaund );

	CJY901 *jy = wmdata->jy; //&JY901;

	while (1)
	{
		ReadWitMotion(wmdata);

		wmdata->jy->print();
		PrintWitMotion(wmdata);

		Sleep(10);

		system("cls");
	}
	return 0;
}

