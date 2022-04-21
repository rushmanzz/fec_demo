
#include "galois_noasm.h"
#include "matrix.h"
#include <iostream>

extern const byte mulTable[256][256];

void galMulSlice(byte c, row_type in, row_type out) {
    //std::cout << "-----------galMulSlice-----------" << std::endl;
    for (int n=0;n<in->size();n++) {
        (*out)[n] = mulTable[c][(*in)[n]];
    }
    //std::cout << std::endl;
}

void galMulSliceXor(byte c, row_type in, row_type out) {
    //std::cout << "-----------galMulSliceXor-----------" << std::endl;
    for (int n=0;n<in->size();n++) {
        (*out)[n] ^= mulTable[c][(*in)[n]];
    }
    //std::cout << std::endl;
}
