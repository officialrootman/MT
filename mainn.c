#include <stdio.h>

void telefon_sogutucu() {
    printf("Telefon soğutucu çalıştırılıyor...\n");
    // Telefon soğutucuya ait işlemleri buraya ekleyin
}

void hoparlor_temizleyici() {
    printf("Hoparlör temizleyici çalıştırılıyor...\n");
    // Hoparlör temizleyiciye ait işlemleri buraya ekleyin
}

int main() {
    int secim;

    printf("Lütfen bir seçenek seçin:\n");
    printf("1 - Telefon Soğutucu\n");
    printf("2 - Hoparlör Temizleyici\n");
    printf("Seçiminiz: ");
    scanf("%d", &secim);

    switch (secim) {
        case 1:
            telefon_sogutucu();
            break;
        case 2:
            hoparlor_temizleyici();
            break;
        default:
            printf("Geçersiz bir seçim yaptınız.\n");
            break;
    }

    return 0;
}