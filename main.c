#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Function to read temperature (simulated)
int read_temperature() {
    // Simulating temperature between 20°C and 70°C
    return (rand() % 50) + 20;
}

// Function to control cooler
void control_cooler(int state) {
    if (state == 1) {
        printf("Soğutucu açıldı.\n");
        // Here you would add actual hardware control code (e.g., GPIO or fan control)
    } else {
        printf("Soğutucu kapatıldı.\n");
    }
}

int main() {
    // Initialize random number generator
    srand(time(NULL));
    
    // Temperature threshold
    const int TEMP_THRESHOLD = 40;
    
    // Main loop
    while (1) {
        // Read temperature
        int temperature = read_temperature();
        printf("Okunan sıcaklık: %d°C\n", temperature);
        
        // Control cooler based on temperature
        if (temperature > TEMP_THRESHOLD) {
            control_cooler(1); // Turn cooler on
        } else {
            control_cooler(0); // Turn cooler off
        }
        
        // Wait for 2 seconds
        sleep(2);
    }
    
    return 0;
}