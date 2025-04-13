#!/bin/bash

# Güvenlik ayarları
set -o nounset  # Tanımlanmamış değişkenleri hata olarak değerlendir
set -o pipefail # Pipeline'daki hataları yakala
umask 077      # Güvenli dosya izinleri

# Script bilgileri
SCRIPT_VERSION="1.0"
LAST_UPDATED="2025-04-13 13:02:01"
AUTHOR="officialrootman"

# Renkli çıktılar için yardımcı işlevler
info() { echo -e "\e[34m[INFO]\e[0m $1"; }
success() { echo -e "\e[32m[SUCCESS]\e[0m $1"; }
error() { echo -e "\e[31m[ERROR]\e[0m $1"; }
warning() { echo -e "\e[33m[WARNING]\e[0m $1"; }

# Bağımlılık kontrolü
check_dependencies() {
    local deps=("nmap" "openssl" "sshpass" "grep" "find" "sha256sum")
    for dep in "${deps[@]}"; do
        if ! command -v "$dep" &> /dev/null; then
            error "$dep bulunamadı. Lütfen yükleyin."
            exit 1
        fi
    done
}

# Dosya ve dizin kontrolü
check_file() {
    if [[ ! -f "$1" ]]; then
        error "Dosya bulunamadı: $1"
        return 1
    fi
    return 0
}

check_directory() {
    if [[ ! -d "$1" ]]; then
        error "Dizin bulunamadı: $1"
        return 1
    fi
    return 0
}

# IP adresi doğrulama
validate_ip() {
    local ip=$1
    if [[ ! $ip =~ ^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+(/[0-9]+)?$ ]]; then
        return 1
    fi
    return 0
}

# Ağ tarama (Nmap)
network_scan() {
    read -p "Taranacak IP aralığını girin (örnek: 192.168.1.0/24): " ip_range
    if ! validate_ip "$ip_range"; then
        error "Geçersiz IP formatı!"
        return 1
    fi
    
    info "Ağ taraması başlatılıyor..."
    if ! sudo nmap -sV "$ip_range"; then
        error "Nmap taraması başarısız oldu!"
        return 1
    fi
    success "Ağ taraması tamamlandı."
}

# Log analizi
log_analysis() {
    read -p "Analiz edilecek log dosyasının yolunu girin: " log_file
    if ! check_file "$log_file"; then return 1; fi
    
    read -p "Aranacak kelime veya desen (örnek: 'Failed password'): " pattern
    info "Log analizi başlatılıyor..."
    
    # Dosya boş mu kontrolü
    if [ ! -s "$log_file" ]; then
        warning "Log dosyası boş!"
        return 1
    fi
    
    if ! grep -q "$pattern" "$log_file"; then
        info "Belirtilen desen bulunamadı."
        return 0
    fi
    
    grep "$pattern" "$log_file" | sort | uniq -c | sort -nr
    success "Log analizi tamamlandı."
}

# Hash kontrolü
hash_check() {
    read -p "Hash alınacak dosyanın yolunu girin: " file
    if ! check_file "$file"; then return 1; fi
    
    info "SHA256 hash hesaplanıyor..."
    local hash_result
    if hash_result=$(sha256sum "$file"); then
        echo "$hash_result"
        success "Hash hesaplama tamamlandı."
    else
        error "Hash hesaplama başarısız oldu!"
    fi
}

# SSH brute force testi
ssh_bruteforce() {
    read -p "Test edilecek SSH hedefi (örnek: user@192.168.1.10): " target
    read -p "Şifre listesi dosyasının yolunu girin: " wordlist
    if ! check_file "$wordlist"; then return 1; fi
    
    local timeout=5
    local max_attempts=100
    local attempt=0
    local start_time=$(date +%s)
    
    info "Brute force testi başlatılıyor..."
    while IFS= read -r password; do
        ((attempt++))
        if [ $attempt -gt $max_attempts ]; then
            warning "Maksimum deneme sayısına ulaşıldı!"
            break
        fi
        
        info "Deneme $attempt/$max_attempts: Devam ediliyor..."
        if timeout $timeout sshpass -p "$password" ssh -o StrictHostKeyChecking=no "$target" exit 2>/dev/null; then
            success "Başarıyla giriş yapıldı: $password"
            local end_time=$(date +%s)
            info "Toplam süre: $((end_time - start_time)) saniye"
            return 0
        fi
    done < "$wordlist"
    
    error "Tüm şifreler denendi, giriş yapılamadı."
    local end_time=$(date +%s)
    info "Toplam süre: $((end_time - start_time)) saniye"
}

