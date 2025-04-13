#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define MAX_COMMAND_LENGTH 100

// Global değişkenler
volatile sig_atomic_t running = 1;

// Sinyal yakalayıcı
void signal_handler(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        running = 0;
    }
}

// Sistem komutlarını çalıştırmak için yardımcı fonksiyon
void execute_command(const char* command) {
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        perror("Command execution failed");
        return;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, pipe) != NULL) {
        printf("%s", buffer);
    }

    pclose(pipe);
}

// Sistem bilgilerini görüntüle
void display_system_info() {
    printf("\n=== System Information ===\n");
    execute_command("uname -a");
    execute_command("cat /proc/cpuinfo | grep 'model name' | head -n1");
    execute_command("free -h");
    execute_command("df -h /");
}

// Sıcaklık bilgisini al (simüle edilmiş)
float get_temperature() {
    // ISH'de gerçek sıcaklık sensörü olmadığı için simüle ediyoruz
    time_t t;
    srand((unsigned) time(&t));
    return 20.0 + ((float)(rand() % 100) / 10.0); // 20-30°C arası
}

// Zaman bilgisini formatla
void get_formatted_time(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

int main() {
    char time_buffer[26];
    char command[MAX_COMMAND_LENGTH];

    // Sinyal yakalayıcıları ayarla
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("ISH Shell System Monitor\n");
    printf("Press Ctrl+C to exit\n");

    while (running) {
        get_formatted_time(time_buffer, sizeof(time_buffer));
        float temp = get_temperature();

        printf("\n=== Status Update: %s ===\n", time_buffer);
        printf("Simulated Temperature: %.1f°C\n", temp);
        
        // System load
        printf("\nSystem Load:\n");
        execute_command("uptime");
        
        // Process list
        printf("\nTop Processes:\n");
        execute_command("ps aux | head -n5");

        // Memory usage
        printf("\nMemory Usage:\n");
        execute_command("free -m | head -n2");

        // Disk usage
        printf("\nDisk Usage:\n");
        execute_command("df -h / | tail -n1");

        sleep(5); // 5 saniye bekle
        printf("\033[2J\033[H"); // Ekranı temizle
    }

    printf("\nProgram terminated.\n");
    return 0;
}