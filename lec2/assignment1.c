/*
 * 가위바위보 토너먼트 프로그램
 * - 8명의 선수가 참가하는 토너먼트 (8강 → 4강 → 결승)
 * - 플레이어(1번)는 직접 가위/바위/보를 선택
 * - 나머지 7명은 AI로, 랜덤하게 선택
 * - 비기면 재경기, 1/2/3 이외의 입력은 재입력 요구
 */

#include <stdio.h>   // printf(화면 출력), scanf(키보드 입력), getchar(문자 하나 읽기) 사용
#include <stdlib.h>  // rand()(난수 생성), srand()(난수 시드 설정) 사용
#include <time.h>    // time()(현재 시각을 초 단위로 반환) 사용

/*
 * ── Player 구조체 정의 ──
 * 구조체(struct)는 서로 관련 있는 변수들을 하나로 묶는 사용자 정의 타입이다.
 * typedef를 앞에 붙이면, 사용할 때 'struct Player p;' 대신 'Player p;'로 짧게 쓸 수 있다.
 */
typedef struct {
    int id;          // 선수 고유 번호 (0번~7번). 0번이 플레이어.
    char name[20];   // 선수 이름. 한글은 UTF-8에서 글자당 3바이트이므로 20바이트면 넉넉함.
    int is_human;    // 사람인지 AI인지 구분. 1이면 사람, 0이면 AI.
} Player;

/*
 * ── 전역 상수 배열: 8명의 선수 데이터 ──
 *
 * const 키워드: 이 배열은 "읽기 전용"이라고 컴파일러에게 알려준다.
 * 프로그램 어디서든 all_players[i].name을 읽을 수는 있지만,
 * all_players[i].name[0] = 'A' 같이 값을 변경하려 하면 컴파일 에러가 발생한다.
 *
 * 전역 변수로 둔 이유:
 * 이 데이터는 프로그램 전체에서 "이름 조회"용으로만 쓰이고 절대 수정되지 않으므로,
 * 매번 함수 매개변수로 전달하는 것보다 전역에 두는 것이 코드가 더 간결하다.
 *
 * 구조체 초기화 문법: {id값, "이름문자열", is_human값} 형태로 각 필드를 순서대로 채운다.
 */
const Player all_players[8] = {
    {0, "플레이어", 1},  // 인덱스 0: 사람 플레이어 (is_human = 1)
    {1, "철수",     0},  // 인덱스 1: AI (is_human = 0)
    {2, "영희",     0},  // 인덱스 2: AI
    {3, "민준",     0},  // 인덱스 3: AI
    {4, "지아",     0},  // 인덱스 4: AI
    {5, "현우",     0},  // 인덱스 5: AI
    {6, "수빈",     0},  // 인덱스 6: AI
    {7, "태양",     0}   // 인덱스 7: AI
};

/*
 * ── 승패 판정 함수 ──
 *
 * 매개변수:
 *   a - 첫 번째 선수의 선택 (1=가위, 2=바위, 3=보)
 *   b - 두 번째 선수의 선택 (1=가위, 2=바위, 3=보)
 *
 * 반환값:
 *    0 → 무승부 (같은 것을 냄)
 *    1 → a가 이김
 *   -1 → b가 이김
 *
 * 이 함수를 따로 만든 이유 (DRY 원칙):
 *   ai_vs_ai와 human_vs_ai 두 함수에서 동일한 판정 로직이 필요하다.
 *   함수로 분리하면 한 곳만 수정하면 되므로 유지보수가 쉬워진다.
 */
int judge(int a, int b) {
    /* 같은 것을 냈으면 무승부 */
    if (a == b) return 0;

    /*
     * 이기는 3가지 경우를 직접 나열:
     *   가위(1)는 보(3)를 자른다   → a=1, b=3
     *   바위(2)는 가위(1)를 부순다 → a=2, b=1
     *   보(3)는 바위(2)를 감싼다   → a=3, b=2
     *
     * &&는 AND 연산자: 왼쪽과 오른쪽이 모두 참이어야 참
     * ||는 OR 연산자: 하나라도 참이면 참
     *
     * 세 조건 중 하나라도 해당하면 a가 이긴 것이므로 1을 반환한다.
     */
    if ((a == 1 && b == 3) ||
        (a == 2 && b == 1) ||
        (a == 3 && b == 2))
        return 1;

    /* 무승부도 아니고 a가 이긴 것도 아니면, b가 이긴 것 */
    return -1;
}

