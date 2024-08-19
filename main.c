#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
    int x;
    int y;
} Point;

static inline float euclidean_distance(Point a, Point b) {
    int dx = b.x - a.x;
    int dy = b.y - a.y;
    return sqrtf(dx * dx + dy * dy);
}

bool save_solution(int **s, int k, int *route_lengths) {
    FILE *f = fopen("result.txt", "w");
    if (f == NULL) {
        return false;
    }

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < route_lengths[i]; j++) {
            fprintf(f, "%d ", s[i][j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    return true;
}

int* distribute(int items, int lists) {
    int base = items / lists;
    int extra = items % lists;
    int *result = malloc(lists * sizeof(int));
    
    for (int i = 0; i < lists; i++) {
        result[i] = base + (i < extra ? 1 : 0);
    }
    
    return result;
}

int* read_input(const char* instance, int *k, int *n, Point **points) {
    char path[256];
    snprintf(path, sizeof(path), "./instances/%s", instance);

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        return NULL;
    }

    fscanf(f, "%d", k);

    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        count++;
    }
    *n = count - 2;  // -2 to account for k and company location

    *points = malloc((*n + 1) * sizeof(Point));
    rewind(f);
    fscanf(f, "%*d");  // Skip k

    for (int i = 0; i <= *n; i++) {
        int id, x, y;
        fscanf(f, "%d %d %d", &id, &x, &y);
        (*points)[id - 1] = (Point){x, y};
    }

    fclose(f);
    return NULL;
}

float** build_distance_matrix(int n, Point *points) {
    float **m = malloc((n + 1) * sizeof(float*));
    for (int i = 0; i <= n; i++) {
        m[i] = malloc((n + 1) * sizeof(float));
        for (int j = 0; j <= n; j++) {
            if (i != j) {
                m[i][j] = round(euclidean_distance(points[i], points[j]) * 100) / 100;
            } else {
                m[i][j] = 0;
            }
        }
    }
    return m;
}

int** get_initial_solution(int k, int n, int *route_lengths) {
    int **r = malloc(k * sizeof(int*));
    for (int i = 0; i < k; i++) {
        r[i] = malloc((n + 2) * sizeof(int));  // +2 for start and end points
    }

    int *temp = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        temp[i] = i + 1;
    }

    int *distribution = distribute(n, k);

    for (int i = 0; i < k; i++) {
        r[i][0] = 0;
        route_lengths[i] = 1;
    }

    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        int collector;
        do {
            collector = rand() % k;
        } while (distribution[collector] == 0);

        distribution[collector]--;
        int index = rand() % (n - i);
        int point = temp[index];

        r[collector][route_lengths[collector]] = point;
        route_lengths[collector]++;

        temp[index] = temp[n - i - 1];
    }

    for (int i = 0; i < k; i++) {
        r[i][route_lengths[i]] = 0;
        route_lengths[i]++;
    }

    free(temp);
    free(distribution);
    return r;
}

float get_route_distance(int *a, int n, float **m) {
    float total = 0;
    for (int i = 0; i < n - 1; i++) {
        total += m[a[i]][a[i + 1]];
    }
    return total;
}

float get_max_route_distance(int **r, int k, int *route_lengths, float **m) {
    float max = 0;
    float route_distance;

    for (int i = 0; i < k; i++) {
        route_distance = get_route_distance(r[i], route_lengths[i], m);
        if (route_distance > max) max = route_distance;
    }
    
    return round(max * 100) / 100;
}

void choose_two_routes(int k, int *r1, int *r2) {
    *r1 = rand() % k;
    do {
        *r2 = rand() % k;
    } while (*r2 == *r1);
}

void random_route_point(int *r, int n, int *point, int *index) {
    *index = 1 + rand() % (n - 2);
    *point = r[*index];
}

