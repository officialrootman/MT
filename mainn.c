#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int main() {
    int fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY); // Seri portu aç
    if (fd == -1) {
        perror("Seri port açılamadı");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600); // Baud hızı
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &options);

    write(fd, "AT\r", 3); // Modemi test et
    sleep(1);
    write(fd, "AT+CMGF=1\r", 10); // SMS modu (Metin)
    sleep(1);
    write(fd, "AT+CMGS=\"+5514959493\"\r", 22); // Alıcı numarası
    sleep(1);
    write(fd, "Merhaba, bu bir test mesajıdır.\x1A", 33); // Mesaj ve Ctrl+Z

    close(fd);
    return 0;
}