# Zayıf dosya izinlerini bulma
find_weak_permissions() {
    read -p "Taranacak dizin (örnek: /etc): " directory
    if ! check_directory "$directory"; then return 1; fi
    
    info "Zayıf dosya izinleri aranıyor..."
    local weak_files
    weak_files=$(find "$directory" -type f -perm -0777 -ls 2>/dev/null)
    
    if [ -n "$weak_files" ]; then
        echo "$weak_files"
        warning "Zayıf izinli dosyalar bulundu!"
    else
        success "Zayıf izinli dosya bulunamadı."
    fi
    success "Tarama tamamlandı."
}

# Dosya şifreleme
file_encrypt() {
    read -p "Şifrelenecek dosyanın yolunu girin: " file
    if ! check_file "$file"; then return 1; fi
    
    read -p "Şifreleme için parola girin: " -s password
    echo
    read -p "Parolayı tekrar girin: " -s password2
    echo
    
    if [[ "$password" != "$password2" ]]; then
        error "Parolalar eşleşmiyor!"
        return 1
    fi
    
    info "Dosya şifreleniyor..."
    if echo "$password" | openssl enc -aes-256-cbc -salt -pbkdf2 -in "$file" -out "${file}.enc" -pass stdin; then
        success "Dosya şifreleme tamamlandı: ${file}.enc"
        
        # Orijinal dosyanın güvenli silinmesi
        read -p "Orijinal dosya güvenli bir şekilde silinsin mi? (e/h): " delete_original
        if [[ "$delete_original" == "e" ]]; then
            if shred -u "$file"; then
                success "Orijinal dosya güvenli bir şekilde silindi."
            else
                error "Orijinal dosya silinemedi!"
            fi
        fi
    else
        error "Şifreleme işlemi başarısız oldu!"
        rm -f "${file}.enc"
    fi
}

# Yardım menüsü
show_help() {
    cat << EOF
CyberSec Multi-Tool v${SCRIPT_VERSION}
Yazar: ${AUTHOR}
Son Güncelleme: ${LAST_UPDATED}

Kullanım:
  1) Ağ Taraması: Belirtilen IP aralığında port ve servis taraması yapar
  2) Log Analizi: Log dosyalarında belirli desenleri arar ve analiz eder
  3) Hash Kontrolü: Dosyaların SHA256 hash değerini hesaplar
  4) SSH Brute Force Testi: SSH sunucularına brute force testi yapar
  5) Zayıf Dosya İzinleri: Sistem üzerinde zayıf izinli dosyaları bulur
  6) Dosya Şifreleme: AES-256-CBC ile dosya şifreleme yapar
  7) Yardım: Bu menüyü gösterir
  8) Çıkış: Programdan çıkar

Not: Bu tool etik hack testleri için tasarlanmıştır.
EOF
}

# Ctrl+C işleyici
trap 'echo -e "\nProgramdan çıkılıyor..."; exit 0' INT

# Ana program
main() {
    clear
    info "CyberSec Multi-Tool başlatılıyor..."
    check_dependencies
    
    while true; do
        echo -e "\nCyberSec Multi-Tool v${SCRIPT_VERSION}"
        echo "1) Ağ Taraması"
        echo "2) Log Analizi"
        echo "3) Hash Kontrolü"
        echo "4) SSH Brute Force Testi"
        echo "5) Zayıf Dosya İzinlerini Bulma"
        echo "6) Dosya Şifreleme"
        echo "7) Yardım"
        echo "8) Çıkış"
        read -p "Seçiminizi yapın: " choice
        
        case $choice in
            1) network_scan ;;
            2) log_analysis ;;
            3) hash_check ;;
            4) ssh_bruteforce ;;
            5) find_weak_permissions ;;
            6) file_encrypt ;;
            7) show_help ;;
            8) info "Program sonlandırılıyor..."; exit 0 ;;
            *) error "Geçersiz seçim!" ;;
        esac
    done
}

# Programı başlat
main