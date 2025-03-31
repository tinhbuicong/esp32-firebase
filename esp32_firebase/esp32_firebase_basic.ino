#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>

// Thông tin WiFi
const char *ssid = "stephen";
const char *password = "25111999a@";

// Cấu hình Firebase
#define API_KEY "AIzaSyAxufg9SmzOYyGkDnWOjeYKzDJm2mKbdH0"
#define DATABASE_URL "https://monitor-ccab4-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Khai báo đối tượng Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    // Kết nối WiFi
    Serial.print("Đang kết nối WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");

    // Cấu hình Firebase
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void loop()
{
    if (Serial.available())
    {
        String receivedData = Serial.readStringUntil('\n'); // Đọc dữ liệu từ COM

        Serial.println("Dữ liệu nhận từ COM: " + receivedData);

        // Gửi dữ liệu lên Firebase
        if (Firebase.RTDB.setString(&fbdo, "/ESP32/data", receivedData))
        {
            Serial.println("Gửi thành công!");
        }
        else
        {
            Serial.println("Lỗi: " + fbdo.errorReason());
        }
    }

    delay(1000);
}