/*
 * ── AI vs AI 경기 함수 ──
 *
 * 매개변수:
 *   a     - 첫 번째 AI 선수의 포인터 (const이므로 읽기만 가능)
 *   b     - 두 번째 AI 선수의 포인터
 *   round - 현재 라운드 이름 문자열 ("8강", "4강", "결승")
 *
 * 반환값: 승자의 id (a->id 또는 b->id)
 *
 * const Player *a 의 의미:
 *   Player 구조체를 가리키는 포인터인데, const가 붙었으므로
 *   이 포인터를 통해 구조체 내용을 수정할 수 없다.
 *   all_players 배열이 const로 선언되어 있으므로, 그 주소를 받는
 *   포인터도 const여야 타입이 일치한다.
 *
 * 포인터로 전달하는 이유:
 *   구조체를 값으로 전달하면 전체 데이터가 복사되지만,
 *   포인터로 전달하면 주소(8바이트)만 복사되므로 효율적이다.
 */
int ai_vs_ai(const Player *a, const Player *b, const char *round) {
    /*
     * hand 배열: 숫자를 한글로 변환하기 위한 문자열 포인터 배열
     * hand[0]은 사용하지 않고, hand[1]="가위", hand[2]="바위", hand[3]="보"
     * const char*: 각 포인터가 가리키는 문자열을 수정할 수 없다는 의미
     */
    const char *hand[] = {"", "가위", "바위", "보"};

    /* a->name: 포인터 a가 가리키는 구조체의 name 필드에 접근 */
    /* (*a).name 과 동일한 표현이지만, -> 연산자가 더 간결함 */
    printf("  [AI 경기 - %s] %s vs %s\n",round, a->name, b->name);

    /*
     * while (1): 무한 반복. 비기면 재경기를 해야 하므로 무한 루프를 쓰고,
     * 승패가 결정되면 return으로 함수 자체를 빠져나간다.
     */
    while (1) {
        /*
         * rand(): 0 ~ RAND_MAX 사이의 랜덤 정수를 반환
         * % 3: 3으로 나눈 나머지 → 0, 1, 2 중 하나
         * + 1: 1, 2, 3 중 하나로 변환 (가위, 바위, 보에 대응)
         */
        int ca = rand() % 3 + 1;  /* a 선수의 선택 */
        int cb = rand() % 3 + 1;  /* b 선수의 선택 */

        /*
         * hand[ca]: ca가 2이면 hand[2] = "바위"
         * 숫자를 바로 배열 인덱스로 써서 한글 변환
         */
        printf("    %s: %s  vs  %s: %s", a->name, hand[ca], b->name, hand[cb]);

        /* judge 함수를 호출하여 승패 판정 */
        int result = judge(ca, cb);

        if (result == 0) {
            /* 무승부: 재경기 안내 후 continue로 while 처음으로 돌아감 */
            printf(" → 무승부! 재경기\n");
            continue;
        }

        if (result == 1) {
            /* a가 이김: a의 id를 반환하며 함수 종료 */
            printf(" → %s 승리!\n", a->name);
            return a->id;
        } else {
            /* b가 이김: b의 id를 반환하며 함수 종료 */
            printf(" → %s 승리!\n", b->name);
            return b->id;
        }
    }
}

/*
 * ── 사람 vs AI 경기 함수 ──
 *
 * 매개변수:
 *   human - 플레이어의 포인터
 *   opp   - 상대 AI의 포인터
 *   round - 현재 라운드 이름
 *
 * 반환값: 승자의 id (0이면 플레이어 승, 아니면 AI 승)
 *
 * ai_vs_ai와의 차이: 플레이어의 선택은 rand()가 아니라 scanf()로 입력받음
 */
