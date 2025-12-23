#include "sevensegment.hpp"
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <sys/stat.h>

// Defined as member of outputstream
const int outputstream::segment_map[10][7] = {
    {1,1,1,1,1,1,0}, // 0
    {0,1,1,0,0,0,0}, // 1
    {1,1,0,1,1,0,1}, // 2
    {1,1,1,1,0,0,1}, // 3
    {0,1,1,0,0,1,1}, // 4
    {1,0,1,1,0,1,1}, // 5
    {1,0,1,1,1,1,1}, // 6
    {1,1,1,0,0,0,0}, // 7
    {1,1,1,1,1,1,1}, // 8
    {1,1,1,1,0,1,1}  // 9
};

const int sevensegment::gpio_pins[7] = {529, 530, 539, 534, 535, 536, 537};

stream::stream(const char* filePath) : fd(-1), path(filePath) {}
stream::~stream() { if(fd != -1) close(fd); }

inputstream::inputstream() : stream(nullptr) {}
void inputstream::openfd() {}
int inputstream::readNumber() {
    int num;
    std::cout << "Enter number (0-9): ";
    if(!(std::cin >> num)) return 0;
    return (num < 0) ? 0 : (num > 9 ? 9 : num);
}
inputstream::~inputstream() {}

outputstream::outputstream() : stream(nullptr) {
    for(int i=0; i<7; i++) fd_values[i] = -1;
}
void outputstream::openfd() {}
void outputstream::print(int x) { std::cout << "Displaying: " << x << std::endl; }

void outputstream::showSegment(int digit) {
    if(digit < 0 || digit > 9) return;
    for(int i = 0; i < 7; i++) {
        if(fd_values[i] < 0) continue;
        const char* val = segment_map[digit][i] ? "1" : "0";
        // pwrite handles the seek to 0 and write in one atomic step
        pwrite(fd_values[i], val, 1, 0); 
    }
}
outputstream::~outputstream() {}

sevensegment::sevensegment(const char* path) : stream(path), inputstream(), outputstream() {
    GpioInit();
}

void sevensegment::GpioInit() {
    for(int i = 0; i < 7; i++) {
        std::string pin = std::to_string(gpio_pins[i]);
        std::string gpio_dir = std::string(path) + "/gpio" + pin;
        
        struct stat st;
        if (stat(gpio_dir.c_str(), &st) != 0) {
            int fe = open((std::string(path) + "/export").c_str(), O_WRONLY);
            if(fe != -1) {
                write(fe, pin.c_str(), pin.size());
                close(fe);
            }
            usleep(100000); 
        }

        int fd_dir = open((gpio_dir + "/direction").c_str(), O_WRONLY);
        if(fd_dir != -1) {
            write(fd_dir, "out", 3);
            close(fd_dir);
        }

        fd_values[i] = open((gpio_dir + "/value").c_str(), O_WRONLY);
    }
}

void sevensegment::GpioUnexport() {
    for(int i = 0; i < 7; i++) {
        if(fd_values[i] != -1) {
            close(fd_values[i]);
            fd_values[i] = -1;
        }
        int fu = open((std::string(path) + "/unexport").c_str(), O_WRONLY);
        if(fu != -1) {
            std::string pin = std::to_string(gpio_pins[i]);
            write(fu, pin.c_str(), pin.size());
            close(fu);
        }
    }
}

void sevensegment::run() {
    int num = readNumber();
    print(num);
    showSegment(num);
}

void sevensegment::openfd() {}
sevensegment::~sevensegment() { GpioUnexport(); }