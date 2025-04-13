#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// ANSI Renk Kodları
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// ISH Shell'de çalışması için değişiklikler:
// 1. Ekran temizleme için system("clear") kullanımı
// 2. sleep() fonksiyonu için unistd.h kullanımı
// 3. Terminal renklendirme için ANSI renk kodları

void clearScreen() {
    system("clear");
}

void printHeader() {
    printf(ANSI_COLOR_BLUE);
    printf("================================\n");
    printf("=         TELEFON SOĞUTUCU    =\n");
    printf("=          ISH Shell v1.0     =\n");
    printf("================================\n");
    printf(ANSI_COLOR_RESET);
}

void printMenu() {
    printf("\n");
    printf(ANSI_COLOR_GREEN);
    printf("1. Soğutucuyu Başlat\n");
    printf("2. Sıcaklığı Kontrol Et\n");
    printf("3. Fan Hızını Ayarla\n");
    printf("4. Çıkış\n");
    printf(ANSI_COLOR_RESET);
    printf("\nSeçiminiz: ");
}

float simulateTemperature() {
    // Simüle edilmiş sıcaklık değeri (30-45 derece arası)
    return 30.0 + (rand() % 16);
}

void showTemperature() {
    float temp = simulateTemperature();
    printf(ANSI_COLOR_YELLOW);
    printf("\nMevcut Sıcaklık: %.1f°C\n", temp);
    printf(ANSI_COLOR_RESET);
    
    if (temp > 40) {
        printf(ANSI_COLOR_RED);
        printf("UYARI: Yüksek Sıcaklık!\n");
        printf(ANSI_COLOR_RESET);
    }
    
    printf("\nDevam etmek için ENTER tuşuna basın...");
    getchar();
    getchar();
}

void startCooling() {
    printf(ANSI_COLOR_GREEN);
    printf("\nSoğutucu başlatılıyor...\n");
    printf(ANSI_COLOR_RESET);
    sleep(2);
    
    for (int i = 0; i < 3; i++) {
        printf("Fan hızı ayarlanıyor: %d%%\n", (i + 1) * 33);
        sleep(1);
    }
    
    printf(ANSI_COLOR_GREEN);
    printf("\nSoğutucu aktif!\n");
    printf(ANSI_COLOR_RESET);
    printf("\nDevam etmek için ENTER tuşuna basın...");
    getchar();
    getchar();
}

void adjustFanSpeed() {
    int speed;
    printf("\nFan hızını giriniz (0-100): ");
    scanf("%d", &speed);
    
    if (speed >= 0 && speed <= 100) {
        printf(ANSI_COLOR_GREEN);
        printf("\nFan hızı %d%% olarak ayarlandı.\n", speed);
        printf(ANSI_COLOR_RESET);
    } else {
        printf(ANSI_COLOR_RED);
        printf("\nGeçersiz hız değeri! (0-100 arası olmalı)\n");
        printf(ANSI_COLOR_RESET);
    }
    
    printf("\nDevam etmek için ENTER tuşuna basın...");
    getchar();
    getchar();
}

int main() {
    srand(time(NULL));
    int choice;
    
    while (1) {
        clearScreen();
        printHeader();
        printMenu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                startCooling();
                break;
            case 2:
                showTemperature();
                break;
            case 3:
                adjustFanSpeed();
                break;
            case 4:
                printf("\nProgram sonlandırılıyor...\n");
                return 0;
            default:
                printf(ANSI_COLOR_RED);
                printf("\nGeçersiz seçim!\n");
                printf(ANSI_COLOR_RESET);
                sleep(2);
        }
    }
    
    return 0;
}