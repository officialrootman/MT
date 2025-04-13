import RPi.GPIO as GPIO
import time

# GPIO pin numarasını belirleyin
FAN_PIN = 18

# GPIO ayarları
GPIO.setmode(GPIO.BCM)
GPIO.setup(FAN_PIN, GPIO.OUT)

try:
    while True:
        # Sıcaklık değerini bir sensörden okuyun (örneğin, bir termistör veya DHT sensöründen)
        temperature = float(input("Enter the current temperature: "))  # Örnek için manuel giriş

        # Sıcaklığa göre fanı aç/kapat
        if temperature > 30:  # 30°C üzeri ise fan çalışsın
            GPIO.output(FAN_PIN, GPIO.HIGH)
            print("Fan çalışıyor...")
        else:
            GPIO.output(FAN_PIN, GPIO.LOW)
            print("Fan kapalı...")
        
        time.sleep(1)

except KeyboardInterrupt:
    print("Program sonlandırılıyor...")
    GPIO.cleanup()