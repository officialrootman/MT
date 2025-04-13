#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

// ISH Shell'de uyumlu çalışması için yapılandırma
#define USE_NANOSLEEP
#ifdef USE_NANOSLEEP
#include <time.h>
#else
#include <unistd.h>
#endif

// Yapılandırma sabitleri
#define BLOCK_SIZE 2048  // ISH'de daha küçük blok boyutu
#define POOL_SIZE 50     // Daha az bellek kullanımı
#define MAX_RETRY 3      // Hata durumunda yeniden deneme sayısı

// Bellek havuzu yapısı
typedef struct {
    void** blocks;
    size_t block_size;
    size_t pool_size;
    size_t available;
    pthread_mutex_t mutex;
} MemoryPool;

// Güvenli uyku fonksiyonu
void safe_sleep_microseconds(unsigned long microseconds) {
#ifdef USE_NANOSLEEP
    struct timespec ts;
    ts.tv_sec = microseconds / 1000000;
    ts.tv_nsec = (microseconds % 1000000) * 1000;
    nanosleep(&ts, NULL);
#else
    usleep(microseconds);
#endif
}

// Bellek havuzu oluşturma
MemoryPool* createMemoryPool(size_t block_size, size_t pool_size) {
    MemoryPool* pool = NULL;
    int retry_count = 0;

    while (retry_count < MAX_RETRY) {
        pool = (MemoryPool*)malloc(sizeof(MemoryPool));
        if (pool) break;
        safe_sleep_microseconds(100000); // 100ms bekle ve tekrar dene
        retry_count++;
    }

    if (!pool) return NULL;

    pool->blocks = (void**)malloc(pool_size * sizeof(void*));
    if (!pool->blocks) {
        free(pool);
        return NULL;
    }

    for (size_t i = 0; i < pool_size; i++) {
        pool->blocks[i] = malloc(block_size);
        if (!pool->blocks[i]) {
            // Hata durumunda temizlik
            for (size_t j = 0; j < i; j++) {
                free(pool->blocks[j]);
            }
            free(pool->blocks);
            free(pool);
            return NULL;
        }
        // Bloğu sıfırla
        memset(pool->blocks[i], 0, block_size);
    }

    pool->block_size = block_size;
    pool->pool_size = pool_size;
    pool->available = pool_size;
    
    if (pthread_mutex_init(&pool->mutex, NULL) != 0) {
        // Mutex oluşturma hatası durumunda temizlik
        for (size_t i = 0; i < pool_size; i++) {
            free(pool->blocks[i]);
        }
        free(pool->blocks);
        free(pool);
        return NULL;
    }

    return pool;
}

// Bellek bloğu alma
void* getMemoryBlock(MemoryPool* pool) {
    if (!pool) return NULL;

    void* block = NULL;
    if (pthread_mutex_lock(&pool->mutex) != 0) {
        return NULL;
    }
    
    if (pool->available > 0) {
        block = pool->blocks[--pool->available];
    }
    
    pthread_mutex_unlock(&pool->mutex);
    return block;
}

// Bellek bloğunu havuza geri verme
int returnMemoryBlock(MemoryPool* pool, void* block) {
    if (!pool || !block) return -1;

    if (pthread_mutex_lock(&pool->mutex) != 0) {
        return -1;
    }
    
    if (pool->available < pool->pool_size) {
        memset(block, 0, pool->block_size); // Bloğu temizle
        pool->blocks[pool->available++] = block;
        pthread_mutex_unlock(&pool->mutex);
        return 0;
    }
    
    pthread_mutex_unlock(&pool->mutex);
    return -1;
}

// Veri işleme fonksiyonu
void processDataEfficiently(void* data, size_t size) {
    if (!data || size == 0) return;

    const size_t chunk_size = 512; // ISH için daha küçük chunk boyutu
    unsigned char* ptr = (unsigned char*)data;
    
    for (size_t i = 0; i < size; i += chunk_size) {
        size_t current_chunk = (i + chunk_size > size) ? (size - i) : chunk_size;
        
        // CPU yükünü dengele
        if (i % (chunk_size * 8) == 0) {
            safe_sleep_microseconds(100); // 100 mikrosaniye bekle
        }
        
        // Veri işleme
        for (size_t j = 0; j < current_chunk; j++) {
            ptr[i + j] = ptr[i + j] ^ 0xFF;
        }
    }
}

// Bellek havuzunu temizle
void cleanupMemoryPool(MemoryPool* pool) {
    if (!pool) return;

    if (pool->blocks) {
        for (size_t i = 0; i < pool->pool_size; i++) {
            if (pool->blocks[i]) {
                free(pool->blocks[i]);
            }
        }
        free(pool->blocks);
    }
    
    pthread_mutex_destroy(&pool->mutex);
    free(pool);
}

// Ana fonksiyon
int main() {
    printf("ISH Shell Memory Pool Manager Starting...\n");

    // Bellek havuzu oluştur
    MemoryPool* pool = createMemoryPool(BLOCK_SIZE, POOL_SIZE);
    if (!pool) {
        fprintf(stderr, "Failed to create memory pool!\n");
        return 1;
    }

    printf("Memory pool created successfully.\n");
    printf("Block Size: %zu bytes\n", BLOCK_SIZE);
    printf("Pool Size: %zu blocks\n", POOL_SIZE);

    // Test işlemleri
    void* block = getMemoryBlock(pool);
    if (block) {
        printf("Successfully acquired memory block.\n");
        
        // Veri işleme testi
        processDataEfficiently(block, BLOCK_SIZE);
        printf("Data processing completed.\n");

        // Bloğu geri ver
        if (returnMemoryBlock(pool, block) == 0) {
            printf("Successfully returned memory block.\n");
        } else {
            fprintf(stderr, "Failed to return memory block!\n");
        }
    } else {
        fprintf(stderr, "Failed to get memory block!\n");
    }

    // Temizlik
    cleanupMemoryPool(pool);
    printf("Memory pool cleaned up. Exiting...\n");

    return 0;
}