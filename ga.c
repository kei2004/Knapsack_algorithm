#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef struct {
    int id;
    int time;
    int wage;
    double efficiency;
} Job;

Job *jobs;
int n, capacity;

int compare_jobs(const void *a, const void *b) {
    Job *jobA = (Job *)a;
    Job *jobB = (Job *)b;
    if (jobA->efficiency < jobB->efficiency) return 1;
    if (jobA->efficiency > jobB->efficiency) return -1;
    return 0;
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
    
    // 元のデータを保持するためのコピーを作成
    Job *original_jobs = malloc(sizeof(Job) * n);
    if (original_jobs == NULL) { free(jobs); return 1; }

    for (int i = 0; i < n; i++) {
        int id;
        fgets(buf, sizeof(buf), fp);
        sscanf(buf, "%d %d %d", &id, &jobs[i].time, &jobs[i].wage);
        jobs[i].id = id;
        if (jobs[i].time > 0) {
            jobs[i].efficiency = (double)jobs[i].wage / jobs[i].time;
        } else {
            jobs[i].efficiency = 0;
        }
        // 元のデータをコピー
        original_jobs[i] = jobs[i];
    }
    fclose(fp);

    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    
    QueryPerformanceCounter(&start);
    
    qsort(jobs, n, sizeof(Job), compare_jobs);

    int best_wage = 0;
    int best_mask = 0;
    int total_time = 0;
    
    for (int i = 0; i < n; i++) {
        if (total_time + jobs[i].time <= capacity) {
            total_time += jobs[i].time;
            best_wage += jobs[i].wage;
            best_mask |= (1 << jobs[i].id);
        }
    }

    QueryPerformanceCounter(&end);
    double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;

    printf("Max total wage = %d yen\n", best_wage);
    printf("Chosen jobs:\n");

    // ★ 元の順番で表示するために original_jobs を使う
    for (int i = 0; i < n; i++) {
        if (best_mask & (1 << i)) {
            printf("Job %d: %d hour(s), %d yen\n", original_jobs[i].id, original_jobs[i].time, original_jobs[i].wage);
        }
    }

    long long search_count = n;
    printf("\n[探索回数] %lld (ソート後の線形探索)\n", search_count);
    printf("[探索時間] %.10f 秒\n", elapsed);

    free(jobs);
    free(original_jobs);
    return 0;
}