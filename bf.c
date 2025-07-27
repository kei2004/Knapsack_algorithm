#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef struct {
    int time;
    int wage;
} Job;

Job *jobs; // ポインタに変更
int n, capacity;

int main(int argc, char *argv[]) {
    system("chcp 65001 > nul");
    FILE *fp;
    char buf[80];
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]); return 1;
    }

    if ((fp = fopen(argv[1], "r")) == NULL) {
        printf("Cannot open file %s\n", argv[1]); return 1;
    }

    // データ読み込み
    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "%d %d", &n, &capacity);

    // nの数に応じて動的にメモリを確保
    jobs = malloc(sizeof(Job) * n);
    if (jobs == NULL) { return 1; }

    for (int i = 0; i < n; i++) {
        int id;
        fgets(buf, sizeof(buf), fp);
        sscanf(buf, "%d %d %d", &id, &jobs[i].time, &jobs[i].wage);
    }
    fclose(fp);

    // 高精度タイマーの準備
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);

    // 計測開始
    QueryPerformanceCounter(&start);

    int best_wage = 0;
    int best_mask = 0;
    long long search_count = 0;

    // 総当たり：2^n通りのビット列を列挙
    for (int mask = 0; mask < (1LL << n); mask++) {
        search_count++; // 検証した組み合わせの数をカウント
        int total_time = 0;
        int total_wage = 0;
        for (int i = 0; i < n; i++) {
            if (mask & (1 << i)) {
                total_time += jobs[i].time;
                total_wage += jobs[i].wage;
            }
        }
        if (total_time <= capacity && total_wage > best_wage) {
            best_wage = total_wage;
            best_mask = mask;
        }
    }

    // 計測終了
    QueryPerformanceCounter(&end);
    double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;

    // 結果出力
    printf("Max total wage = %d yen\n", best_wage);
    printf("Chosen jobs:\n");
    for (int i = 0; i < n; i++) {
        if (best_mask & (1 << i)) {
            printf("Job %d: %d hour(s), %d yen\n", i, jobs[i].time, jobs[i].wage);
        }
    }

    printf("\n[探索回数] %lld 通り\n", search_count);
    printf("[探索時間] %.10f 秒\n", elapsed);

    free(jobs); // メモリ解放
    return 0;
}