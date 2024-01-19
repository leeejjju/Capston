#include <stdio.h>

extern
int 
triangle_type (int, int, int) ;

int main (int argc, char **argv) {
	int a, b, c;
	scanf("%d %d %d", &a, &b, &c) ;
	triangle_type(a, b, c) ;
}
