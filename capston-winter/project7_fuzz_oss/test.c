#include <stdio.h>

__attribute__((weak)) int func();

int main(){

	if(func) func();

}

//int func(){	printf("i am func\n");}
