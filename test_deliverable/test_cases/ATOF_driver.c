#include <stdlib.h>

extern char* pi;

int main(){ return !( 3.14159265 == atof(pi)  ); }
