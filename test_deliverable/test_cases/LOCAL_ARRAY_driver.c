#include <stdio.h>

int local_array();
int main() {
  int v = local_array();
  printf("return val = %i\n", v);
  return !( 9 == v );
}
