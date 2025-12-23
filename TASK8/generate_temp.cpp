#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>

int main() {
    std::srand(time(nullptr));

    int temp = 20 + (std::rand() % 10); // 20 → 29

    int fd = open("/tmp/temp_sensor.txt",
                  O_WRONLY | O_CREAT | O_TRUNC,
                  0644);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    char buffer[16];
    int len = snprintf(buffer, sizeof(buffer), "%d\n", temp);
    write(fd, buffer, len);

    close(fd);
    return 0;
}
