#include <stdio.h>
#include <stdlib.h>

int main(void){
    FILE* fp = fopen("test.png", "wb");
    unsigned char value = 0x0a;
    fwrite(&value, 1, 1, fp);
    return 0;
}