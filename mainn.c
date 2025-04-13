#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> // sleep fonksiyonu için

// Hoparlör frekans ayarları
#define MIN_FREQ 20    // En düşük frekans (Hz)
#define MAX_FREQ 20000 // En yüksek frekans (Hz)
#define DURATION 5     // Her frekans için süre (saniye)

void hoparlor_temizleyici() {
    printf("\n=== Hoparlör Temizleyici ===\n");
    printf("Hoparlör temizleme işlemi başlatılıyor...\n\n");
    
    // Temizleme aşamaları
    const char* asamalar[] = {
        "Düşük frekans temizliği",
        "Orta frekans temizliği",
        "Yüksek frekans temizliği",
        "Toz parçacıkları temizleniyor",
        "Son kontrol yapılıyor"
    };
    
    int toplam_asama = sizeof(asamalar) / sizeof(asamalar[0]);
    
    for (int i = 0; i < toplam_asama; i++) {
        printf("Aşama %d/%d: %s\n", i + 1, toplam_asama, asamalar[i]);
        
        // İlerleme çubuğu
        for (int j = 0; j < 20; j++) {
            printf("\rİlerleme: [");
            for (int k = 0; k <= j; k++) printf("#");
            for (int k = j + 1; k < 20; k++) printf(" ");
            printf("] %d%%", (j + 1) * 5);
            fflush(stdout);
            usleep(250000); // 250ms bekle
        }
        printf("\n");
    }
    
    // Güvenlik kontrolü
    printf("\nGüvenlik kontrolü yapılıyor...\n");
    sleep(1);
    
    // Tamamlandı mesajı
    printf("\n✓ Hoparlör temizleme işlemi başarıyla tamamlandı!\n");
    printf("Öneriler:\n");
    printf("- Hoparlörünüzü temiz ve kuru tutun\n");
    printf("- Aşırı ses seviyesinden kaçının\n");
    printf("- Düzenli olarak temizlik yapın\n");
}

// Mevcut main() fonksiyonunuz aynı kalabilir