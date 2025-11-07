#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define KEY 0x01234u
#define MEM_SIZE 4096

int main() {
    int shmid;

    shmid = shmget(KEY, MEM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    char *ptr = (char *) shmat(shmid, NULL, 0);
    if (ptr == (char *) -1) {
        perror("shmat failed");
        return 1;
    }

    strcpy(ptr, "hello");
    printf("Data written to shared memory!\n");

    
    shmdt(ptr);

    return 0;
}
