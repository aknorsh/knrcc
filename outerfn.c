#include <stdio.h>

int noarg() { printf("noarg()@"); }
int witharg(int x, int y) { printf("printf from witharg (%d) :: ", x + y); return x + y; }