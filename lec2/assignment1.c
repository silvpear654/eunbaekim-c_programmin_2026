#include <stdio.h>
#include <stdlib.h>  // rand(), srand() 함수를 쓰기 위해 포함
#include <time.h>    // time() 함수로 랜덤 시드를 설정하기 위해 포함

int main() {
    // srand(time(NULL)): 프로그램 실행 시마다 다른 난수 시퀀스를 만들기 위해
    // time(NULL)은 현재 시각을 초 단위 정수로 반환하므로, 매번 다른 시드가 됨
    srand(time(NULL));

    // 8명의 선수 이름을 2차원 char 배열로 저장
    // [0]~[7] 인덱스로 접근, 각 이름은 최대 19글자(한글은 3바이트씩이므로 여유 있게)
    char names[8][20] = {
        "플레이어", "철수", "영희", "민준",
        "지아", "현우", "수빈", "태양"
    };

    // players 배열: 현재 라운드에 남아있는 선수들의 '번호(0~7)'를 저장
    // 처음엔 8명 전부 참가
    int players[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    int num_players = 8;  // 현재 남은 선수 수

    // 타이틀 출력
    printf("============================================\n");
    printf("          가위바위보 토너먼트\n");
    printf("============================================\n\n");

    // 대진표 출력
    printf("[ 대진표 ]\n");
    printf("--------------------------------------------\n");
    printf("  8강\n");
    // i를 0, 2, 4, 6으로 2씩 증가시켜 한 쌍씩 출력
    int i;
    for (i = 0; i < 8; i += 2) {
        printf("  [%d] %-8s vs  [%d] %s\n",
               i + 1, names[players[i]],
               i + 2, names[players[i + 1]]);
    }
    printf("--------------------------------------------\n");
    printf("  당신은 [1] %s입니다.\n", names[0]);
    printf("--------------------------------------------\n\n");

    // round_name: 현재 라운드 이름 출력용
    // 8명→"8강", 4명→"4강", 2명→"결승"
    // 토너먼트 루프: 남은 선수가 1명이 될 때까지 반복
    while (num_players > 1) {
        // 이번 라운드의 승자를 담을 임시 배열
        int winners[8];
        int num_winners = 0;

        // 라운드 이름 결정: 조건문으로 분기
        char round_name[10];
        if (num_players == 8) {
            // sprintf: 문자열을 변수에 포맷팅하여 저장하는 함수
            sprintf(round_name, "8강");
        } else if (num_players == 4) {
            sprintf(round_name, "4강");
        } else {
            sprintf(round_name, "결승");
        }

        // 매치 진행: 2명씩 짝지어 경기
        for (i = 0; i < num_players; i += 2) {
            int p1 = players[i];      // 왼쪽 선수 번호
            int p2 = players[i + 1];  // 오른쪽 선수 번호

            int winner;

            // 플레이어(0번)가 포함된 경기인지 확인
            if (p1 == 0 || p2 == 0) {
                // 플레이어가 참여하는 경기
                // opponent: 플레이어의 상대방 번호
                int opponent = (p1 == 0) ? p2 : p1;

                printf("============================================\n");
                printf("  %s  -  상대 : %s\n", round_name, names[opponent]);
                printf("============================================\n");

                // 승패가 결정될 때까지 무한 반복 (비기면 재경기)
                while (1) {
                    int my_choice, opp_choice;

                    printf("  1: 가위    2: 바위    3: 보\n");
                    printf("  선택 > ");
                    scanf("%d", &my_choice);

                    // 입력 검증: 1, 2, 3이 아니면 재입력
                    while (my_choice < 1 || my_choice > 3) {
                        printf("  1, 2, 3 중에서 입력하세요 > ");
                        scanf("%d", &my_choice);
                    }

                    // rand() % 3: 0, 1, 2 중 하나를 반환
                    // +1을 해서 1(가위), 2(바위), 3(보)로 변환
                    opp_choice = rand() % 3 + 1;

                    // 선택 번호를 한글로 변환하기 위한 배열
                    // 인덱스 0은 사용하지 않고, 1=가위, 2=바위, 3=보
                    char *hand[4] = {"", "가위", "바위", "보"};

                    printf("\n  나 : %s    %s : %s\n",
                           hand[my_choice], names[opponent], hand[opp_choice]);

                    // 승패 판정 로직
                    // 같으면 비김 → 재경기(continue)
                    if (my_choice == opp_choice) {
                        printf("  => 비겼습니다! 재경기!\n\n");
                        continue;  // while(1) 처음으로 돌아감
                    }

                    // 이기는 조건: (가위→보), (바위→가위), (보→바위)
                    // 수학적으로: (my - opp + 3) % 3 == 1 이면 내가 이김
                    // 예) 가위(1) vs 보(3): (1-3+3)%3 = 1%3 = 1 → 승리
                    // 예) 바위(2) vs 가위(1): (2-1+3)%3 = 4%3 = 1 → 승리
                    // 예) 보(3) vs 바위(2): (3-2+3)%3 = 4%3 = 1 → 승리
                    // 이 공식이 성립하는 이유:
                    //   가위=1, 바위=2, 보=3으로 놓으면, 이기는 쪽은 항상
                    //   상대보다 '1만큼 작은 값'임 (순환적으로).
                    //   즉 1은 3을, 2는 1을, 3은 2를 이김.
                    //   (my - opp)의 결과는 -2, -1, 0, 1, 2 중 하나인데,
                    //   +3을 해서 음수를 방지한 뒤 %3을 취하면:
                    //     이기면 → 1, 비기면 → 0, 지면 → 2 가 됨.
                    int result = (my_choice - opp_choice + 3) % 3;

                    if (result == 1) {
                        printf("  => 이겼습니다!\n");
                        winner = 0;  // 플레이어 승리
                    } else {
                        printf("  => 졌습니다...\n");
                        winner = opponent;  // 상대 승리
                    }

                    // 다음 라운드 안내
                    if (winner == 0 && num_players > 2) {
                        // 아직 결승이 아니면 진출 메시지
                        char *next_round;
                        if (num_players == 8) next_round = "4강";
                        else next_round = "결승";
                        printf("  => %s 진출!\n", next_round);
                    }
                    printf("\n");
                    break;  // 승패 결정되면 while(1) 탈출
                }

                // 플레이어가 졌으면 패배 메시지 출력 후 프로그램 종료
                if (winner != 0) {
                    printf("============================================\n");
                    printf("  아쉽습니다. 다음 기회에!\n");
                    printf("============================================\n");
                    return 0;  // main에서 return하면 프로그램 즉시 종료
                }

            } else {
                // AI끼리의 경기: 랜덤으로 승자 결정
                // rand() % 2가 0이면 p1 승, 1이면 p2 승
                winner = (rand() % 2 == 0) ? p1 : p2;
            }

            // 이번 경기의 승자를 winners 배열에 추가
            winners[num_winners] = winner;
            num_winners++;
        }

        // 다음 라운드를 위해 players 배열을 winners로 교체
        for (i = 0; i < num_winners; i++) {
            players[i] = winners[i];
        }
        num_players = num_winners;
    }

    // 여기까지 왔다는 것은 플레이어가 우승했다는 뜻
    printf("============================================\n");
    printf("  축하합니다! 우승!\n");
    printf("============================================\n");

    return 0;
}