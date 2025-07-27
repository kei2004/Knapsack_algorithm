#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <windows.h>

typedef struct {
    int id; // ★ 元のIDを追加
    int time;
    int wage;
} Job;

Job *jobs;
Job *original_jobs_for_print; // ★ 表示用にソート前のデータを保持
int n, capacity;
int *selected, *best_selection;
int best_wage = 0;
long long search_count = 0;

void PrintSolution() {
    printf("Max total wage = %d yen\n", best_wage);
    printf("Chosen jobs:\n");
    for (int i = 0; i < n; i++) {
        if (best_selection[i]) {
            // ★ ソート後の順番 i に対応する元のIDを使って表示
            printf("Job %d: %d hour(s), %d yen\n", jobs[i].id, jobs[i].time, jobs[i].wage);
        }
    }
}

double EstimateUpperBound(int idx, int time_used, int wage_total) {
    double bound = wage_total;
    int current_time = time_used;
    for (int i = idx; i < n; i++) {
        if (current_time + jobs[i].time <= capacity) {
            current_time += jobs[i].time;
            bound += jobs[i].wage;
        } else {
            int remain = capacity - current_time;
            bound += (double)jobs[i].wage * remain / jobs[i].time;
            break;
        }
    }
    return bound;
}

int cmp(const void *a, const void *b) {
    Job *j1 = (Job *)a;
    Job *j2 = (Job *)b;
    double r1 = j1->time > 0 ? (double)j1->wage / j1->time : 0;
    double r2 = j2->time > 0 ? (double)j2->wage / j2->time : 0;
    if (r1 < r2) return 1;
    if (r1 > r2) return -1;
    return 0;
}

void BranchBound(int idx, int time_used, int wage_total) {
    search_count++;

    if (idx == n) {
        if (wage_total > best_wage) {
            best_wage = wage_total;
            for (int i = 0; i < n; i++) best_selection[i] = selected[i];
        }
        return;
    }
    
    double bound = EstimateUpperBound(idx, time_used, wage_total);
    if (bound <= best_wage) {
        return;
    }

    if (time_used + jobs[idx].time <= capacity) {
        selected[idx] = 1;
        BranchBound(idx + 1, time_used + jobs[idx].time, wage_total + jobs[idx].wage);
    }

    selected[idx] = 0;
    BranchBound(idx + 1, time_used, wage_total);
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
    if(jobs == NULL) return 1;

    for (int i = 0; i < n; i++) {
        int id;
        fgets(buf, sizeof(buf), fp);
        sscanf(buf, "%d %d %d", &id, &jobs[i].time, &jobs[i].wage);
        jobs[i].id = id; // ★ 元のIDを保存
    }
    fclose(fp);

    selected = malloc(sizeof(int) * n);
    best_selection = malloc(sizeof(int) * n);
    if(selected == NULL || best_selection == NULL) return 1;
    for (int i = 0; i < n; i++) selected[i] = best_selection[i] = 0;

    qsort(jobs, n, sizeof(Job), cmp);

    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);

    QueryPerformanceCounter(&start);
    BranchBound(0, 0, 0);
    QueryPerformanceCounter(&end);
    
    double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;

    PrintSolution();
    printf("\n[探索回数] %lld (探索したノード数)\n", search_count);
    printf("[探索時間] %.10f 秒\n", elapsed);

    free(jobs);
    free(selected);
    free(best_selection);
    return 0;
}