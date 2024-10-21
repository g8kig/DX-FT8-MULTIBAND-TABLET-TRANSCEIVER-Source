For more information about the DX FT8 Multiband Tablet Transceiver please use the

https://github.com/WB2CBA/DX-FT8-FT8-MULTIBAND-TABLET-TRANSCEIVER

repository.

The original sources for the transceiver firmware are stored on a shared google drive (called Katy.zip)

I was unable to build these sources. 
I held an email exchange with Charles Hill, W5BAA. 
Charley supplied me with a very useful PDF to get the sources building using the STM32 Cube IDE. 
I am using the STM32CubeIDE Version: 1.16.1

Unfortunately I was still unable to build the sources due to build errors.
These errors were due to C variables being defined in header files causing multiple definition errors at link time.
I suspect in part this is because Arduino-style C++ code has been converted to C.

The source code contains the sources to the excellent FT8 library by Karlis Goba.
The original repo for the ft8 library is at

https://github.com/kgoba/ft8_lib

I modified the sources to build the firmware successfully.
There are still concerning warnings about potential buffer overflows often I have marked them with TODO's in the code.
I took the opportunity to clean up the code removing most of the unused commented out code, unreferenced variables and so on.
I also used the STM IDE to reformat the code for consistency.
This repo is the result. 
This is a work in progress and will complete it when my hardware arrives.

Paul G8KIG
