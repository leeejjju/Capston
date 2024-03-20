#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

int triangle_type(int, int, int) ;

extern 
int 
LLVMFuzzerTestOneInput (const uint8_t *Data, size_t Size)
{
	if (Size < 12)
		return 0 ;

	int a, b, c ;

	a = *((int *) (Data + 0)) ;
	b = *((int *) (Data + 4)) ;
	c = *((int *) (Data + 8)) ;

	int d = a + b ;
	printf("%d\n", d) ;

	triangle_type(a, b, c) ;

	return 0 ;
}
