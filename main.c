// ağ_izleyici.c
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>

// Global değişkenler
static volatile int calisiyor = 1;
static unsigned long toplam_paket = 0;
static unsigned long tcp_paket = 0;
static unsigned long udp_paket = 0;
static unsigned long icmp_paket = 0;
static unsigned long diger_paket = 0;
static FILE *kayit_dosyasi = NULL;

// IP istatistikleri için yapı
typedef struct {
    char kaynak_ip[16];
    char hedef_ip[16];
    unsigned int sayac;
} ip_istatistik;

#define MAKS_IP_ISTATISTIK 1000
ip_istatistik ip_istatistikleri[MAKS_IP_ISTATISTIK];
int ip_istatistik_sayisi = 0;

void sinyal_yakalayici(int signo) {
    calisiyor = 0;
}

// IP istatistiklerini güncelleme
void ip_istatistik_guncelle(const char *kaynak_ip, const char *hedef_ip) {
    int bulundu = 0;
    for (int i = 0; i < ip_istatistik_sayisi; i++) {
        if (strcmp(ip_istatistikleri[i].kaynak_ip, kaynak_ip) == 0 &&
            strcmp(ip_istatistikleri[i].hedef_ip, hedef_ip) == 0) {
            ip_istatistikleri[i].sayac++;
            bulundu = 1;
            break;
        }
    }
    
    if (!bulundu && ip_istatistik_sayisi < MAKS_IP_ISTATISTIK) {
        strncpy(ip_istatistikleri[ip_istatistik_sayisi].kaynak_ip, kaynak_ip, 16);
        strncpy(ip_istatistikleri[ip_istatistik_sayisi].hedef_ip, hedef_ip, 16);
        ip_istatistikleri[ip_istatistik_sayisi].sayac = 1;
        ip_istatistik_sayisi++;
    }
}

// Ethernet başlığını yazdırma
void ethernet_basligi_yazdir(const u_char *paket) {
    struct ether_header *eth_basligi = (struct ether_header *)paket;
    printf("Ethernet Başlığı:\n");
    printf("   Kaynak MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           eth_basligi->ether_shost[0], eth_basligi->ether_shost[1],
           eth_basligi->ether_shost[2], eth_basligi->ether_shost[3],
           eth_basligi->ether_shost[4], eth_basligi->ether_shost[5]);
    printf("   Hedef MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           eth_basligi->ether_dhost[0], eth_basligi->ether_dhost[1],
           eth_basligi->ether_dhost[2], eth_basligi->ether_dhost[3],
           eth_basligi->ether_dhost[4], eth_basligi->ether_dhost[5]);
}

// Paket işleyici
void paket_isleyici(u_char *args, const struct pcap_pkthdr *baslik, const u_char *paket) {
    struct ip *ip_basligi = (struct ip *)(paket + ETHER_HDR_LEN);
    char zaman_damgasi[26];
    time_t simdi = time(NULL);
    strftime(zaman_damgasi, sizeof(zaman_damgasi), "%Y-%m-%d %H:%M:%S", localtime(&simdi));
    
    toplam_paket++;

    printf("\n=== Paket #%lu Yakalandı: %s ===\n", toplam_paket, zaman_damgasi);
    printf("Paket Uzunluğu: %d bayt\n", baslik->len);
    
    ethernet_basligi_yazdir(paket);
    
    char kaynak_ip[INET_ADDRSTRLEN], hedef_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_basligi->ip_src), kaynak_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_basligi->ip_dst), hedef_ip, INET_ADDRSTRLEN);
    
    printf("IP Başlığı:\n");
    printf("   Sürüm: %d\n", ip_basligi->ip_v);
    printf("   Başlık Uzunluğu: %d bayt\n", ip_basligi->ip_hl * 4);
    printf("   Kaynak IP: %s\n", kaynak_ip);
    printf("   Hedef IP: %s\n", hedef_ip);
    printf("   TTL: %d\n", ip_basligi->ip_ttl);
    
    ip_istatistik_guncelle(kaynak_ip, hedef_ip);

    switch (ip_basligi->ip_p) {
        case IPPROTO_TCP: {
            tcp_paket++;
            struct tcphdr *tcp_basligi = (struct tcphdr *)(paket + ETHER_HDR_LEN + ip_basligi->ip_hl * 4);
            printf("Protokol: TCP\n");
            printf("   Kaynak Port: %d\n", ntohs(tcp_basligi->source));
            printf("   Hedef Port: %d\n", ntohs(tcp_basligi->dest));
            printf("   Sıra Numarası: %u\n", ntohl(tcp_basligi->seq));
            printf("   ACK Numarası: %u\n", ntohl(tcp_basligi->ack_seq));
            printf("   Bayraklar: ");
            if (tcp_basligi->fin) printf("FIN ");
            if (tcp_basligi->syn) printf("SYN ");
            if (tcp_basligi->rst) printf("RST ");
            if (tcp_basligi->psh) printf("PSH ");
            if (tcp_basligi->ack) printf("ACK ");
            if (tcp_basligi->urg) printf("URG ");
            printf("\n");
            break;
        }
        case IPPROTO_UDP: {
            udp_paket++;
            struct udphdr *udp_basligi = (struct udphdr *)(paket + ETHER_HDR_LEN + ip_basligi->ip_hl * 4);
            printf("Protokol: UDP\n");
            printf("   Kaynak Port: %d\n", ntohs(udp_basligi->source));
            printf("   Hedef Port: %d\n", ntohs(udp_basligi->dest));
            printf("   Uzunluk: %d\n", ntohs(udp_basligi->len));
            break;
        }
        case IPPROTO_ICMP: {
            icmp_paket++;
            struct icmphdr *icmp_basligi = (struct icmphdr *)(paket + ETHER_HDR_LEN + ip_basligi->ip_hl * 4);
            printf("Protokol: ICMP\n");
            printf("   Tip: %d\n", icmp_basligi->type);
            printf("   Kod: %d\n", icmp_basligi->code);
            break;
        }
        default:
            diger_paket++;
            printf("Protokol: Diğer (%d)\n", ip_basligi->ip_p);
            break;
    }

    if (kayit_dosyasi) {
        fprintf(kayit_dosyasi, "%s,Paket #%lu,%s,%s,%d\n",
                zaman_damgasi, toplam_paket, kaynak_ip, hedef_ip, ip_basligi->ip_p);
        fflush(kayit_dosyasi);
    }
}

