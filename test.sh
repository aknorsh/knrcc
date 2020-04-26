#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./knrcc "$input" > tmp.s
  cc -o tmp tmp.s outerfn.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

cc -c outerfn.c

assert 0  "int main(){0;}"
assert 42 "int main(){42;}"
assert 21 "int main(){5+20-4;}"
assert 41 "int main(){ 12 + 34 - 5 ;}"

assert 47 "int main(){5 + 6 * 7;  }"
assert 15 "int main(){5 * (9 - 6);}"
assert 4  "int main(){(3 + 5) / 2; }"

assert 10 "int main(){-10 + 20;                }"
assert 10 "int main(){+10;                     }"
assert 12 "int main(){-2 * -3 * +(-6 + 2) / -2;}"

assert 1 "int main(){(0<1)      * ((1<1)+1)  * ((1<0)+1) ;}"
assert 1 "int main(){(0<=1)     * (1<=1)     * ((1<=0)+1);}"
assert 1 "int main(){((0>1)+1)  * ((1>1)+1)  * (1>0)     ;}"
assert 1 "int main(){((0>=1)+1) * (1>=1)     * (1>=0)    ;}"
assert 1 "int main(){((0==1)+1) * (1==1)     * ((1==0)+1);}"
assert 1 "int main(){(0!=1)     * ((1!=1)+1) * (1!=0)    ;}"

assert 2 "int main(){-2; -1; 0; 1; 2;               }"
assert 2 "int main(){int a;a = 3; a * 2 / 3;        }"
assert 2 "int main(){int n; n = 1; n = n + 1; n;    }"
assert 2 "int main(){int y; int z;y = 5; z = 3;y-z; }"
assert 2 "int main(){int f_;int b2;f_=-1;b2=3;f_+b2;}"

assert 3 "int main(){return 3; return 5; return 6;                   }"
assert 3 "int main(){int a; int b;a = 1; b = 2; return a * 2 + b - 1;}"

assert 4 "int main(){int a; a = 2; if (a > 0) return 4;        }"
assert 4 "int main(){int a;a=2;if(a<0)return 3;else return 4;  }"
assert 4 "int main(){if (0) 0; else if (0) 0; else 4;          }"
assert 4 "int main(){int a; a = 1; while (a != 4) a = a + 1; a;}"
assert 4 "int main(){int a;int b;b=0;for(a=0;a<4;a=a+1)b=b+1;b;}"

assert 5 "int main(){int bob;bob=0;{bob=bob+1;bob=bob+2;bob=bob+3;}bob-1;     }"
assert 5 "int main(){{0;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;}5;            }"
assert 5 "int main(){int t;t=10;if(t>11){return 10;}else{t=5;return t;}       }"
assert 5 "int main(){int a;int b;a=120;b=0;while(b!=5){b=b+1;a=b;}a;          }"
assert 5 "int main(){int a;int bo;bo=0;for(a=-12;a<12;a=a+1){bo=bo+1;bo=5;}bo;}"

assert 6 "int main(){noarg();}" # noarg() returns 6
assert 6 "int main(){witharg(2,4);}" # witharg(x,y) returns x+y

assert 7 "int add(int x,int y){return x+y;}int main(){add(4,3);}"
assert 55 "int f(int x){if(x<3)return 1;return f(x-1)+f(x-2);}int main(){f(10);}"

assert 8 "int main(){int x;int y;x=8;y=&x;return *y;                  }"
assert 8 "int main(){int x;int y;int z;x=8;y=5;z=&y+8;return *z;      }"
assert 8 "int main(){int x;int *y;y=&x;*y=8;return x;                 }"
assert 8 "int main(){int *p;alloc4(&p,1,2,4,8);int *q;q=p+3;return *q;}"

assert 8 "int main(){int x;return sizeof(x)+sizeof(x+3);}"
assert 8 "int main(){int *y;return (sizeof(y)+sizeof(y+3)+sizeof(*y)*2)/3;}"
assert 8 "int main(){return sizeof(1)+sizeof(sizeof(1));}"

assert 9 "int main(){int a[10];return 9;}"
assert 9 "int main(){int a[2];*a=5;*(a+1)=4;int *p;p=a;return *p+*(p+1);}"

echo OK