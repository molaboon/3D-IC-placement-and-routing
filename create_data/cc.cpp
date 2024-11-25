#include <stdlib.h>
#include <iostream>
#include <time.h>

int main()
{
	srand( time(NULL) );
	double a = rand() / RAND_MAX ;
	
	printf("%lf\n", a);

	return 0;
}
