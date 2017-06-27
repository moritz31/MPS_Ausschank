

/**
 * @brief Initalize timer and IO for the balance
 * @author Moritz Weichert, Phil Goeritz
 */
void init_balance( void );

/**
 * @brief Measure the current weight on the balance
 * @author Moritz Weichert, Phil Goeritz
 * @return The mass of the object in g
 */
int measure( void );

/**
 * @brief Calculates the mass of the balance based on an given formula
 * @author Moritz Weichert, Phil Goeritz
 * @param c1 Specific calibrationvalue of the balance which need to be adjust
 * @param c2 Specific calibrationvalue of the balance which need to be adjust
 * @param p1 Period1 from the balance
 * @param p2 Period2 from the balance
 * @return The mass of the object in g
 */
int calculateMass(int c1, int c2, float p1, float p2);