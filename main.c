#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <math.h>
#include <sqlite3.h>

// Sistem Konfigürasyonu
#define VERSION "2.0.0"
#define MIN_TEMP 15.0
#define MAX_TEMP 75.0
#define DEFAULT_THRESHOLD 40.0
#define CRITICAL_TEMP 65.0
#define EMERGENCY_TEMP 70.0
#define SAMPLING_INTERVAL 1    // 1 saniye
#define HISTORY_SIZE 3600     // 1 saatlik veri
#define DATABASE_FILE "temperature_data.db"
#define CONFIG_FILE "system_config.json"
#define LOG_FILE "system.log"

// Sistem Durumları
typedef enum {
    SYSTEM_OK,
    WARNING_HIGH_TEMP,
    CRITICAL_HIGH_TEMP,
    EMERGENCY_SHUTDOWN,
    SENSOR_ERROR,
    SYSTEM_ERROR
} SystemState;

// Sıcaklık verisi yapısı
typedef struct {
    double value;
    time_t timestamp;
    SystemState state;
} TemperatureReading;

// Sistem konfigürasyonu yapısı
typedef struct {
    double threshold;
    double critical_temp;
    double emergency_temp;
    int sampling_interval;
    int logging_enabled;
    int database_enabled;
    char sensor_type[32];
    char cooler_type[32];
} SystemConfig;

// Global değişkenler
volatile sig_atomic_t running = 1;
pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t config_mutex = PTHREAD_MUTEX_INITIALIZER;
TemperatureReading *temp_history;
SystemConfig config;
sqlite3 *db;

// İleri fonksiyon bildirimleri
void init_system(void);
void cleanup_system(void);
double read_temperature(void);
void control_cooler(int state);
void log_event(const char *format, ...);
void update_system_state(SystemState new_state);
void handle_emergency(void);
void *monitoring_thread(void *arg);
void *database_thread(void *arg);
void create_database(void);
void save_temperature_to_db(TemperatureReading reading);
double calculate_moving_average(void);
SystemState evaluate_system_state(double temp);
void print_status(TemperatureReading current, double avg_temp);

// Sistem başlatma
void init_system(void) {
    printf("\033[2J\033[H"); // Ekranı temizle
    printf("=== Soğutma Kontrol Sistemi v%s ===\n", VERSION);
    printf("Sistem başlatılıyor...\n");

    // Bellek tahsisi
    temp_history = calloc(HISTORY_SIZE, sizeof(TemperatureReading));
    if (!temp_history) {
        log_event("HATA: Bellek tahsisi başarısız");
        exit(1);
    }

    // Veritabanı oluşturma
    create_database();

    // Sinyal işleyicileri
    signal(SIGINT, (void (*)(int))cleanup_system);
    signal(SIGTERM, (void (*)(int))cleanup_system);

    // Varsayılan konfigürasyon
    config.threshold = DEFAULT_THRESHOLD;
    config.critical_temp = CRITICAL_TEMP;
    config.emergency_temp = EMERGENCY_TEMP;
    config.sampling_interval = SAMPLING_INTERVAL;
    config.logging_enabled = 1;
    config.database_enabled = 1;
    strcpy(config.sensor_type, "DS18B20");
    strcpy(config.cooler_type, "PWM_FAN");

    srand(time(NULL));
    log_event("Sistem başlatıldı");
}

// Gelişmiş sıcaklık okuma
double read_temperature(void) {
    int retries = 3;
    double temp;
    
    while (retries--) {
        // Simüle edilmiş sıcaklık okuması
        temp = (double)(rand() % 550 + 150) / 10.0; // 15.0 - 70.0 arası
        
        if (temp >= MIN_TEMP && temp <= MAX_TEMP) {
            return temp;
        }
        
        log_event("UYARI: Geçersiz sıcaklık okuması (%.1f°C), yeniden deneniyor...", temp);
        usleep(100000); // 100ms bekle
    }
    
    log_event("HATA: Sensör okuma hatası");
    return -1.0;
}

// Gelişmiş soğutucu kontrolü
void control_cooler(int state) {
    static int current_state = -1;
    static time_t last_switch = 0;
    time_t now = time(NULL);
    
    // Durumların çok hızlı değişmesini önle
    if (state == current_state || (now - last_switch < 5 && state != 2)) {
        return;
    }
    
    pthread_mutex_lock(&config_mutex);
    
    switch (state) {
        case 0: // Kapalı
            printf("\033[31mSoğutucu: KAPALI\033[0m\n");
            // GPIO pin kontrolü burada
            break;
            
        case 1: // Açık (Normal mod)
            printf("\033[32mSoğutucu: AÇIK (Normal mod)\033[0m\n");
            // GPIO pin kontrolü burada
            break;
            
        case 2: // Acil durum modu (Maksimum güç)
            printf("\033[1;31mSoğutucu: ACİL DURUM MODU\033[0m\n");
            // GPIO pin kontrolü burada
            break;
            
        default:
            log_event("HATA: Geçersiz soğutucu durumu (%d)", state);
            pthread_mutex_unlock(&config_mutex);
            return;
    }
    
    current_state = state;
    last_switch = now;
    pthread_mutex_unlock(&config_mutex);
    
    log_event("Soğutucu durumu değiştirildi: %d", state);
}

