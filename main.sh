#!/bin/bash

# Ağ Trafiği Analiz Aracı

# Renkli çıktı için tanımlar
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Ana Menü
function show_menu() {
    echo -e "${GREEN}=== Ağ Trafiği Analiz Aracı ===${NC}"
    echo "1) Belirli bir IP adresini filtrele"
    echo "2) Belirli bir portu filtrele"
    echo "3) Tüm trafiği kaydet"
    echo "4) Çıkış"
    echo -n "Seçiminizi yapın: "
    read choice
}

# Trafiği analiz et
function analyze_traffic() {
    filter=$1
    echo -e "${YELLOW}Trafik analiz ediliyor...${NC}"
    echo -e "${GREEN}tcpdump komutu: tcpdump $filter${NC}"
    
    # tcpdump ile trafiği analiz et
    sudo tcpdump $filter -n -c 100
}

# Trafiği kaydet
function save_traffic() {
    filter=$1
    output_file="trafik_analizi_$(date +%Y%m%d_%H%M%S).pcap"
    echo -e "${YELLOW}Trafik kaydediliyor...${NC}"
    echo -e "${GREEN}tcpdump komutu: tcpdump $filter -w $output_file${NC}"
    
    # tcpdump ile trafiği kaydet
    sudo tcpdump $filter -w $output_file
    echo -e "${GREEN}Trafik ${output_file} dosyasına kaydedildi.${NC}"
}

while true; do
    show_menu
    case $choice in
        1)
            echo -n "İzlenecek IP adresini girin: "
            read ip
            analyze_traffic "host $ip"
            ;;
        2)
            echo -n "İzlenecek portu girin: "
            read port
            analyze_traffic "port $port"
            ;;
        3)
            echo "Kaydedilecek trafiği seçin: "
            echo "1) Tüm trafik"
            echo "2) Belirli bir IP adresi"
            echo "3) Belirli bir port"
            echo -n "Seçiminizi yapın: "
            read save_choice

            case $save_choice in
                1)
                    save_traffic ""
                    ;;
                2)
                    echo -n "Kaydedilecek IP adresini girin: "
                    read save_ip
                    save_traffic "host $save_ip"
                    ;;
                3)
                    echo -n "Kaydedilecek portu girin: "
                    read save_port
                    save_traffic "port $save_port"
                    ;;
                *)
                    echo -e "${RED}Geçersiz seçim!${NC}"
                    ;;
            esac
            ;;
        4)
            echo -e "${GREEN}Çıkış yapılıyor...${NC}"
            exit 0
            ;;
        *)
            echo -e "${RED}Geçersiz seçim!${NC}"
            ;;
    esac
done