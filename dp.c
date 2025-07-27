#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct {
    int time;
    int wage;
} Job;

Job *jobs;
int n, capacity;

// DPテーブルと選択記録テーブル
int **dp;
int **choice;
long long search_count = 0; // ★ グローバル変数としてカウンタを宣言

void solve_dp() {
    // メモリ確保と初期化
    dp = malloc((n + 1) * sizeof(int *));
    choice = malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        dp[i] = malloc((capacity + 1) * sizeof(int));
        choice[i] = malloc((capacity + 1) * sizeof(int));
        memset(dp[i], 0, (capacity + 1) * sizeof(int));
        memset(choice[i], 0, (capacity + 1) * sizeof(int));
    }

    // DPテーブルを埋める
    for (int i = 1; i <= n; i++) {
        for (int w = 1; w <= capacity; w++) {
            search_count++; // ★ テーブルを1マス埋める計算ごとにカウント
            
            int wage_without = dp[i - 1][w];
            if (jobs[i - 1].time <= w) {
                int wage_with = dp[i - 1][w - jobs[i - 1].time] + jobs[i - 1].wage;
                if (wage_with > wage_without) {
                    dp[i][w] = wage_with;
                    choice[i][w] = 1;
                } else {
                    dp[i][w] = wage_without;
                }
            } else {
                dp[i][w] = wage_without;
            }
        }
    }
}

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

    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "%d %d", &n, &capacity);

    jobs = malloc(sizeof(Job) * n);
    if (jobs == NULL) { return 1; }

    for (int i = 0; i < n; i++) {
        int id;
        fgets(buf, sizeof(buf), fp);
        sscanf(buf, "%d %d %d", &id, &jobs[i].time, &jobs[i].wage);
    }
    fclose(fp);

    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);

    QueryPerformanceCounter(&start);
    solve_dp();
    QueryPerformanceCounter(&end);
    
    double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;

    int best_wage = dp[n][capacity];
    int best_mask = 0;
    int w = capacity;
    for (int i = n; i > 0; i--) {
        if (choice[i][w]) {
            best_mask |= (1 << (i - 1));
            w -= jobs[i - 1].time;
        }
    }
    
    printf("Max total wage = %d yen\n", best_wage);
    printf("Chosen jobs:\n");
    for (int i = 0; i < n; i++) {
        if (best_mask & (1 << i)) {
            printf("Job %d: %d hour(s), %d yen\n", i, jobs[i].time, jobs[i].wage);
        }
    }

    printf("\n[探索回数] %lld (DPテーブルの計算回数)\n", search_count);
    printf("[探索時間] %.10f 秒\n", elapsed);

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
        free(choice[i]);
    }
    free(dp);
    free(choice);
    free(jobs);

    return 0;
}