int human_vs_ai(const Player *human, const Player *opp, const char *round) {
    const char *hand[] = {"", "가위", "바위", "보"};

    /* 경기 헤더 출력: 현재 라운드와 상대 이름 표시 */
    printf("\n============================================\n");
    printf("  %s  -  상대 : %s\n", round, opp->name);
    printf("============================================\n");

    /* 무한 루프: 비기면 재경기, 잘못된 입력이면 재입력 */
    while (1) {
        int my, ai;  /* my: 플레이어 선택, ai: AI 선택 */
        printf("  1: 가위    2: 바위    3: 보\n");
        printf("  선택 > ");

        /*
         * scanf("%d", &my)의 반환값 체크
         *
         * scanf는 성공적으로 읽어들인 항목의 개수를 반환한다.
         * "%d"는 정수 1개를 읽으므로, 성공하면 1을 반환한다.
         * 만약 사용자가 "abc" 같은 문자를 입력하면, 정수로 변환할 수 없어서
         * scanf는 0을 반환하고, 잘못된 문자는 입력 버퍼에 그대로 남는다.
         *
         * 이때 버퍼를 비우지 않으면, 다음 scanf에서도 같은 문자를 읽으려 하다가
         * 계속 실패하여 무한루프에 빠진다.
         */
        if (scanf("%d", &my) != 1) {
            /*
             * getchar(): 입력 버퍼에서 문자를 하나씩 꺼내서 소비하는 함수
             * '\n'(엔터)이 나올 때까지 반복하여 버퍼에 남은 쓰레기를 모두 비운다.
             * 예: 사용자가 "abc\n"을 입력했으면, 'a', 'b', 'c', '\n'을 순서대로 소비
             */
            while (getchar() != '\n');
            printf("  1, 2, 3 중에서 입력하세요 > ");
            continue;  /* while(1) 처음으로 돌아가서 다시 입력받음 */
        }

        /*
         * 범위 검증: 정수는 성공적으로 읽었지만 1~3이 아닌 경우
         * 예: 0, 4, -1, 99 등
         * || 는 OR 연산자: 둘 중 하나라도 참이면 참
         * my < 1 이면 참(0 이하의 수), my > 3 이면 참(4 이상의 수)
         */
        if (my < 1 || my > 3) {
            printf("  1, 2, 3 중에서 입력하세요 > ");
            continue;
        }

        /* AI의 선택: 1~3 중 랜덤 */
        ai = rand() % 3 + 1;

        /* 양쪽의 선택을 한글로 변환하여 출력 */
        printf("\n  나 : %s    %s : %s\n", hand[my], opp->name, hand[ai]);

        /* 승패 판정 */
        int result = judge(my, ai);

        if (result == 0) {
            printf("  => 비겼습니다! 재경기!\n\n");
            continue;  /* 무승부: while 처음으로 돌아가서 재경기 */
        }

        if (result == 1) {
            printf("  => 이겼습니다!\n");
            return human->id;  /* 플레이어(id=0)를 반환 */
        } else {
            printf("  => 졌습니다...\n");
            return opp->id;    /* 상대의 id를 반환 (0이 아닌 값) */
        }
    }
}

/*
 * ── 메인 함수 ──
 * 프로그램의 진입점(entry point). 운영체제가 프로그램을 실행할 때
 * 가장 먼저 호출하는 함수이다.
 * 반환 타입 int: 프로그램 종료 시 운영체제에 종료 코드를 전달.
 * 0이면 정상 종료, 0이 아니면 에러.
 */
