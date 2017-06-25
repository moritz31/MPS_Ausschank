#include "pio.h"

/**
  * @brief Gets an input from an specified pin
  * @author Moritz Weichert, Phil Goeritz
  * @param pioBase Base where the pin is attached too
  * @param key Number of the pin in that base
  * @return Returns the value converted to active high (1 --> On, 0 --> Off)
  */
 int getInput(StructPIO* pioBase, int key);

/**
 * @brief Initalise a specifc key as input
 * @author Moritz Weichert, Phil Goeritz
 * @param pioBase Base where the pin is attached too
 * @param key Number of the pin in that base
 */
 void initInput(StructPIO* pioBase, int key);