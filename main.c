#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

// Advanced color coding system
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_BOLD          "\x1b[1m"

// Enhanced system constants
#define MIN_TEMP 20.0
#define MAX_TEMP 45.0
#define CRITICAL_TEMP 38.0
#define OPTIMAL_TEMP 28.0
#define MIN_FAN_SPEED 0
#define MAX_FAN_SPEED 150
#define STARTUP_DELAY 0.5
#define FAN_SPEED_STEPS 10
#define TEMP_HISTORY_SIZE 60
#define DATA_LOG_FILE "cooling_log.csv"
#define CONFIG_FILE "cooler_config.ini"

// Advanced performance profiles
typedef enum {
    ECO_MODE,
    BALANCED,
    PERFORMANCE,
    TURBO,
    SILENT,
    SMART_AUTO,
    CUSTOM
} CoolingProfile;

// Temperature zones
typedef enum {
    ZONE_COLD,
    ZONE_OPTIMAL,
    ZONE_WARM,
    ZONE_HOT,
    ZONE_CRITICAL
} TempZone;

// Advanced cooling state structure
typedef struct {
    float current_temp;
    float temp_history[TEMP_HISTORY_SIZE];
    int temp_history_index;
    int fan_speed;
    bool is_cooling;
    CoolingProfile profile;
    time_t last_temp_check;
    float temp_trend;
    unsigned long runtime_seconds;
    struct {
        float total_power_used;
        float cooling_efficiency;
        int thermal_throttle_events;
    } statistics;
    struct {
        float fan_curve[5];
        float temp_thresholds[5];
        int custom_max_fan_speed;
    } custom_settings;
} CoolerState;

// Function prototypes
void initializeState(CoolerState *state);
void saveConfiguration(const CoolerState *state);
void loadConfiguration(CoolerState *state);
void logCoolingData(const CoolerState *state);
float calculateCoolingEfficiency(const CoolerState *state);
void updateThermalTrend(CoolerState *state);
void displayDetailedStats(const CoolerState *state);
void configureFanCurve(CoolerState *state);
void emergencyCooling(CoolerState *state);

// Initialize the cooling system state
void initializeState(CoolerState *state) {
    memset(state, 0, sizeof(CoolerState));
    state->profile = BALANCED;
    state->current_temp = simulateTemperature(state);
    state->cooling_efficiency = 100.0;
    
    // Initialize custom fan curve
    float default_curve[5] = {20.0, 40.0, 60.0, 80.0, 100.0};
    float default_thresholds[5] = {25.0, 30.0, 35.0, 38.0, 40.0};
    memcpy(state->custom_settings.fan_curve, default_curve, sizeof(default_curve));
    memcpy(state->custom_settings.temp_thresholds, default_thresholds, sizeof(default_thresholds));
    state->custom_settings.custom_max_fan_speed = MAX_FAN_SPEED;
    
    loadConfiguration(state);
}

// Advanced temperature simulation
float simulateTemperature(CoolerState *state) {
    if (!state->is_cooling) {
        return state->current_temp + (((float)rand() / RAND_MAX) * 2.0 - 0.5);
    }

    float cooling_effect = (float)state->fan_speed / MAX_FAN_SPEED;
    float ambient_factor = sinf((float)time(NULL) / 3600.0) * 2.0; // Daily temperature cycle
    float workload_factor = ((float)rand() / RAND_MAX) * 3.0; // Random workload variation
    
    float new_temp = state->current_temp;
    new_temp += workload_factor;
    new_temp += ambient_factor;
    new_temp -= (cooling_effect * 5.0);
    
    // Clamp temperature
    return fmaxf(MIN_TEMP, fminf(MAX_TEMP, new_temp));
}

// Smart fan speed controller
void adjustFanSpeedAutomatically(CoolerState *state) {
    updateThermalTrend(state);
    
    float target_speed = 0.0;
    float temp_delta = state->current_temp - OPTIMAL_TEMP;
    
    switch (state->profile) {
        case ECO_MODE:
            target_speed = fmaxf(0, temp_delta * 5.0);
            target_speed = fminf(target_speed, MAX_FAN_SPEED * 0.6);
            break;
            
        case PERFORMANCE:
            target_speed = fmaxf(20, temp_delta * 8.0);
            target_speed = fminf(target_speed, MAX_FAN_SPEED * 0.9);
            break;
            
        case TURBO:
            target_speed = fmaxf(40, temp_delta * 10.0);
            target_speed = fminf(target_speed, MAX_FAN_SPEED);
            break;
            
        case SILENT:
            target_speed = fmaxf(0, temp_delta * 3.0);
            target_speed = fminf(target_speed, MAX_FAN_SPEED * 0.4);
            break;
            
        case SMART_AUTO:
            // Consider thermal trend
            float trend_factor = state->temp_trend * 20.0;
            target_speed = fmaxf(0, (temp_delta + trend_factor) * 6.0);
            target_speed = fminf(target_speed, MAX_FAN_SPEED * 0.85);
            break;
            
        case CUSTOM:
            // Use custom fan curve
            for (int i = 0; i < 5; i++) {
                if (state->current_temp <= state->custom_settings.temp_thresholds[i]) {
                    target_speed = state->custom_settings.fan_curve[i];
                    break;
                }
            }
            target_speed = fminf(target_speed, state->custom_settings.custom_max_fan_speed);
            break;
            
        default: // BALANCED
            target_speed = fmaxf(10, temp_delta * 6.0);
            target_speed = fminf(target_speed, MAX_FAN_SPEED * 0.75);
    }
    
    // Smooth transition
    float speed_diff = target_speed - state->fan_speed;
    state->fan_speed += (int)(speed_diff * 0.2);
    
    // Emergency override
    if (state->current_temp >= CRITICAL_TEMP) {
        emergencyCooling(state);
    }
}