// Sistem durumu değerlendirme
SystemState evaluate_system_state(double temp) {
    if (temp < 0) return SENSOR_ERROR;
    
    if (temp >= config.emergency_temp) return EMERGENCY_SHUTDOWN;
    if (temp >= config.critical_temp) return CRITICAL_HIGH_TEMP;
    if (temp >= config.threshold) return WARNING_HIGH_TEMP;
    
    return SYSTEM_OK;
}

// Gelişmiş durum gösterimi
void print_status(TemperatureReading current, double avg_temp) {
    static int update_counter = 0;
    time_t now = time(NULL);
    char time_str[26];
    ctime_r(&now, time_str);
    time_str[24] = '\0';

    // Her 10 güncellemede bir ekranı temizle
    if (++update_counter % 10 == 0) {
        printf("\033[2J\033[H");
    }

    printf("\033[H"); // Ekranın başına git
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║             SOĞUTMA SİSTEMİ KONTROL PANELİ            ║\n");
    printf("╠════════════════════════════════════════════════════════╣\n");
    printf("║ Zaman: %-47s ║\n", time_str);
    printf("║ Sıcaklık: %6.1f°C ", current.value);
    
    // Sıcaklık durumuna göre renklendirme
    switch (current.state) {
        case SYSTEM_OK:
            printf("\033[32m[NORMAL]\033[0m");
            break;
        case WARNING_HIGH_TEMP:
            printf("\033[33m[YÜKSEK]\033[0m");
            break;
        case CRITICAL_HIGH_TEMP:
            printf("\033[31m[KRİTİK]\033[0m");
            break;
        case EMERGENCY_SHUTDOWN:
            printf("\033[1;31m[ACİL DURUM]\033[0m");
            break;
        default:
            printf("\033[31m[HATA]\033[0m");
    }
    
    printf("%25s ║\n", "");
    printf("║ Ortalama: %6.1f°C %37s ║\n", avg_temp, "");
    printf("║ Eşik:     %6.1f°C %37s ║\n", config.threshold, "");
    printf("║ Kritik:   %6.1f°C %37s ║\n", config.critical_temp, "");
    printf("╠════════════════════════════════════════════════════════╣\n");
    printf("║ Sistem Durumu: ");
    
    switch (current.state) {
        case SYSTEM_OK:
            printf("\033[32mSistem normal çalışıyor\033[0m");
            break;
        case WARNING_HIGH_TEMP:
            printf("\033[33mYüksek sıcaklık uyarısı\033[0m");
            break;
        case CRITICAL_HIGH_TEMP:
            printf("\033[31mKRİTİK SICAKLIK UYARISI!\033[0m");
            break;
        case EMERGENCY_SHUTDOWN:
            printf("\033[1;31mACİL DURUM - SİSTEM KAPATILIYOR!\033[0m");
            break;
        case SENSOR_ERROR:
            printf("\033[31mSENSÖR HATASI\033[0m");
            break;
        case SYSTEM_ERROR:
            printf("\033[31mSİSTEM HATASI\033[0m");
            break;
    }
    
    printf("%15s ║\n", "");
    printf("╚════════════════════════════════════════════════════════╝\n");
}

// Ana döngü
int main() {
    init_system();
    
    pthread_t monitor_thread, db_thread;
    pthread_create(&monitor_thread, NULL, monitoring_thread, NULL);
    pthread_create(&db_thread, NULL, database_thread, NULL);
    
    TemperatureReading current;
    int consecutive_errors = 0;
    
    while (running) {
        current.timestamp = time(NULL);
        current.value = read_temperature();
        
        if (current.value < 0) {
            if (++consecutive_errors >= 3) {
                update_system_state(SENSOR_ERROR);
                handle_emergency();
                break;
            }
            continue;
        }
        
        consecutive_errors = 0;
        current.state = evaluate_system_state(current.value);
        
        pthread_mutex_lock(&data_mutex);
        memmove(&temp_history[1], &temp_history[0], 
                (HISTORY_SIZE - 1) * sizeof(TemperatureReading));
        temp_history[0] = current;
        pthread_mutex_unlock(&data_mutex);
        
        double avg_temp = calculate_moving_average();
        
        // Soğutucu kontrolü
        switch (current.state) {
            case SYSTEM_OK:
                control_cooler(0);
                break;
            case WARNING_HIGH_TEMP:
                control_cooler(1);
                break;
            case CRITICAL_HIGH_TEMP:
            case EMERGENCY_SHUTDOWN:
                control_cooler(2);
                handle_emergency();
                break;
            default:
                control_cooler(0);
        }
        
        print_status(current, avg_temp);
        
        if (config.database_enabled) {
            save_temperature_to_db(current);
        }
        
        sleep(config.sampling_interval);
    }
    
    pthread_join(monitor_thread, NULL);
    pthread_join(db_thread, NULL);
    cleanup_system();
    
    return 0;
}

// Sistem temizleme
void cleanup_system(void) {
    running = 0;
    log_event("Sistem kapatılıyor...");
    
    control_cooler(0);
    
    if (temp_history) {
        free(temp_history);
    }
    
    if (db) {
        sqlite3_close(db);
    }
    
    pthread_mutex_destroy(&data_mutex);
    pthread_mutex_destroy(&config_mutex);
    
    log_event("Sistem güvenli bir şekilde kapatıldı");
    printf("\nSistem güvenli bir şekilde kapatıldı.\n");
}