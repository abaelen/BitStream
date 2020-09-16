#pragma once
#include <stdint.h>

/*
Array = ordinary byte array (ie. 8-bit) -> therefore portable accross any program via any means (ie. internal memory or USB):
Allocation into the array is dynamic on bit-by-bit basis.
Number of Members is ('unlimited') choosen (in this example 3).
Each Member has a number of bits allocated (max 64-bits) and shows and offset relative to the beginning of the row

Member declaration is equivalent to declaring an array, eg. ADC_Reading[1023], ADC_Tick[1023] (although internally it is stored in the backend Array)
Member assignment is equivalent as an array, eg. ADC_Reading[4] = 4095
Member data collection is equivalent as an array, eg. uint64_t out = ADC_Reading[4]
=> User is not confronted with the complicated process of storing/collecting  the number into/from the backend byte array.

The Members hook into the byte buffer using a pointer. This implies the members can be reassigned to another buffer on the fly.

--> Purpose of my design. Serialize ADC readings with tick data and CRC check into a byte array, 
which can be used to feed the USB byte stream buffer of the Arduino, ie. Serial.print/write. As speed is important in this application. 
This program ensures no bits on the stream goes wasted. As the ADC reading is only 12 bit, a 16-bit variable would send zero 4-bits over the USB line, each time transmitted.
The code for capturing and storing the data is not interferred by the serializing task as each data (reading, tick, CRC) is written as if in its 'own' array.



Code length is less < 200 lines and uses no external libraries.


Back end array, declared as ordinary byte array (8-bits).
But internal datastructure managed as a 'collection' of bit-subarrays:

											BitStringLength=31
						<------------------------------------------------------->

						31-offset Mbr3: 24-----offset Mbr2:12------offset Mbr1:-0
								7bit				12bit				12bit
						<----------------> <----------------> <----------------->
Row 1022
	1021						Mbr3				Mbr2			Mbr1
	.																			^
	.																			|
	.																			|
	0																	<-----{0,0}

Actual storing is according to 8-bite (ie. byte) scheme.

*/


class BitStorageArray
{
	public: class _BitStorageMember 
	{
		public: uint64_t BitSize = 0;

		public: uint8_t* Storage = NULL;
		public: uint64_t StorageSize = 0;
		public: uint64_t FirstOffset = 0;
		public: uint64_t TotalBitString = 0;
		public: int64_t Index=-1;
		
		public: void operator= (uint64_t value);

		public: operator uint64_t()
		{
			try {
				uint64_t firstByte = (FirstOffset + Index * TotalBitString) / 8;
				uint64_t firstBit = (FirstOffset + Index * TotalBitString) % 8;
				uint64_t lastByte = (FirstOffset + Index * TotalBitString + BitSize) / 8;
				uint64_t lastBit = (FirstOffset + Index * TotalBitString + BitSize) % 8;
				if (lastBit == 0) { lastByte--; lastBit = 8; };


				uint8_t* tempStorage = Storage;

				union _value
				{
					uint64_t b64=0;
					uint8_t b8[8];
				}value;

				for (uint64_t i = firstByte; i <= lastByte; i++)
				{
					value.b8[i - firstByte] = tempStorage[i];
				}

				uint8_t filter = pow(2, firstBit) - 1;
				filter = ~(filter);
				value.b8[0] = value.b8[0] & filter;
				filter = pow(2, lastBit) - 1;
				value.b8[lastByte - firstByte] = value.b8[lastByte - firstByte] & filter;
				value.b64 = (value.b64 >> firstBit) & (uint64_t)(pow(2,BitSize)-1);

				Index = -1;
				
				
				return value.b64;

			}
			catch (...)
			{
				std::cout << "Error";
			}
		}


	} BitStorageMember;


	public: BitStorageArray(uint8_t* storage, uint64_t bitSize, uint64_t firstOffset, uint64_t totalBitString)
			{
				BitStorageMember.BitSize = bitSize;
				BitStorageMember.FirstOffset = firstOffset;
				BitStorageMember.Storage = storage;
				BitStorageMember.TotalBitString = totalBitString;
			}

	public: _BitStorageMember& operator[] (uint64_t index)
			{
				BitStorageMember.Index = index;
				return BitStorageMember;
			}


};

void BitStorageArray::_BitStorageMember::operator= (uint64_t value)
{
	try{
		uint64_t lastByte = (FirstOffset + Index * TotalBitString + BitSize) / 8;
		uint64_t lastBit = (FirstOffset + Index * TotalBitString + BitSize) % 8;
		if (lastBit == 0) {lastByte--; lastBit=8;};


		uint64_t tempValue = value;
		uint64_t bit = 0;
		uint64_t bytePos = lastByte;
		uint64_t bitPos = lastBit;
		uint8_t byte = 0;

		for (int64_t bitptr = BitSize-1; bitptr >=0; bitptr--)
		{
			bit = (tempValue >> bitptr)& (uint64_t)1; 
			bit = bit << (bitPos-1);
			byte = Storage[bytePos];
			byte = byte | bit;
			Storage[bytePos] = byte;
			if (bitPos==1) {bitPos = 8; bytePos--;}
			else {bitPos--;}
			//tempValue = tempValue >> 1;
		}

		Index = -1;
		
	}
	catch (...)
	{
		std::cout << "Error";
	}
}