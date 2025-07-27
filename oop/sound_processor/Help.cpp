//
// Created by helttek on 8/13/24.
//
#include <iostream>
#include "Help.h"

void Help::Display() {
    std::cout << "To use this program the comman should look like this:\n";
    std::cout << "\"sound_processor -c <config.txt> <output.wav> <input1.wav> [<input2.wav> …]\"\n";
    std::cout <<
            "Where config.txt is a ocnfiguration file, that contains information about contverters to be applied to the input .wav files.\n";
    std::cout << "Available converters:\n";
}
