#!/bin/bash

# Sıcaklık sensöründen veri okuma (örnek: bir dosyadan veya komuttan)
function read_temperature() {
    # Örnek: Sıcaklığı rastgele bir sayı olarak simüle et
    echo $((RANDOM % 50 + 20)) # 20°C ile 70°C arasında bir sıcaklık döndür
}

# Soğutucuyu kontrol etme
function control_cooler() {
    if [ "$1" -eq 1 ]; then
        echo "Soğutucu açıldı."
        # Burada gerçek donanım kontrol kodu yazılır (örneğin, GPIO veya fan kontrol komutları)
    else
        echo "Soğutucu kapatıldı."
    fi
}

# Ana döngü
TEMP_THRESHOLD=40 # Soğutucunun açılacağı sıcaklık
while true; do
    temperature=$(read_temperature)
    echo "Okunan sıcaklık: ${temperature}°C"

    # Soğutucu kontrolü
    if [ "$temperature" -gt "$TEMP_THRESHOLD" ]; then
        control_cooler 1 # Soğutucuyu aç
    else
        control_cooler 0 # Soğutucuyu kapat
    fi

    sleep 2 # 2 saniye bekle
done