#!/bin/bash

# Sıcaklık sensöründen veri okuma
function read_temperature() {
    # Hata kontrolü
    temperature=$((RANDOM % 50 + 20)) # 20°C ile 70°C arasında bir sıcaklık döndür
    if [ -z "$temperature" ]; then
        echo "Hata: Sıcaklık verisi okunamadı!"
        exit 1
    fi
    echo "$temperature"
}

# Soğutucuyu kontrol etme
function control_cooler() {
    case "$1" in
        1)
            echo "Soğutucu açıldı."
            ;;
        0)
            echo "Soğutucu kapatıldı."
            ;;
        *)
            echo "Geçersiz giriş: $1"
            ;;
    esac
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