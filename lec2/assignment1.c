#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ── 전역 변수 ──
// [수정 포인트] const 키워드 추가
// 왜 이 코드를 추가했냐면: names 배열을 전역 변수로 두어 여러 함수에서 접근하기 쉽게 만들면서도,
// 코드 어딘가에서 실수로 선수들의 이름이 변경되는 것을 원천 차단(읽기 전용)하기 위해서다냥!
const char names[8][20] = {
    "플레이어", "철수", "영희", "민준",
    "지아", "현우", "수빈", "태양"
};

// ── 함수 선언부 (프로토타입) ──
// [수정 포인트] 이전 버전에서 추가했던 names 배열 매개변수를 다시 제거했다냥.
// 왜 이 코드를 수정했냐면: 이제 안전한 전역 변수를 바로 쓰면 되니까 매개변수로 넘겨줄 필요가 없어졌기 때문이다냥!
void print_title(void);
void print_bracket(int players[], int n);
int get_player_choice(void);
const char* hand_name(int choice);
int judge(int a, int b);
int play_match(int opponent);
int ai_match(int p1, int p2);
const char* round_name(int n);

// ── 타이틀 출력 ──
void print_title(void) {
    printf("============================================\n");
    printf("          가위바위보 토너먼트\n");
    printf("============================================\n\n");
}

// ── 대진표 출력 ──
void print_bracket(int players[], int n) {
    printf("[ 대진표 ]\n");
    printf("--------------------------------------------\n");
    printf("  %s\n", round_name(n));
    int i;
    for (i = 0; i < n; i += 2) {
        printf("  [%d] %-8s vs  [%d] %s\n",
               players[i] + 1, names[players[i]],
               players[i + 1] + 1, names[players[i + 1]]);
    }
    printf("--------------------------------------------\n");
    printf("  당신은 [1] %s입니다.\n", names[0]);
    printf("--------------------------------------------\n\n");
}

// ── 플레이어 입력 받기 ──
int get_player_choice(void) {
    int choice;
    printf("  1: 가위    2: 바위    3: 보\n");
    printf("  선택 > ");
    scanf("%d", &choice);
    while (choice < 1 || choice > 3) {
        printf("  1, 2, 3 중에서 입력하세요 > ");
        scanf("%d", &choice);
    }
    return choice;
}

// ── 숫자 → 한글 변환 ──
const char* hand_name(int choice) {
    switch (choice) {
        case 1: return "가위";
        case 2: return "바위";
        case 3: return "보";
        default: return "???";
    }
}

// ── 승패 판정 ──
int judge(int a, int b) {
    if (a == b) return 0;
    if ((a == 1 && b == 3) || 
        (a == 2 && b == 1) || 
        (a == 3 && b == 2))   
        return 1;
    return -1;
}

// ── 라운드 이름 반환 ──
const char* round_name(int n) {
    if (n == 8) return "8강";
    if (n == 4) return "4강";
    return "결승";
}

// ── 플레이어 vs AI 경기 ──
int play_match(int opponent) {
    printf("============================================\n");
    printf("  (플레이어 경기) 상대 : %s\n", names[opponent]);
    printf("============================================\n");

    while (1) {
        int my = get_player_choice();
        int opp = rand() % 3 + 1;

        printf("\n  나 : %s    %s : %s\n",
               hand_name(my), names[opponent], hand_name(opp));

        int result = judge(my, opp);
        if (result == 0) {
            printf("  => 비겼습니다! 재경기!\n\n");
            continue;
        } else if (result == 1) {
            printf("  => 이겼습니다!\n");
            return 0;
        } else {
            printf("  => 졌습니다...\n");
            return opponent;
        }
    }
}

// ── AI vs AI 경기 ──
int ai_match(int p1, int p2) {
    // 현재는 단순히 50% 확률로 승자를 결정하는 상태다냥
    return (rand() % 2 == 0) ? p1 : p2;
}

// ── 메인 함수 ──
int main() {
    srand(time(NULL));

    int players[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    int num = 8;

    print_title();
    print_bracket(players, num);

    while (num > 1) {
        int winners[8];
        int w_count = 0;
        int i;

        for (i = 0; i < num; i += 2) {
            int p1 = players[i];
            int p2 = players[i + 1];
            int winner;

            if (p1 == 0 || p2 == 0) {
                int opp = (p1 == 0) ? p2 : p1;
                winner = play_match(opp);

                if (winner != 0) {
                    printf("\n============================================\n");
                    printf("  아쉽습니다. 다음 기회에!\n");
                    printf("============================================\n");
                    return 0;
                }
                
                if (num > 2) {
                    printf("  => %s 진출!\n\n",
                           (num == 8) ? "4강" : "결승");
                } else {
                    printf("\n");
                }
            } else {
                winner = ai_match(p1, p2);
            }

            winners[w_count++] = winner;
        }

        for (i = 0; i < w_count; i++) {
            players[i] = winners[i];
        }
        num = w_count;
    }

    printf("============================================\n");
    printf("  축하합니다! 우승!\n");
    printf("============================================\n");

    return 0;
}