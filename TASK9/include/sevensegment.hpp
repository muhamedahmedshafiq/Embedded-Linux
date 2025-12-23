#ifndef SEVENSEGMENT_HPP
#define SEVENSEGMENT_HPP

#include <iostream>

class stream {
protected:
    int fd;
    const char* path;
public:
    stream(const char* filePath);
    virtual void openfd() = 0;
    virtual ~stream();
};

class inputstream : virtual public stream {
public:
    inputstream();
    int readNumber();
    void openfd() override;
    virtual ~inputstream();
};

class outputstream : virtual public stream {
protected:
    int fd_values[7]; 
    static const int segment_map[10][7]; 
public:
    outputstream();
    void print(int x);
    void showSegment(int x);
    void openfd() override;
    virtual ~outputstream();
};

class sevensegment : public inputstream, public outputstream {
private:
    static const int gpio_pins[7]; 
    void GpioInit();
    void GpioUnexport();
public:
    sevensegment(const char* path);
    void openfd() override;
    void run();
    ~sevensegment();
};

#endif