int** local_search(int **s, int k, int x, float **m, int *route_lengths, float *min_distance) {
    int **bks = malloc(k * sizeof(int*));
    int **ps = malloc(k * sizeof(int*));
    int *bks_lengths = malloc(k * sizeof(int));
    int *ps_lengths = malloc(k * sizeof(int));
    
    for (int i = 0; i < k; i++) {
        bks[i] = malloc((route_lengths[i]) * sizeof(int));
        ps[i] = malloc((route_lengths[i]) * sizeof(int));
        memcpy(bks[i], s[i], route_lengths[i] * sizeof(int));
        memcpy(ps[i], s[i], route_lengths[i] * sizeof(int));
        bks_lengths[i] = route_lengths[i];
        ps_lengths[i] = route_lengths[i];
    }

    *min_distance = get_max_route_distance(s, k, route_lengths, m);
    bool first = true;

    while (*min_distance < get_max_route_distance(ps, k, ps_lengths, m) || first) {
        for (int i = 0; i < k; i++) {
            memcpy(ps[i], bks[i], bks_lengths[i] * sizeof(int));
            ps_lengths[i] = bks_lengths[i];
        }
        first = false;

        for (int i = 0; i < x; i++) {
            int **ts = malloc(k * sizeof(int*));
            int *ts_lengths = malloc(k * sizeof(int));
            for (int j = 0; j < k; j++) {
                ts[j] = malloc(ps_lengths[j] * sizeof(int));
                memcpy(ts[j], ps[j], ps_lengths[j] * sizeof(int));
                ts_lengths[j] = ps_lengths[j];
            }

            int r1, r2;
            choose_two_routes(k, &r1, &r2);

            int p1, i1, p2, i2;
            random_route_point(ts[r1], ts_lengths[r1], &p1, &i1);
            random_route_point(ts[r2], ts_lengths[r2], &p2, &i2);

            for (int j = i1; j < ts_lengths[r1] - 1; j++) {
                ts[r1][j] = ts[r1][j + 1];
            }
            ts_lengths[r1]--;

            for (int j = i2; j < ts_lengths[r2] - 1; j++) {
                ts[r2][j] = ts[r2][j + 1];
            }
            ts_lengths[r2]--;

            for (int j = ts_lengths[r1]; j > i1; j--) {
                ts[r1][j] = ts[r1][j - 1];
            }
            ts[r1][i1] = p2;
            ts_lengths[r1]++;

            for (int j = ts_lengths[r2]; j > i2; j--) {
                ts[r2][j] = ts[r2][j - 1];
            }
            ts[r2][i2] = p1;
            ts_lengths[r2]++;

            float distance = get_max_route_distance(ts, k, ts_lengths, m);

            if (distance < *min_distance) {
                for (int j = 0; j < k; j++) {
                    free(bks[j]);
                    bks[j] = malloc(ts_lengths[j] * sizeof(int));
                    memcpy(bks[j], ts[j], ts_lengths[j] * sizeof(int));
                    bks_lengths[j] = ts_lengths[j];
                }
                *min_distance = distance;
            }

            for (int j = 0; j < k; j++) {
                free(ts[j]);
            }
            free(ts);
            free(ts_lengths);
        }
    }

    for (int i = 0; i < k; i++) {
        free(ps[i]);
    }
    free(ps);
    free(ps_lengths);

    memcpy(route_lengths, bks_lengths, k * sizeof(int));
    free(bks_lengths);

    return bks;
}

int** mutate_swap(int **s, int k, int *route_lengths) {
    int **ns = malloc(k * sizeof(int*));
    for (int i = 0; i < k; i++) {
        ns[i] = malloc(route_lengths[i] * sizeof(int));
        memcpy(ns[i], s[i], route_lengths[i] * sizeof(int));
    }

    int r1, r2;
    int p1, i1, p2, i2;
    
    choose_two_routes(k, &r1, &r2);
    random_route_point(ns[r1], route_lengths[r1], &p1, &i1);
    random_route_point(ns[r2], route_lengths[r2], &p2, &i2);
    ns[r1][i1] = p2;
    ns[r2][i2] = p1;

    return ns;
}

int** ils(int k, int n, float **m, int x, int *route_lengths, float *min_distance) {
    int **s = get_initial_solution(k, n, route_lengths);
    s = local_search(s, k, x, m, route_lengths, min_distance);
    int counter = 0;

    while (1) {
        int **ns = mutate_swap(s, k, route_lengths);
        float new_min;
        ns = local_search(ns, k, x, m, route_lengths, &new_min);

        if (new_min < *min_distance) {
            counter = 0;
            for (int i = 0; i < k; i++) {
                free(s[i]);
            }
            free(s);
            s = ns;
            *min_distance = new_min;
            printf("%f\n", *min_distance);
        } else {
            counter++;
            for (int i = 0; i < k; i++) {
                free(ns[i]);
            }
            free(ns);
        }

        if (counter >= 100){
          counter = 0;
          save_solution(s, k, route_lengths);
        }
    }

    return s;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <instance> <x>\n", argv[0]);
        return 1;
    }

    char *instance = argv[1];
    int x = atoi(argv[2]);

    int k, n;
    Point *points;
    read_input(instance, &k, &n, &points);

    float **m = build_distance_matrix(n, points);
    float min_distance = 0;

    int *route_lengths = (int *)malloc(k * sizeof(int));
    int **s = get_initial_solution(k, n, route_lengths);

    ils(k, n, m, x, route_lengths, &min_distance);

    // Clean up
    for (int i = 0; i <= n; i++) {
        free(m[i]);
    }
    free(m);
    free(points);
    for (int i = 0; i < k; i++) {
        free(s[i]);
    }
    free(s);
    free(route_lengths);

    return 0;
}