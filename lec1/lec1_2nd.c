#include <stdio.h>
int main(){
    int num1,num2;
    printf("첫 번째 정수 :");
    scanf("%d",&num1);
    printf("두 번째 정수 :");
    scanf("%d",&num2);

    printf("입력한 정수는 각각 [%d] 와/과 [%d] 입니다.\n",num1,num2);

    printf("-------------\n");
    printf("합 : %d\n", num1 + num2);  //sum
    printf("차 : %d\n", num1 - num2);  //subtraction
    printf("곱 : %d\n", num1 * num2); //product
    printf("몫 : %d\n", num1 / num2); //quotient
    printf("나머지 : %d\n", num1 % num2); //remainder

    
    return 0;

};