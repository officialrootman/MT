#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

// ANSI Renk Kodları (ISH Shell uyumlu)
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Program sabitleri
#define CRITICAL_CPU 80
#define CRITICAL_RAM 90
#define DELAY_SHORT 1
#define DELAY_NORMAL 2
#define MAX_BUFFER 1024
#define VERSION "1.0-ish"

// Hata kodları
#define SUCCESS 0
#define ERROR_GENERAL -1

typedef struct {
    int cpu_usage;
    int ram_usage;
    int battery_level;
    int performance_mode; // 0: Normal, 1: Yüksek Performans, 2: Güç Tasarrufu
    bool is_optimizing;
} SystemState;

// ISH Shell uyumlu ekran temizleme
void clearScreen(void) {
    printf("\x1b[2J\x1b[H");
    fflush(stdout);
}

// ISH Shell uyumlu giriş tampon temizleme
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void showHeader(void) {
    printf(ANSI_COLOR_CYAN
           "╔════════════════════════════════╗\n"
           "║     TELEFON HIZLANDIRICI      ║\n"
           "║        ISH Shell v%s        ║\n"
           "╚════════════════════════════════╝\n"
           ANSI_COLOR_RESET, VERSION);
}

void showMenu(void) {
    printf("\n%s"
           "1) Sistem Durumunu Görüntüle\n"
           "2) Hızlandırma Başlat\n"
           "3) Performans Modunu Değiştir\n"
           "4) Arka Plan Temizle\n"
           "5) Çıkış\n"
           "%s\n"
           "Seçiminiz (1-5): ",
           ANSI_COLOR_GREEN,
           ANSI_COLOR_RESET);
}

// ISH Shell uyumlu sistem durumu simülasyonu
void updateSystemStats(SystemState *state) {
    if (!state) return;
    
    // ISH Shell'de gerçek sistem bilgilerini alamadığımız için simüle ediyoruz
    state->cpu_usage = 30 + (rand() % 50); // 30-80 arası
    state->ram_usage = 40 + (rand() % 40); // 40-80 arası
    state->battery_level = rand() % 101;    // 0-100 arası
}

