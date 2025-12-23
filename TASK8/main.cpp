#include <iostream>
#include <memory>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>


class Sensor {
private:
    const char* path;

public:
    Sensor(const char* filePath) : path(filePath) {}

    void read(int& temp) {
        int fd = open(path, O_RDONLY);
        if (fd == -1) {
            perror("open");
            return;
        }

        char buffer[16];
        int bytes = ::read(fd, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            temp = atoi(buffer);
        }

        close(fd);
    }
};



class Display {
public:
    void displayTemp(std::weak_ptr<int> tempWeak) {
        if (auto temp = tempWeak.lock()) {
            std::cout << "Temperature: " << *temp << " C\n";
        }
    }
};


class Logging {
private:
    int fd;

public:
    Logging(const char* path) {
        fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1) {
            perror("open");
        }
    }

    ~Logging() {
        if (fd != -1) {
            close(fd);
        }
    }

    void logTemp(std::weak_ptr<int> tempWeak) {
        if (auto temp = tempWeak.lock()) {
            char buffer[64];
            int len = snprintf(buffer, sizeof(buffer),
                               "Temperature: %d C\n", *temp);
            write(fd, buffer, len);
        }
    }
};


int main() {
    
    std::unique_ptr<Sensor> sensor =
    std::make_unique<Sensor>("/tmp/temp_sensor.txt");

    
    std::shared_ptr<int> temperature = std::make_shared<int>(0);

    
    std::weak_ptr<int> weakTemp = temperature;

    
    std::unique_ptr<Display> display = std::make_unique<Display>();
    std::unique_ptr<Logging> logger =
        std::make_unique<Logging>("temperature.log");

    
    sensor->read(*temperature);

    
    display->displayTemp(weakTemp);
    logger->logTemp(weakTemp);

    return 0;
}
