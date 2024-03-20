#include <stdio.h>
#include <stdlib.h>

extern 
int 
triangle_type (int a, int b, int c) 
{
	int match ;


	match = 0 ;
	if (a == b)
		match = match + 1 ;
	if (a == c)
		match = match + 2 ;
	if (b == c)
		match = match + 3 ;

	int d = a + b ;

	printf("(%d,%d,%d, %d)\n", a, b, c, d) ;


	if (match == 0) {
		if ((a + b) <= c)
			printf("Not a triangle\n") ;
		else if ((b + c) <= a)
			printf("Not a triangle\n") ;
		else if ((a + c) <= b)
			printf("Not a triangle\n") ;
		else
			printf("Scalene\n") ;
	}
	else if (match == 1) {
		if ((a + c) <= b)
			printf("Not a triangle\n") ;
		else 
			printf("Isosceles\n") ;
	}
	else if (match == 2) {
		if ((a + c) <= b)
			printf("Not a triangle\n") ;
		else 
			printf("Isosceles\n") ;
	}
	else if (match == 3) {
		if ((b + c) <= a)
			printf("Not a triangle\n") ;
		else
			printf("Isosceles\n") ;
	}
	else {
		printf("Equilateral\n") ;
	}

	//exit(0) ;

	return 0 ;
}
