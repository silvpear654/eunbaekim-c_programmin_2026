#include <stdio.h>

int main() {
    int num, count = 0;
    
    printf("정수를 입력하세요: ");
    scanf("%d", &num);
    
    // 음수를 양수로 변환
    if (num < 0) {
        num = -num;
    }
    
    // 0인 경우 처리
    if (num == 0) {
        count = 1;
    } else {
        // do-while을 사용하여 자릿수 계산
        do {
            count++;
            num /= 10;
        } while (num != 0);
    }
    
    printf("자릿수: %d\n", count);
    
    return 0;
}