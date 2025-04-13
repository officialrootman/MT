#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

// Sıcaklık sabitleri
#define MIN_SICAKLIK 25.0
#define MAX_SICAKLIK 45.0
#define HEDEF_SICAKLIK 30.0

// Soğutma modu türleri
typedef enum {
    NORMAL = 1,
    HIZLI = 2,
    TURBO = 3
} SogutmaModu;

// Telefon durumu yapısı
typedef struct {
    double sicaklik;
    int pil_yuzdesi;
    bool asiri_isinma;
} TelefonDurum;

// Rastgele sıcaklık üret
double rastgele_sicaklik() {
    return MIN_SICAKLIK + ((double)rand() / RAND_MAX) * (MAX_SICAKLIK - MIN_SICAKLIK);
}

// Telefon durumunu kontrol et
TelefonDurum telefon_durum_kontrol() {
    TelefonDurum durum;
    durum.sicaklik = rastgele_sicaklik();
    durum.pil_yuzdesi = rand() % 100 + 1;
    durum.asiri_isinma = durum.sicaklik > 40.0;
    return durum;
}

// Soğutma modunu seç
SogutmaModu sogutucu_mod_sec() {
    int secim;
    printf("\nSoğutma Modunu Seçin:\n");
    printf("1 - Normal Mod (Yavaş ve Sessiz)\n");
    printf("2 - Hızlı Mod (Orta Hızda)\n");
    printf("3 - Turbo Mod (Maksimum Soğutma)\n");
    printf("Seçiminiz: ");
    scanf("%d", &secim);
    
    if (secim < 1 || secim > 3) {
        printf("Geçersiz seçim! Normal mod kullanılacak.\n");
        return NORMAL;
    }
    return (SogutmaModu)secim;
}

void telefon_sogutucu() {
    srand(time(NULL));
    
    printf("\n=== Telefon Soğutucu ===\n");
    printf("Sistem başlatılıyor...\n");
    sleep(1);
    
    // Telefon durumunu kontrol et
    TelefonDurum durum = telefon_durum_kontrol();
    
    printf("\nTelefon Durum Analizi:\n");
    printf("------------------------\n");
    printf("Mevcut Sıcaklık: %.1f°C\n", durum.sicaklik);
    printf("Pil Durumu: %%%d\n", durum.pil_yuzdesi);
    printf("Durum: %s\n", durum.asiri_isinma ? "DİKKAT! Aşırı Isınma!" : "Normal çalışma sıcaklığı");
    
    if (durum.sicaklik <= HEDEF_SICAKLIK) {
        printf("\nTelefon zaten optimum sıcaklıkta çalışıyor.\n");
        printf("Soğutma işlemine gerek yok.\n");
        return;
    }
    
    // Soğutma modunu seç
    SogutmaModu mod = sogutucu_mod_sec();
    
    printf("\nSoğutma işlemi başlatılıyor...\n");
    
    // Soğutma simülasyonu
    double mevcut_sicaklik = durum.sicaklik;
    int adim = 0;
    double sogutma_hizi = (mod == NORMAL) ? 0.5 : (mod == HIZLI) ? 1.0 : 2.0;
    
    while (mevcut_sicaklik > HEDEF_SICAKLIK && adim < 10) {
        printf("\rİşlem devam ediyor: [");
        for (int i = 0; i < adim; i++) printf("###");
        for (int i = adim; i < 10; i++) printf("   ");
        printf("] %.1f°C", mevcut_sicaklik);
        fflush(stdout);
        
        mevcut_sicaklik -= sogutma_hizi;
        adim++;
        usleep(500000); // 500ms bekle
    }
    
    printf("\n\nSoğutma işlemi tamamlandı!\n");
    printf("Son sıcaklık: %.1f°C\n", mevcut_sicaklik);
    
    // Öneriler
    printf("\nÖneriler:\n");
    printf("- Telefonu doğrudan güneş ışığından uzak tutun\n");
    printf("- Arka planda çalışan gereksiz uygulamaları kapatın\n");
    printf("- Telefon kılıfını çıkararak daha iyi havalandırma sağlayın\n");
    if (durum.pil_yuzdesi < 20) {
        printf("- Pil seviyesi düşük! Şarj etmeniz önerilir\n");
    }
}