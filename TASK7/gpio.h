#ifndef GPIO_H
#define GPIO_H
#include <cstddef>   // for size_t

class GPIO {
private:
    int GPIO_pin;
    int fd_value;

    void writeFile(const char* path, const char* value, size_t len);

public:
    GPIO(int pin); 
    ~GPIO();         

    void on();       
    void off();      
};

#endif
