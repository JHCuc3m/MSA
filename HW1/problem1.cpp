/**
 * problem1.cpp
 * Assignment #1 - Memory Systems Architecture
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

static inline double now_ms() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1.0e6;
}

static inline void compiler_barrier() {
    asm volatile("" ::: "memory");
}

/* Fisher–Yates shuffle */
void shuffle(size_t *array, size_t n) {
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        size_t t = array[i];
        array[i] = array[j];
        array[j] = t;
    }
}

/* Capacity + Line Size */
double run_experiment(size_t size_bytes, size_t stride) {
    size_t n = size_bytes / sizeof(void*);
    void **buffer;

    posix_memalign((void**)&buffer, 4096, size_bytes);

    for (size_t i = 0; i < n; i++)
        buffer[i] = &buffer[i];

    size_t *idx = (size_t*)malloc(n * sizeof(size_t));
    for (size_t i = 0; i < n; i++) idx[i] = i;

    if (stride == 0) {
        shuffle(idx, n);
        for (size_t i = 0; i < n - 1; i++)
            buffer[idx[i]] = &buffer[idx[i + 1]];
        buffer[idx[n - 1]] = &buffer[idx[0]];
    } else {
        size_t step = stride / sizeof(void*);
        if (step == 0) step = 1;
        for (size_t i = 0; i < n; i++)
            buffer[i] = &buffer[(i + step) % n];
    }

    volatile void *p = buffer[0];

    /* Warm-up */
    for (size_t i = 0; i < n; i++)
        p = *(void**)p;

    compiler_barrier();

    const size_t iters = 10'000'000;
    double start = now_ms();

    for (size_t i = 0; i < iters; i++)
        p = *(void**)p;

    double end = now_ms();
    compiler_barrier();

    if (p == NULL) printf("Impossible\n");

    free(idx);
    free(buffer);

    return ((end - start) * 1e6) / iters;
}

/* ---------- ASSOCIATIVITY KERNEL ---------- */
void run_associativity_experiment() {

    const size_t STRIDE = 1024 * 1024;
    const int MAX_WAYS = 32;

    void **buffer;
    posix_memalign((void**)&buffer, 4096, STRIDE * MAX_WAYS);

    for (int ways = 1; ways <= MAX_WAYS; ways++) {

        for (int i = 0; i < ways; i++) {
            size_t cur = i * STRIDE / sizeof(void*);
            size_t nxt = ((i + 1) % ways) * STRIDE / sizeof(void*);
            buffer[cur] = &buffer[nxt];
        }

        volatile void *p = buffer;

        /* Warm-up */
        for (int i = 0; i < ways * 500; i++)
            p = *(void**)p;

        compiler_barrier();

        size_t iters = 5'000'000;

        double start = now_ms();
        for (size_t i = 0; i < iters; i++)
            p = *(void**)p;
        double end = now_ms();

        compiler_barrier();

        if (p == NULL) printf("Impossible\n");

        double latency = ((end - start) * 1e6) / iters;
        printf("%d, %.2f\n", ways, latency);
    }

    free(buffer);
}

int main() {
    srand(1234);

    /* PART 1 */
    printf("--- PART 1: Cache Capacity & Levels ---\n");
    printf("Size(KB), Latency(ns)\n");

    for (size_t s = 1 << 10; s <= (1ULL << 26); ) {
        double lat = run_experiment(s, 0);
        printf("%zu, %.2f\n", s / 1024, lat);

        if ((s & (s - 1)) == 0)
            s = s + s / 2;
        else
            s = 1ULL << (64 - __builtin_clzll(s));
    }

    /* PART 2 */
    printf("\n--- PART 2: Cache Line Size ---\n");
    printf("Stride(Bytes), Latency(ns)\n");

    size_t test_size = 512 * 1024;
    for (size_t stride = 8; stride <= 512; stride *= 2) {
        double lat = run_experiment(test_size, stride);
        printf("%zu, %.2f\n", stride, lat);
    }

    /* PART 3 */
    printf("\n--- PART 3: LLC Associativity ---\n");
    printf("Ways, Latency(ns)\n");
    
    run_associativity_experiment();
    return 0;
}
