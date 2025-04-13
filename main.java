import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity implements SensorEventListener {

    private SensorManager sensorManager;
    private Sensor temperatureSensor;
    private TextView temperatureView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        temperatureView = findViewById(R.id.temperatureView);
        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);

        // Telefonun sıcaklık sensörüne erişim
        if (sensorManager != null) {
            temperatureSensor = sensorManager.getDefaultSensor(Sensor.TYPE_AMBIENT_TEMPERATURE);
            if (temperatureSensor == null) {
                temperatureView.setText("Sıcaklık sensörü bulunamadı.");
            } else {
                sensorManager.registerListener(this, temperatureSensor, SensorManager.SENSOR_DELAY_NORMAL);
            }
        }
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_AMBIENT_TEMPERATURE) {
            float temperature = event.values[0];
            temperatureView.setText("Sıcaklık: " + temperature + "°C");
            
            // Sıcaklık kontrolü
            if (temperature > 40) {
                // Örneğin, bir uyarı ver
                temperatureView.setText("Cihaz çok sıcak! Soğutma gerekli.");
            }
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // Sensör hassasiyeti değişikliği için gereken işlemler
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (sensorManager != null) {
            sensorManager.unregisterListener(this);
        }
    }
}