int main() {
    /*
     * srand(time(NULL)) — 난수 시드 초기화
     *
     * rand() 함수는 "의사 난수(pseudo-random)"를 생성한다.
     * 이는 수학 공식으로 만들어낸 숫자 시퀀스라서,
     * 시드(seed, 시작값)가 같으면 항상 같은 순서의 숫자가 나온다.
     *
     * time(NULL)은 1970년 1월 1일 0시 0분 0초(UTC) 이후
     * 현재까지 경과한 초 수를 정수로 반환한다.
     * 이 값은 매 초 바뀌므로, 프로그램을 실행할 때마다
     * 다른 시드가 설정되어 다른 난수 시퀀스가 생성된다.
     *
     * 만약 srand를 호출하지 않으면 시드가 기본값(보통 1)으로 고정되어,
     * 매번 실행할 때마다 AI가 똑같은 패를 내게 된다.
     */
    srand(time(NULL));

    /*
     * bracket 배열: 현재 라운드에 남아있는 선수들의 id를 저장
     * 처음에는 0~7 모두 참가 (8명)
     * 라운드가 진행되면 승자들의 id로 덮어씌워진다.
     * 예: 8강 후 → {0, 3, 5, 7, ?, ?, ?, ?} (앞 4개만 유효)
     */
    int bracket[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    /*
     * count: 현재 남은 선수 수
     * 8강: 8명, 4강: 4명, 결승: 2명
     * 1명이 되면 토너먼트 종료
     */
    int count = 8;

    /* ── 타이틀 출력 ── */
    printf("============================================\n");
    printf("          가위바위보 토너먼트\n");
    printf("============================================\n\n");

    /*
     * ── 토너먼트 메인 루프 ──
     * count > 1인 동안 반복: 선수가 2명 이상이면 아직 우승자가 안 정해진 것
     * 8명 → 4경기 → 4명 남음
     * 4명 → 2경기 → 2명 남음
     * 2명 → 1경기 → 1명 남음 → 루프 종료
     */
    while (count > 1) {
        /*
         * round: 현재 라운드 이름을 가리키는 포인터
         * const char*: 가리키는 문자열("8강" 등)을 수정할 수 없다는 의미
         * count 값에 따라 적절한 문자열을 가리키도록 설정
         */
        const char *round;
        if (count == 8) round = "8강";
        else if (count == 4) round = "4강";
        else round = "결승";

        /* ── 대진표 출력 ── */
        printf("[ 대진표 - %s ]\n", round);
        printf("--------------------------------------------\n");

        /*
         * 대진표 반복문: i를 0, 2, 4, 6... 으로 2씩 증가
         * bracket[i]와 bracket[i+1]이 한 쌍(대전 상대)
         *
         * bracket[i] + 1: 배열은 0번부터 시작하지만,
         *   화면에는 1번부터 보여주기 위해 +1
         *
         * %-8s: 왼쪽 정렬(-), 8칸 확보(8)하여 문자열(s) 출력
         *   이름 길이가 달라도 "vs"의 위치가 가지런해짐
         *   예: "철수    " (4글자 + 빈칸) vs "플레이어" (8글자)
         *
         * all_players[bracket[i]].name: 이중 인덱싱
         *   bracket[i]가 3이면 → all_players[3].name → "민준"
         */
        int i;
        for (i = 0; i < count; i += 2) {
            printf("  [%d] %-8s vs  [%d] %s\n",
                   bracket[i] + 1, all_players[bracket[i]].name,
                   bracket[i + 1] + 1, all_players[bracket[i + 1]].name);
        }
        printf("--------------------------------------------\n\n");

        /*
         * next 배열: 이번 라운드 승자들의 id를 임시로 저장
         * next_count: 승자 수 (0부터 시작하여 경기마다 1씩 증가)
         */
        int next[8];
        int next_count = 0;

        /*
         * ── 매치 진행 반복문 ──
         * 2명씩 짝지어 경기 진행
         * i=0: bracket[0] vs bracket[1]
         * i=2: bracket[2] vs bracket[3]
         * ...
         */
        for (i = 0; i < count; i += 2) {
            int id1 = bracket[i];      /* 왼쪽 선수의 id */
            int id2 = bracket[i + 1];  /* 오른쪽 선수의 id */
            int winner;                /* 이 경기의 승자 id를 담을 변수 */

            /*
             * 플레이어가 포함된 경기인지 확인
             * all_players[id1].is_human 이 1(참)이거나
             * all_players[id2].is_human 이 1(참)이면
             * → 플레이어가 참여하는 경기
             */
            if (all_players[id1].is_human || all_players[id2].is_human) {
                /*
                 * human_p: 플레이어의 포인터
                 * ai_p: 상대 AI의 포인터
                 *
                 * 삼항 연산자로 누가 사람인지 판별:
                 *   id1이 사람이면 → human_p = &all_players[id1], ai_p = &all_players[id2]
                 *   id2가 사람이면 → human_p = &all_players[id2], ai_p = &all_players[id1]
                 *
                 * & (주소 연산자): 구조체 원본의 메모리 주소를 꺼냄
                 *   이 주소를 포인터 변수에 저장하면, 포인터를 통해 원본에 접근 가능
                 */
                const Player *human_p = all_players[id1].is_human ?
                                        &all_players[id1] : &all_players[id2];
                const Player *ai_p = all_players[id1].is_human ?
                                     &all_players[id2] : &all_players[id1];

                /* 플레이어 경기 진행: 반환값은 승자의 id */
                winner = human_vs_ai(human_p, ai_p, round);

                /*
                 * winner != 0 이면 플레이어가 진 것
                 * (플레이어의 id는 0이므로, 0이 아닌 값이 반환되면 상대가 이긴 것)
                 *
                 * return 0: main 함수에서 return하면 프로그램 전체가 즉시 종료됨
                 * 토너먼트에서 한 번이라도 지면 탈락이므로, 더 진행할 필요 없음
                 */
                if (winner != 0) {
                    printf("\n============================================\n");
                    printf("  아쉽습니다. 다음 기회에!\n");
                    printf("============================================\n");
                    return 0;
                }

                /*
                 * 이겼을 때 다음 라운드 안내
                 * count > 2: 아직 결승이 아닌 경우에만 "진출" 메시지 출력
                 * 결승에서 이기면 진출이 아니라 우승이므로, 여기서는 출력하지 않음
                 *
                 * (count == 8) ? "4강" : "결승"
                 *   8강(count=8)이면 다음은 4강
                 *   4강(count=4)이면 다음은 결승
                 */
                if (count > 2) {
                    const char *next_r = (count == 8) ? "4강" : "결승";
                    printf("  => %s 진출!\n\n", next_r);
                }
            } else {
                /*
                 * 플레이어가 없는 경기: AI끼리 실제로 가위바위보를 진행
                 * &all_players[id1]: id1번 선수 구조체의 주소를 전달
                 */
                winner = ai_vs_ai(&all_players[id1], &all_players[id2], round);
            }

            /*
             * 승자를 next 배열에 추가
             *
             * next[next_count++] = winner; 를 풀어쓰면:
             *   next[next_count] = winner;  // 현재 인덱스에 승자 저장
             *   next_count = next_count + 1; // 인덱스를 1 증가
             *
             * 후위 증가(++)는 "현재 값을 먼저 사용한 뒤, 그 다음에 1을 더한다"
             */
            next[next_count++] = winner;
        }

        /*
         * ── 다음 라운드 준비 ──
         * next 배열(이번 라운드 승자들)을 bracket 배열에 복사
         * 이렇게 하면 bracket이 다음 라운드의 참가자 목록이 됨
         *
         * 예: 8강 결과 승자가 {0, 3, 5, 7}이면
         *   bracket[0]=0, bracket[1]=3, bracket[2]=5, bracket[3]=7
         *   count=4 (4강)
         */
        for (i = 0; i < next_count; i++) {
            bracket[i] = next[i];
        }

        /*
         * count를 승자 수로 업데이트
         * 8강 끝나면 next_count=4 → count=4 (4강)
         * 4강 끝나면 next_count=2 → count=2 (결승)
         * 결승 끝나면 next_count=1 → count=1 (루프 종료)
         */
        count = next_count;

        printf("\n");
    }
    /* while (count > 1) 루프 끝 */

    /*
     * 여기에 도달했다는 것은:
     * - 플레이어가 중간에 패배하지 않았고 (패배 시 return 0으로 이미 종료됨)
     * - count가 1이 되어 루프를 빠져나왔으므로
     * - 플레이어가 모든 경기를 이겨서 우승했다는 뜻
     */
    printf("============================================\n");
    printf("  축하합니다! 우승!\n");
    printf("============================================\n");

    /*
     * return 0: 프로그램이 정상적으로 종료되었음을 운영체제에 알려줌
     * 관례적으로 0은 성공, 그 외의 값은 에러를 의미
     */
    return 0;
}