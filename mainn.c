#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void temizle_ekran() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void telefon_sogutucu() {
    printf("\n=== Telefon Soğutucu ===\n");
    printf("Telefon soğutucu çalıştırılıyor...\n");
    printf("İşlem tamamlandı!\n");
}

void hoparlor_temizleyici() {
    printf("\n=== Hoparlör Temizleyici ===\n");
    printf("Hoparlör temizleyici çalıştırılıyor...\n");
    printf("İşlem tamamlandı!\n");
}

void menu_goster() {
    printf("\n=== Ana Menü ===\n");
    printf("1 - Telefon Soğutucu\n");
    printf("2 - Hoparlör Temizleyici\n");
    printf("3 - Çıkış\n");
    printf("\nSeçiminiz (1-3): ");
}

int secim_al() {
    int secim;
    char buffer[256];
    
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (sscanf(buffer, "%d", &secim) == 1) {
            return secim;
        }
    }
    return -1;
}

int main() {
    int secim;
    bool devam = true;
    
    while (devam) {
        temizle_ekran();
        menu_goster();
        
        // Input buffer'ı temizle
        fflush(stdin);
        
        secim = secim_al();
        
        switch (secim) {
            case 1:
                telefon_sogutucu();
                break;
            case 2:
                hoparlor_temizleyici();
                break;
            case 3:
                printf("\nProgram sonlandırılıyor...\n");
                devam = false;
                break;
            default:
                printf("\nHata: Geçersiz seçim! Lütfen 1-3 arasında bir sayı girin.\n");
                break;
        }
        
        if (devam) {
            printf("\nDevam etmek için Enter tuşuna basın...");
            getchar();
        }
    }
    
    return 0;
}