void showSystemStatus(SystemState *state) {
    if (!state) return;
    
    updateSystemStats(state);
    
    printf("\n%s=== Sistem Durumu ===%s\n\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
    
    // CPU Kullanımı
    printf("CPU: [");
    int bars = state->cpu_usage / 5;
    for (int i = 0; i < 20; i++) {
        if (i < bars) {
            printf(i < 16 ? ANSI_COLOR_GREEN : ANSI_COLOR_RED);
            printf("█");
        } else {
            printf(ANSI_COLOR_RESET "░");
        }
    }
    printf(ANSI_COLOR_RESET "] %d%%\n", state->cpu_usage);
    
    // RAM Kullanımı
    printf("RAM: [");
    bars = state->ram_usage / 5;
    for (int i = 0; i < 20; i++) {
        if (i < bars) {
            printf(i < 16 ? ANSI_COLOR_GREEN : ANSI_COLOR_RED);
            printf("█");
        } else {
            printf(ANSI_COLOR_RESET "░");
        }
    }
    printf(ANSI_COLOR_RESET "] %d%%\n", state->ram_usage);
    
    // Batarya Durumu
    printf("BAT: [");
    bars = state->battery_level / 5;
    for (int i = 0; i < 20; i++) {
        if (i < bars) {
            printf(i > 3 ? ANSI_COLOR_GREEN : ANSI_COLOR_RED);
            printf("█");
        } else {
            printf(ANSI_COLOR_RESET "░");
        }
    }
    printf(ANSI_COLOR_RESET "] %d%%\n", state->battery_level);
    
    // Performans Modu
    printf("\nPerformans Modu: %s%s%s\n",
           ANSI_COLOR_CYAN,
           state->performance_mode == 0 ? "Normal" :
           state->performance_mode == 1 ? "Yüksek Performans" : "Güç Tasarrufu",
           ANSI_COLOR_RESET);
    
    printf("\nENTER'a basın...");
    clearInputBuffer();
}

void optimizeSystem(SystemState *state) {
    if (!state) return;
    
    printf("\n%sOptimizasyon başlatılıyor...%s\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
    
    // Adım 1: Arka plan uygulamaları
    printf("\n1) Arka plan uygulamaları temizleniyor...\n");
    for (int i = 0; i < 3; i++) {
        printf("   İşleniyor... %d%%\r", (i + 1) * 33);
        fflush(stdout);
        sleep(DELAY_SHORT);
    }
    
    // Adım 2: Önbellek temizleme
    printf("\n2) Önbellek temizleniyor...\n");
    for (int i = 0; i < 3; i++) {
        printf("   Temizleniyor... %d%%\r", (i + 1) * 33);
        fflush(stdout);
        sleep(DELAY_SHORT);
    }
    
    // Adım 3: RAM optimizasyonu
    printf("\n3) RAM optimize ediliyor...\n");
    for (int i = 0; i < 3; i++) {
        printf("   Optimize ediliyor... %d%%\r", (i + 1) * 33);
        fflush(stdout);
        sleep(DELAY_SHORT);
    }
    
    // Optimize edilmiş değerleri güncelle
    state->cpu_usage = state->cpu_usage * 0.7;
    state->ram_usage = state->ram_usage * 0.6;
    
    printf("\n\n%sOptimizasyon tamamlandı!%s\n", ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
    printf("\nYeni değerler:\n");
    printf("CPU Kullanımı: %d%%\n", state->cpu_usage);
    printf("RAM Kullanımı: %d%%\n", state->ram_usage);
    
    printf("\nENTER'a basın...");
    clearInputBuffer();
}

void changePerformanceMode(SystemState *state) {
    if (!state) return;
    
    printf("\n%sPerformans Modları:%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
    printf("1) Normal Mod\n");
    printf("2) Yüksek Performans\n");
    printf("3) Güç Tasarrufu\n");
    printf("\nSeçiminiz (1-3): ");
    
    int choice;
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > 3) {
        printf("\n%sHatalı seçim!%s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
        sleep(DELAY_NORMAL);
        clearInputBuffer();
        return;
    }
    
    state->performance_mode = choice - 1;
    
    printf("\n%sMod değiştirildi: %s%s\n",
           ANSI_COLOR_GREEN,
           choice == 1 ? "Normal" :
           choice == 2 ? "Yüksek Performans" : "Güç Tasarrufu",
           ANSI_COLOR_RESET);
    
    clearInputBuffer();
    printf("\nENTER'a basın...");
    getchar();
}

void cleanBackground(SystemState *state) {
    if (!state) return;
    
    printf("\n%sArka plan temizleniyor...%s\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
    
    char *tasks[] = {
        "Arka plan uygulamaları durduruluyor",
        "Geçici dosyalar temizleniyor",
        "Sistem önbelleği temizleniyor",
        "RAM temizleniyor"
    };
    
    for (int i = 0; i < 4; i++) {
        printf("\n%s...\n", tasks[i]);
        for (int j = 0; j < 3; j++) {
            printf("   İşleniyor... %d%%\r", (j + 1) * 33);
            fflush(stdout);
            sleep(DELAY_SHORT);
        }
    }
    
    state->ram_usage = (state->ram_usage * 2) / 3;
    
    printf("\n\n%sArka plan temizlendi!%s\n", ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
    printf("Yeni RAM Kullanımı: %d%%\n", state->ram_usage);
    
    printf("\nENTER'a basın...");
    clearInputBuffer();
}

int main(void) {
    SystemState state = {
        .cpu_usage = 50,
        .ram_usage = 60,
        .battery_level = 100,
        .performance_mode = 0,
        .is_optimizing = false
    };
    
    srand(time(NULL));
    int choice;
    
    while (1) {
        clearScreen();
        showHeader();
        showMenu();
        
        if (scanf("%d", &choice) != 1) {
            printf("\n%sHatalı giriş!%s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
            sleep(DELAY_NORMAL);
            clearInputBuffer();
            continue;
        }
        
        clearInputBuffer();
        
        switch (choice) {
            case 1:
                showSystemStatus(&state);
                break;
            case 2:
                optimizeSystem(&state);
                break;
            case 3:
                changePerformanceMode(&state);
                break;
            case 4:
                cleanBackground(&state);
                break;
            case 5:
                clearScreen();
                printf("%sProgram sonlandırılıyor...%s\n", 
                       ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
                return SUCCESS;
            default:
                printf("\n%sGeçersiz seçim!%s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
                sleep(DELAY_NORMAL);
        }
    }
    
    return SUCCESS;
}