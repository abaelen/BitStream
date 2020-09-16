// BitStream.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "BitStorage.h"

void main()
{
    std::cout << "Hello World!\n";

    uint8_t Writebuffer[310];
    memset(Writebuffer,0,310);
    //BitStorageArray ADC_Reading(Writebuffer, 16, 0, 32);
	//BitStorageArray ADC_Tick(Writebuffer, 16, 16, 32);
	//BitStorageArray ADC_CRC(Writebuffer, 7, 24, 31);

	BitStorageArray ADC_Reading(Writebuffer, 12, 0, 31);
	BitStorageArray ADC_Tick(Writebuffer, 12, 12, 31);
	BitStorageArray ADC_CRC(Writebuffer, 7, 24, 31);

    ADC_Reading[0] = 1365;
    ADC_Reading[2] = 4095;
	ADC_Reading[4] = 1365;
	ADC_Reading[6] = 2048;
	ADC_Tick[0]= 585;
	ADC_CRC[1]=1365;


	uint32_t count = 1; uint32_t count1 = 1;

	int i = 0;
	int y = 0;

	std::cout <<" ";
	for (i = 1022; i >= 0; i--) {
		for (y = 7; y >= 0; y--) {
			std::cout << ((Writebuffer[i] >> y) & 1);
			//if (count == 8) { std::cout << "\n"; count = 0; }
			if (count >= 31) { std::cout << "\n"; count = 0; }
			if (count1==7 ||count1 ==19 || count1==31) { std::cout << " "; }
			if (count1>=31) count1=0;
			count++; count1++;
		}
	}

	uint64_t out = ADC_Reading[4];
	

	std::cout << " ";
	union _outa {
		uint64_t b64;
		uint8_t b8[8];
	} outa;

	outa.b64 = out;

	for (i = 7; i >= 0; i--) {
		for (y = 7; y >= 0; y--) {
			std::cout << ((outa.b8[i] >> y) & 1);
			if (count >= 8) { std::cout << " "; count = 0; }
			//if (count1 == 8 || count1 == 19 || count1 == 31) { std::cout << " "; }
			if (count1 >= 8) count1 = 0;
			count++; count1++;
		}
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

