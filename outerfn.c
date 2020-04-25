#include <stdio.h>
#include <stdlib.h>

int noarg() { printf("[noarg()]"); return 6; }
int witharg(int x,int y) { printf("[witharg%d]", x + y); return x+y; }
void alloc4(int **p,int a,int b,int c,int d) { (*p)=calloc(4,sizeof(int));(*p)[0]=a;(*p)[1]=b;(*p)[2]=c;(*p)[3]=d; }