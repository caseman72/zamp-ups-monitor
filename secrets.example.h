// Zamp UPS monitor secrets
// Copy to secrets.h and fill in values
// This file is gitignored - do not commit to public repos

// Primary WiFi (default at boot)
#define WIFI_SSID_PRIMARY "your_primary_ssid"
#define WIFI_PASSWORD_PRIMARY "your_primary_password"

// Secondary WiFi (e.g. Starlink fallback)
#define WIFI_SSID_SECONDARY "your_secondary_ssid"
#define WIFI_PASSWORD_SECONDARY "your_secondary_password"

#define MQTT_BROKER "your_broker"
#define MQTT_USERNAME "your_username"
#define MQTT_PASSWORD "your_password"

// OTA password
#define OTA_PASSWORD "your_ota_password"

// ESPHome native API encryption key (base64, 32 bytes)
// Generate with: python3 -c "import secrets, base64; print(base64.b64encode(secrets.token_bytes(32)).decode())"
#define API_ENCRYPTION_KEY "your_base64_32byte_key"