// Emergency cooling procedure
void emergencyCooling(CoolerState *state) {
    state->fan_speed = MAX_FAN_SPEED;
    state->statistics.thermal_throttle_events++;
    printf(ANSI_COLOR_RED ANSI_BOLD "\nKRİTİK SICAKLIK UYARISI! Acil soğutma başlatıldı!\n" ANSI_COLOR_RESET);
    logCoolingData(state);
}

// Update thermal trend analysis
void updateThermalTrend(CoolerState *state) {
    state->temp_history[state->temp_history_index] = state->current_temp;
    state->temp_history_index = (state->temp_history_index + 1) % TEMP_HISTORY_SIZE;
    
    float sum = 0;
    for (int i = 0; i < TEMP_HISTORY_SIZE; i++) {
        sum += state->temp_history[i];
    }
    float avg = sum / TEMP_HISTORY_SIZE;
    state->temp_trend = state->current_temp - avg;
}

// Display detailed statistics
void displayDetailedStats(const CoolerState *state) {
    printf("\n%s=== Detaylı Sistem İstatistikleri ===%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
    printf("Çalışma Süresi: %lu saat %lu dakika\n", 
           state->runtime_seconds / 3600, (state->runtime_seconds % 3600) / 60);
    printf("Toplam Güç Kullanımı: %.2f Watt\n", state->statistics.total_power_used);
    printf("Soğutma Verimliliği: %.1f%%\n", state->statistics.cooling_efficiency);
    printf("Termal Limit Olayları: %d\n", state->statistics.thermal_throttle_events);
    printf("Sıcaklık Trendi: %s%.1f°C/saat%s\n", 
           state->temp_trend > 0 ? ANSI_COLOR_RED : ANSI_COLOR_GREEN,
           state->temp_trend * 3600,
           ANSI_COLOR_RESET);
}

// Enhanced menu system
void printEnhancedMenu(void) {
    printf("\n%s%s=== Gelişmiş Soğutma Kontrol Sistemi ===%s\n",
           ANSI_BOLD, ANSI_COLOR_BLUE, ANSI_COLOR_RESET);
    printf("1. Soğutucuyu Başlat\n");
    printf("2. Sıcaklık ve Fan Durumu\n");
    printf("3. Soğutma Profili Seç\n");
    printf("4. Özel Fan Eğrisi Ayarla\n");
    printf("5. Detaylı İstatistikler\n");
    printf("6. Yapılandırmayı Kaydet\n");
    printf("7. Acil Durum Soğutma\n");
    printf("8. Çıkış\n");
    printf("\nSeçiminiz: ");
}

// Main function with enhanced menu handling
int main(void) {
    srand(time(NULL));
    CoolerState state;
    initializeState(&state);
    
    while (true) {
        if (!clearScreen()) {
            return ERROR_SYSTEM_CALL;
        }
        
        printEnhancedMenu();
        int choice = getMenuChoice();
        
        switch (choice) {
            case 1:
                startCooling(&state);
                break;
            case 2:
                showDetailedStatus(&state);
                break;
            case 3:
                selectCoolingProfile(&state);
                break;
            case 4:
                configureFanCurve(&state);
                break;
            case 5:
                displayDetailedStats(&state);
                break;
            case 6:
                saveConfiguration(&state);
                break;
            case 7:
                emergencyCooling(&state);
                break;
            case 8:
                printf("\nProgram sonlandırılıyor...\n");
                saveConfiguration(&state);
                return SUCCESS;
            default:
                printf(ANSI_COLOR_RED "\nGeçersiz seçim!\n" ANSI_COLOR_RESET);
                sleep(1);
        }
        
        // Update system state
        state.runtime_seconds += 1;
        state.current_temp = simulateTemperature(&state);
        if (state.is_cooling) {
            adjustFanSpeedAutomatically(&state);
        }
        logCoolingData(&state);
    }
}