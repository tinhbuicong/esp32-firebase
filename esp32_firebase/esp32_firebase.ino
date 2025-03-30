#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <EEPROM.h>
#include "time.h"

// Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Configuration structure
struct Config
{
    char wifi_ssid[32];
    char wifi_password[64];
    char firebase_host[128];
    char firebase_auth[256];
} configuration;

// EEPROM size
#define EEPROM_SIZE 512

// Configuration flag address
#define CONFIG_FLAG_ADDR 0
#define CONFIG_START_ADDR 1

// NTP Server settings
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

bool isConfigured = false;
String inputBuffer = "";

void saveConfiguration()
{
    EEPROM.write(CONFIG_FLAG_ADDR, 0xAA); // Configuration flag
    EEPROM.put(CONFIG_START_ADDR, configuration);
    EEPROM.commit();
}

void loadConfiguration()
{
    if (EEPROM.read(CONFIG_FLAG_ADDR) == 0xAA)
    {
        EEPROM.get(CONFIG_START_ADDR, configuration);
        isConfigured = true;
    }
}

void processCommand(String cmd)
{
    if (cmd.startsWith("#set_variable_"))
    {
        cmd = cmd.substring(13); // Remove "#set_variable_"
        int separatorPos = cmd.indexOf("_");
        if (separatorPos != -1)
        {
            String variable = cmd.substring(0, separatorPos);
            String value = cmd.substring(separatorPos + 1);

            // Remove quotes if present
            if (value.startsWith("\"") && value.endsWith("\""))
            {
                value = value.substring(1, value.length() - 1);
            }

            if (variable == "WIFI_SSID")
            {
                strncpy(configuration.wifi_ssid, value.c_str(), sizeof(configuration.wifi_ssid) - 1);
            }
            else if (variable == "WIFI_PASSWORD")
            {
                strncpy(configuration.wifi_password, value.c_str(), sizeof(configuration.wifi_password) - 1);
            }
            else if (variable == "FIREBASE_HOST")
            {
                strncpy(configuration.firebase_host, value.c_str(), sizeof(configuration.firebase_host) - 1);
            }
            else if (variable == "FIREBASE_AUTH")
            {
                strncpy(configuration.firebase_auth, value.c_str(), sizeof(configuration.firebase_auth) - 1);
            }

            saveConfiguration();
            Serial.println("Configuration updated and saved");
            ESP.restart();
        }
    }
}

void connectWiFi()
{
    if (strlen(configuration.wifi_ssid) == 0 || strlen(configuration.wifi_password) == 0)
    {
        Serial.println("WiFi not configured");
        return;
    }

    WiFi.begin(configuration.wifi_ssid, configuration.wifi_password);
    Serial.print("Connecting to WiFi");

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\nWiFi connection failed");
    }
}

void initFirebase()
{
    if (strlen(configuration.firebase_host) == 0 || strlen(configuration.firebase_auth) == 0)
    {
        Serial.println("Firebase not configured");
        return;
    }

    config.api_key = configuration.firebase_auth;
    config.database_url = configuration.firebase_host;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void sendToFirebase(String message)
{
    if (!Firebase.ready())
    {
        Serial.println("Firebase not ready");
        return;
    }

    // Get timestamp
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }

    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);

    // Create JSON object
    FirebaseJson json;
    json.set("timestamp", timestamp);
    json.set("message", message);

    String path = "/logs/" + String(millis());
    if (Firebase.RTDB.setJSON(&fbdo, path, &json))
    {
        Serial.println("Log sent to Firebase");
    }
    else
    {
        Serial.println("Failed to send log");
        Serial.println(fbdo.errorReason());
    }
}

void setup()
{
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);

    // Load configuration
    loadConfiguration();

    if (isConfigured)
    {
        connectWiFi();
        if (WiFi.status() == WL_CONNECTED)
        {
            // Init and get the time
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
            initFirebase();
        }
    }

    Serial.println("Ready for configuration commands");
}

void loop()
{
    if (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n')
        {
            if (inputBuffer.length() > 0)
            {
                if (inputBuffer.startsWith("#set_variable_"))
                {
                    processCommand(inputBuffer);
                }
                else if (WiFi.status() == WL_CONNECTED && Firebase.ready())
                {
                    sendToFirebase(inputBuffer);
                }
                inputBuffer = "";
            }
        }
        else
        {
            inputBuffer += c;
        }
    }

    // Maintain Firebase connection
    if (WiFi.status() == WL_CONNECTED)
    {
        Firebase.loop();
    }
}