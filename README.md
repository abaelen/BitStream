# BitStream
 This github hosts a simple class which implements a back end array, declared as ordinary byte array (8-bits). But internal datastructure managed as a 'collection' of bit-subarray.
 
 The concept behind this class is to be able to store multiple bit-arrays into one larger 'portable' array.
 This concept could for example be used in serializing ADC data. In an ADC data application, one has typically a 12-bit ADC reading and a 14-bit timestamp reading.
 One would want to serialize this 12-bit and 14-bit data into a bit stream that can be pushed over USB line.
 
 To avoid the program being cluttered with the serialization portion, one would want to write the 12-bit ADC reading into an ADC_Readings array and the 14-bit timestamp reading into an ADC_Timestamp array. On the background, the class would manage to store the data serialized into a portable byte sized buffer. This byte sized buffer can then be pushed onto the USB line and de-serialized on the other size with the same class.
 
 More detailed overview of the concept:
 
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
							7bit		12bit			12bit
						<----------------> <----------------> <----------------->
	Row 1022
	Row 1021					Mbr3		Mbr2			Mbr1
	Row .												^
	Row .												|
	Row .												|
	0											<-----{0,0}
 
