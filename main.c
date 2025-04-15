#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <syslog.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_CONNECTIONS 100
#define LOG_FILE "honeypot.log"
#define BANNED_IPS_FILE "banned_ips.txt"
#define BAN_THRESHOLD 5
#define BAN_DURATION 3600 // 1 saat

typedef struct {
    char ip[INET_ADDRLEN];
    time_t last_attempt;
    int attempt_count;
} IPRecord;

// Global değişkenler
IPRecord *ip_records;
int ip_record_count = 0;
pthread_mutex_t ip_mutex = PTHREAD_MUTEX_INITIALIZER;
FILE *log_file;

// Fonksiyon prototipleri
void setup_signal_handlers(void);
void log_activity(const char *ip, const char *message, const char *payload);
void check_and_ban_ip(const char *ip);
int is_ip_banned(const char *ip);
void save_banned_ips(void);
void load_banned_ips(void);
void cleanup(void);

void log_activity(const char *ip, const char *message, const char *payload) {
    time_t now;
    struct tm *timeinfo;
    char timestamp[64];

    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    pthread_mutex_lock(&ip_mutex);
    fprintf(log_file, "[%s] IP: %s - %s - Payload: %s\n", timestamp, ip, message, payload);
    fflush(log_file);
    pthread_mutex_unlock(&ip_mutex);
    
    // Sistem log'una da kaydet
    syslog(LOG_AUTH | LOG_NOTICE, "Honeypot: %s - %s - %s", ip, message, payload);
}

void handle_connection(int client_socket, struct sockaddr_in *client_addr) {
    char buffer[BUFFER_SIZE] = {0};
    char client_ip[INET_ADDRLEN];
    char *responses[] = {
        "Access Denied: Invalid credentials\n",
        "Error: Service temporarily unavailable\n",
        "Authentication failed: Please try again later\n",
        "System is currently under maintenance\n"
    };

    inet_ntop(AF_INET, &(client_addr->sin_addr), client_ip, INET_ADDRLEN);

    // IP'yi kontrol et
    if (is_ip_banned(client_ip)) {
        close(client_socket);
        return;
    }

    // Gelen veriyi oku
    int bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        
        // Zararlı içerik kontrolü
        if (strstr(buffer, "exec") || strstr(buffer, "bash") || 
            strstr(buffer, "/bin/") || strstr(buffer, "passwd")) {
            log_activity(client_ip, "Zararlı içerik tespit edildi", buffer);
            check_and_ban_ip(client_ip);
        }

        // Aktiviteyi logla
        log_activity(client_ip, "Gelen bağlantı", buffer);
    }

    // Rastgele sahte cevap seç
    int response_index = rand() % (sizeof(responses) / sizeof(responses[0]));
    send(client_socket, responses[response_index], strlen(responses[response_index]), 0);

    // Gecikme ekle
    usleep(rand() % 1000000 + 500000); // 0.5-1.5 saniye arası

    close(client_socket);
}

void check_and_ban_ip(const char *ip) {
    pthread_mutex_lock(&ip_mutex);
    
    int found = 0;
    for (int i = 0; i < ip_record_count; i++) {
        if (strcmp(ip_records[i].ip, ip) == 0) {
            ip_records[i].attempt_count++;
            ip_records[i].last_attempt = time(NULL);
            found = 1;
            
            if (ip_records[i].attempt_count >= BAN_THRESHOLD) {
                log_activity(ip, "IP engellendi", "Çok fazla deneme");
                save_banned_ips();
            }
            break;
        }
    }

    if (!found && ip_record_count < MAX_CONNECTIONS) {
        strcpy(ip_records[ip_record_count].ip, ip);
        ip_records[ip_record_count].attempt_count = 1;
        ip_records[ip_record_count].last_attempt = time(NULL);
        ip_record_count++;
    }

    pthread_mutex_unlock(&ip_mutex);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Başlangıç ayarları
    srand(time(NULL));
    setup_signal_handlers();
    ip_records = calloc(MAX_CONNECTIONS, sizeof(IPRecord));
    log_file = fopen(LOG_FILE, "a");
    load_banned_ips();

    // Soket oluştur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Soket oluşturulamadı");
        exit(EXIT_FAILURE);
    }

    // SO_REUSEADDR ayarı
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bağlama hatası");
        cleanup();
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("Dinleme hatası");
        cleanup();
        exit(EXIT_FAILURE);
    }

    printf("Gelişmiş Honeypot v2.0 başlatıldı. Port: %d\n", PORT);
    openlog("honeypot", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Honeypot başlatıldı");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("Bağlantı kabul edilemedi");
            continue;
        }

        // Yeni thread oluştur ve bağlantıyı işle
        pthread_t thread_id;
        struct sockaddr_in *client_addr_copy = malloc(sizeof(struct sockaddr_in));
        memcpy(client_addr_copy, &client_addr, sizeof(struct sockaddr_in));
        
        if (pthread_create(&thread_id, NULL, (void *)handle_connection, (void *)new_socket) != 0) {
            perror("Thread oluşturulamadı");
            close(new_socket);
            free(client_addr_copy);
            continue;
        }
        pthread_detach(thread_id);
    }

    cleanup();
    return 0;
}

void cleanup(void) {
    if (log_file) fclose(log_file);
    if (ip_records) free(ip_records);
    closelog();
}

void setup_signal_handlers(void) {
    struct sigaction sa;
    sa.sa_handler = cleanup;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

// IP yasaklama işlemleri
void save_banned_ips(void) {
    FILE *fp = fopen(BANNED_IPS_FILE, "w");
    if (!fp) return;

    pthread_mutex_lock(&ip_mutex);
    for (int i = 0; i < ip_record_count; i++) {
        if (ip_records[i].attempt_count >= BAN_THRESHOLD) {
            fprintf(fp, "%s %ld %d\n", 
                ip_records[i].ip, 
                ip_records[i].last_attempt, 
                ip_records[i].attempt_count);
        }
    }
    pthread_mutex_unlock(&ip_mutex);
    fclose(fp);
}

void load_banned_ips(void) {
    FILE *fp = fopen(BANNED_IPS_FILE, "r");
    if (!fp) return;

    char ip[INET_ADDRLEN];
    time_t last_attempt;
    int attempts;

    while (fscanf(fp, "%s %ld %d", ip, &last_attempt, &attempts) == 3) {
        if (ip_record_count < MAX_CONNECTIONS) {
            strcpy(ip_records[ip_record_count].ip, ip);
            ip_records[ip_record_count].last_attempt = last_attempt;
            ip_records[ip_record_count].attempt_count = attempts;
            ip_record_count++;
        }
    }
    fclose(fp);
}

int is_ip_banned(const char *ip) {
    pthread_mutex_lock(&ip_mutex);
    for (int i = 0; i < ip_record_count; i++) {
        if (strcmp(ip_records[i].ip, ip) == 0) {
            if (ip_records[i].attempt_count >= BAN_THRESHOLD) {
                time_t now = time(NULL);
                if (now - ip_records[i].last_attempt < BAN_DURATION) {
                    pthread_mutex_unlock(&ip_mutex);
                    return 1;
                } else {
                    // Ban süresi dolmuş, sıfırla
                    ip_records[i].attempt_count = 0;
                }
            }
            break;
        }
    }
    pthread_mutex_unlock(&ip_mutex);
    return 0;
}