void istatistikleri_yazdir() {
    printf("\n=== Yakalama İstatistikleri ===\n");
    printf("Toplam Paket: %lu\n", toplam_paket);
    printf("TCP Paketleri: %lu (%%%.2f)\n", tcp_paket, (float)tcp_paket/toplam_paket*100);
    printf("UDP Paketleri: %lu (%%%.2f)\n", udp_paket, (float)udp_paket/toplam_paket*100);
    printf("ICMP Paketleri: %lu (%%%.2f)\n", icmp_paket, (float)icmp_paket/toplam_paket*100);
    printf("Diğer Paketler: %lu (%%%.2f)\n", diger_paket, (float)diger_paket/toplam_paket*100);
    
    printf("\nEn Çok Görülen IP Bağlantıları:\n");
    for (int i = 0; i < ip_istatistik_sayisi && i < 10; i++) {
        printf("%s -> %s: %u paket\n",
               ip_istatistikleri[i].kaynak_ip,
               ip_istatistikleri[i].hedef_ip,
               ip_istatistikleri[i].sayac);
    }
}

int main() {
    char hata_mesaji[PCAP_ERRBUF_SIZE];
    pcap_if_t *tum_aygitlar, *aygit;
    char kayit_dosyasi_adi[100];

    signal(SIGINT, sinyal_yakalayici);

    if (pcap_findalldevs(&tum_aygitlar, hata_mesaji) == -1) {
        fprintf(stderr, "Aygıt bulma hatası: %s\n", hata_mesaji);
        return 1;
    }

    printf("Kullanılabilir Ağ Arayüzleri:\n");
    int aygit_sayisi = 0;
    for (aygit = tum_aygitlar; aygit; aygit = aygit->next) {
        printf("[%d] %s", ++aygit_sayisi, aygit->name);
        if (aygit->description)
            printf(" - %s", aygit->description);
        printf("\n");
    }

    if (aygit_sayisi == 0) {
        printf("Aygıt bulunamadı! Yetki seviyenizi kontrol edin.\n");
        return 1;
    }

    int secim;
    printf("\nArayüz seçin (1-%d): ", aygit_sayisi);
    scanf("%d", &secim);

    if (secim < 1 || secim > aygit_sayisi) {
        printf("Geçersiz seçim. Çıkılıyor.\n");
        return 1;
    }

    aygit = tum_aygitlar;
    for (int i = 1; i < secim; i++) {
        aygit = aygit->next;
    }

    snprintf(kayit_dosyasi_adi, sizeof(kayit_dosyasi_adi), "paket_yakala_%ld.csv", time(NULL));
    kayit_dosyasi = fopen(kayit_dosyasi_adi, "w");
    if (kayit_dosyasi) {
        fprintf(kayit_dosyasi, "Zaman,Paket Numarası,Kaynak IP,Hedef IP,Protokol\n");
    }

    pcap_t *yakala = pcap_open_live(aygit->name, BUFSIZ, 1, 1000, hata_mesaji);
    if (yakala == NULL) {
        fprintf(stderr, "Aygıt açılamadı %s: %s\n", aygit->name, hata_mesaji);
        return 1;
    }

    printf("Paket filtresi girin (örn: 'tcp', 'udp port 53', 'icmp' veya tümü için Enter): ");
    char filtre[100];
    getchar();
    fgets(filtre, sizeof(filtre), stdin);
    filtre[strcspn(filtre, "\n")] = 0;

    if (strlen(filtre) > 0) {
        struct bpf_program fp;
        if (pcap_compile(yakala, &fp, filtre, 0, PCAP_NETMASK_UNKNOWN) == -1) {
            fprintf(stderr, "Filtre derleme hatası: %s\n", pcap_geterr(yakala));
            return 1;
        }
        if (pcap_setfilter(yakala, &fp) == -1) {
            fprintf(stderr, "Filtre uygulama hatası: %s\n", pcap_geterr(yakala));
            return 1;
        }
        printf("Filtre uygulandı: %s\n", filtre);
    }

    printf("\n%s üzerinde paket yakalama başlatılıyor...\n", aygit->name);
    printf("Durdurmak için Ctrl+C'ye basın\n\n");

    while (calisiyor) {
        pcap_dispatch(yakala, -1, paket_isleyici, NULL);
    }

    istatistikleri_yazdir();

    if (kayit_dosyasi) {
        printf("\nYakalama kaydı şuraya kaydedildi: %s\n", kayit_dosyasi_adi);
        fclose(kayit_dosyasi);
    }
    
    pcap_close(yakala);
    pcap_freealldevs(tum_aygitlar);
    return 0;
}