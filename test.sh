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

assert 0 "main(){0;}"
assert 42 "main(){42;}"
assert 21 "main(){5+20-4;}"
assert 41 "main(){ 12 + 34 - 5 ;}"

assert 47 "main(){5 + 6 * 7;  }"
assert 15 "main(){5 * (9 - 6);}"
assert 4  "main(){(3 + 5) / 2; }"

assert 10 "main(){-10 + 20;                }"
assert 10 "main(){+10;                     }"
assert 12 "main(){-2 * -3 * +(-6 + 2) / -2;}"

assert 1 "main(){(0<1)      * ((1<1)+1)  * ((1<0)+1) ;}"
assert 1 "main(){(0<=1)     * (1<=1)     * ((1<=0)+1);}"
assert 1 "main(){((0>1)+1)  * ((1>1)+1)  * (1>0)     ;}"
assert 1 "main(){((0>=1)+1) * (1>=1)     * (1>=0)    ;}"
assert 1 "main(){((0==1)+1) * (1==1)     * ((1==0)+1);}"
assert 1 "main(){(0!=1)     * ((1!=1)+1) * (1!=0)    ;}"

assert 2 "main(){-2; -1; 0; 1; 2;                     }"
assert 2 "main(){a = 3; a * 2 / 3;                    }"
assert 2 "main(){n = 1; n = n + 1; n;                 }"
assert 2 "main(){y = 5; z = 3; y - z;                 }"
assert 2 "main(){f_ = -1; b2 = 3; f_ + b2;            }"

assert 3 "main(){return 3; return 5; return 6;        }"
assert 3 "main(){a = 1; b = 2; return a * 2 + b - 1;  }"

assert 4 "main(){a = 2; if (a > 0) return 4;                }"
assert 4 "main(){a = 2; if (a < 0) return 3; else return 4; }"
assert 4 "main(){if (0) 0; else if (0) 0; else 4;           }"
assert 4 "main(){a = 1; while (a != 4) a = a + 1; a;        }"
assert 4 "main(){b = 0; for (a = 0; a < 4; a = a+1)b=b+1; b;}"

assert 5 "main(){bob=0;{bob=bob+1;bob=bob+2;bob=bob+3;}bob-1; }"
assert 5 "main(){{0;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;}5;}"
assert 5 "main(){t=10;if(t>11){return 10;}else{t=5;return t;} }"
assert 5 "main(){a_c2=120;b=0;while(b!=5){b=b+1;a_c2=b;}a_c2; }"
assert 5 "main(){bo=0;for(a=-12;a<12;a=a+1){bo=bo+1;bo=5;}bo; }"

assert 6 "main(){noarg();}" # noarg() returns 6
assert 6 "main(){witharg(2,4);}" # witharg(x,y) returns x+y

assert 7 "add(x,y){return x+y;}main(){add(4,3);}"
assert 55 "f(x){if(x<3)return 1;return f(x-1)+f(x-2);}main(){f(10);}"

echo OK