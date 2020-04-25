#include <stdio.h>

int noarg() { printf("[noarg()]"); return 6; }
int witharg(int x, int y) { printf("[witharg%d]", x + y); return x+y; }