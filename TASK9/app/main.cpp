#include "sevensegment.hpp"  // include header
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cerr << "Usage: sudo ./a.out <digit 0-9>\n";
        return 1;
    }

    int number = std::atoi(argv[1]); // convert argument to integer
    if(number < 0 || number > 9) {
        std::cerr << "Error: digit must be 0-9\n";
        return 1;
    }

    sevensegment seg("/sys/class/gpio"); // initialize GPIO pins

    seg.print(number);       // print to console
    seg.showSegment(number); // display on 7-segment

    return 0;
}
