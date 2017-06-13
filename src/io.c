#include "io.h"

/// This function returns 0 if the specific value in the PIO_PDSR register is set.
/// Since the board is active low, this means, that if we have an low pegel (0) on the pin we have an active input
/// otherwise it is inactive (1)
int getInput(StructPIO* pioBase,int key) {
 
    if(pioBase->PIO_PDSR & key) {
            return 0;
    }
    
    return 1;
}