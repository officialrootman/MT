#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string.h>

// Serial port configuration function
int configure_port(int fd) {
    struct termios options;
    
    if (tcgetattr(fd, &options) == -1) {
        perror("Error getting port attributes");
        return -1;
    }
    
    // Configure port settings
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    
    // Apply settings
    if (tcsetattr(fd, TCSANOW, &options) == -1) {
        perror("Error setting port attributes");
        return -1;
    }
    
    return 0;
}

// Function to try opening serial port
int open_serial_port(const char* port_name) {
    int retry_count = 0;
    int fd;
    
    while (retry_count < 3) {  // Try 3 times
        fd = open(port_name, O_RDWR | O_NOCTTY);
        if (fd != -1) {
            printf("Serial port %s opened successfully\n", port_name);
            return fd;
        }
        
        printf("Attempt %d: Cannot open %s: %s\n", 
               retry_count + 1, port_name, strerror(errno));
        sleep(2);  // Wait 2 seconds before retrying
        retry_count++;
    }
    
    return -1;
}

int main() {
    const char* port_names[] = {
        "/dev/ttyS0",  // Standard serial port
        "/dev/ttyUSB0",  // USB-to-Serial adapter
        "/dev/ttyACM0"   // Another common serial device
    };
    int num_ports = sizeof(port_names) / sizeof(port_names[0]);
    int fd = -1;
    
    // Try different serial ports until one works
    for (int i = 0; i < num_ports && fd == -1; i++) {
        printf("Trying to open %s...\n", port_names[i]);
        fd = open_serial_port(port_names[i]);
    }
    
    if (fd == -1) {
        fprintf(stderr, "Failed to open any serial port\n");
        return -1;
    }
    
    // Configure the port
    if (configure_port(fd) == -1) {
        close(fd);
        return -1;
    }
    
    // Send commands with error checking
    const char* commands[] = {
        "AT\r",
        "AT+CMGF=1\r",
        "AT+CMGS=\"+1234567890\"\r",
        "Merhaba, bu bir test mesajidir.\x1A"
    };
    
    for (int i = 0; i < 4; i++) {
        ssize_t bytes_written = write(fd, commands[i], strlen(commands[i]));
        if (bytes_written == -1) {
            perror("Write failed");
            close(fd);
            return -1;
        }
        printf("Sent command: %s\n", commands[i]);
        sleep(1);  // Wait between commands
    }
    
    close(fd);
    printf("Port closed successfully\n");
    return 0;
}