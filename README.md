For more information about the DX FT8 Multiband Tablet Transceiver please use the

https://github.com/WB2CBA/DX-FT8-FT8-MULTIBAND-TABLET-TRANSCEIVER

repositotory.

The original sources for the transceiver firmware are stored on a shared google drive (called Katy.zip)

I was unable to build these sources. 
I held and email exchange with Charles Hill, W5BAA. 
Charlie supplied with a very useful PDF to get the sources building using the STM32 Cube IDE. 
I am using the STM32CubeIDE Version: 1.16.1

This was very successful, unfortunately I was still unable to build the sources due to build errors.
These were due to C variables are being defined in header files causing multiple definition errors at link time.
I suspect in part this is because Arduino-style C++ code has been converted to C.

I modified these sources to build the firmware successfully.
There are still concerning warnings about potential buffer overflows often marked by TODO's in the code.
I took the opportunity to clean up the code removing most of the commented out code, unused variables and so on.
I also used the STM IDE to reformat the code for consistency.
This repo is the result. 
This is a work in progress and will complete it when my hardware arrives.

Paul G8KIG
