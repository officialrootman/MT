#include <DHT.h>

// DHT sensör pin ve tipi
#define DHTPIN 2     // DHT sensörünün bağlı olduğu pin
#define DHTTYPE DHT11   // DHT11 ya da DHT22 sensörünü kullanabilirsiniz

// Fan kontrolü için röle pin
#define FAN_PIN 3

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();

  // Fan kontrol pini
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW); // Fan başlangıçta kapalı
}

void loop() {
  // Sıcaklık ve nem değerlerini oku
  float temperature = dht.readTemperature();

  // Sensör hatası varsa veri okumayı atla
  if (isnan(temperature)) {
    Serial.println("Sıcaklık okuma hatası!");
    return;
  }

  // Sıcaklık değerini yazdır
  Serial.print("Sıcaklık: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Sıcaklığa göre fanı kontrol et
  if (temperature > 30.0) { // Sıcaklık 30°C'den büyükse fan çalışsın
    digitalWrite(FAN_PIN, HIGH);
    Serial.println("Fan çalışıyor...");
  } else {
    digitalWrite(FAN_PIN, LOW);
    Serial.println("Fan kapalı...");
  }

  // 2 saniye bekle
  delay(2000);
}