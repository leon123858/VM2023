#include <stdio.h>
#include <unistd.h>

int main() {
        char* target = "I am cute baby.";
        while (1) {
                printf("Address of target: %p\n", target);
                printf("target value: %s\n", target);
                sleep(5);
        }
}