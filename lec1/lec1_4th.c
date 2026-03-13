#include <stdio.h>
int main() {
    int b=3;
    int a;

    a=b;
    printf("a=%d, b=%d\n", a, b);

    a+=2;
    printf("a: %d\n", a);
    return 0;
}