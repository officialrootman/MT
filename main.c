#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for sleep()

// Simüle edilmiş sıcaklık okuma fonksiyonu
float read_temperature() {
    // Örnek: Rastgele bir sıcaklık değeri döndür
    return (rand() % 50) + 20; // 20°C ile 70°C arasında
}

// Soğutucu kontrol fonksiyonu
void control_cooler(int turn_on) {
    if (turn_on) {
        printf("Soğutucu açıldı.\n");
        // Burada donanım kontrol kodu yazılır (GPIO, I2C, vb.)
    } else {
        printf("Soğutucu kapatıldı.\n");
    }
}

int main() {
    const float TEMP_THRESHOLD = 40.0; // Soğutucunun açılacağı sıcaklık
    while (1) {
        float temperature = read_temperature();
        printf("Okunan sıcaklık: %.2f°C\n", temperature);

        if (temperature > TEMP_THRESHOLD) {
            control_cooler(1); // Soğutucuyu aç
        } else {
            control_cooler(0); // Soğutucuyu kapat
        }

        sleep(2); // 2 saniye bekle
    }

    return 0;
}