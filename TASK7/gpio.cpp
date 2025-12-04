#include "gpio.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <iostream>

void GPIO::writeFile(const char* path, const char* value, size_t len) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        std::cerr << "Cannot open " << path << ": " << strerror(errno) << "\n";
        exit(1);
    }
    if (write(fd, value, len) != (ssize_t)len) {
        std::cerr << "Cannot write to " << path << "\n";
        close(fd);
        exit(1);
    }
    close(fd);
}

GPIO::GPIO(int pin) : GPIO_pin(pin + 512), fd_value(-1) {
   
    std::string gpio_str = std::to_string(GPIO_pin);
    writeFile("/sys/class/gpio/export", gpio_str.c_str(), gpio_str.size());
    sleep(1);

    
    std::string dir_path = "/sys/class/gpio/gpio" + gpio_str + "/direction";
    writeFile(dir_path.c_str(), "out", 3);

    
    std::string value_path = "/sys/class/gpio/gpio" + gpio_str + "/value";
    fd_value = open(value_path.c_str(), O_WRONLY);
    if (fd_value < 0) {
        std::cerr << "Error opening value file: " << strerror(errno) << "\n";
        exit(1);
    }
}

GPIO::~GPIO() {
    if (fd_value >= 0){
        close(fd_value);
    }
    std::string gpio_str = std::to_string(GPIO_pin);
    writeFile("/sys/class/gpio/unexport", gpio_str.c_str(), gpio_str.size());
}

void GPIO::on() {
    if (write(fd_value, "1", 1) != 1){
        std::cerr << "Error writing 1 to value\n";
    }
        
}

void GPIO::off() {
    if (write(fd_value, "0", 1) != 1){
        std::cerr << "Error writing 0 to value\n";
    }
        
}
