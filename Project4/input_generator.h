/*
    Operating Systems: Random Input generator (Header)

    author: Fuad Aghazada
    date: 08/10/2018
*/

#ifndef INPUT_GENERATOR_
#define INPUT_GENERATOR_

// Function Prototype
int generateRandomNumber(int min, int max);

int *generateRandomNumbers(int size, int min, int max);

void clearRandomNumbers();

void seed();    

#endif
