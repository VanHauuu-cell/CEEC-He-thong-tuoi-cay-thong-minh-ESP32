#include "esp_http_client.h"
#include "cJSON.h"
#include "weather_api.h"
#include "fsm.h"
#include "esp_log.h"
#include "esp_netif.h"

static const char *TAG = "WEATHER";

void weather_task(void *pv) {
    ESP_LOGI("WEATHER", "Task started, waiting for stable connection");
    char buffer[1024];
    while (1) {
        esp_netif_ip_info_t ip_info;
        esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        
        if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK && ip_info.ip.addr != 0) {
            // CÓ IP RỒI MỚI CHẠY TIẾP
            ESP_LOGI("WEATHER", "Internet Ready! Fetching data...");
        
        esp_http_client_config_t config = { 
                .url = API_URL, 
                .timeout_ms = 10000};
        esp_http_client_handle_t client = esp_http_client_init(&config);
        
        esp_err_t err = esp_http_client_open(client, 0);
        if (err == ESP_OK) {
            esp_http_client_fetch_headers(client);
            int read_len = esp_http_client_read(client, buffer, sizeof(buffer));
            buffer[read_len] = '\0';

            cJSON *json = cJSON_Parse(buffer);
            if (json) {
                cJSON *weather_array = cJSON_GetObjectItem(json, "weather");
                cJSON *weather_obj = cJSON_GetArrayItem(weather_array, 0);
                const char *main_weather = cJSON_GetObjectItem(weather_obj, "main")->valuestring;

                ESP_LOGI(TAG, "Current Weather: %s", main_weather);

                system_event ev = { .event_type = E_WEATHER_UPDATE };
                // Kiểm tra nếu là Rain hoặc Drizzle (mưa phùn)
                ev.weather = (strcmp(main_weather, "Rain") == 0 || strcmp(main_weather, "Drizzle") == 0);
                xQueueSend(system_queue, &ev, pdMS_TO_TICKS(100));

                cJSON_Delete(json);
            }
        }
        esp_http_client_cleanup(client);
        vTaskDelay(pdMS_TO_TICKS(600000)); // Cập nhật mỗi 10 phút để tránh bị API khóa
        }else {
            // Chưa có IP thì đợi và thử lại
            ESP_LOGW("WEATHER", "WiFi not ready, retrying in 2s...");
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}