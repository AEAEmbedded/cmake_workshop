#include "hardware/structs/rosc.h"
#include <stdio.h>

#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/apps/mqtt.h"

#include "lwip/apps/mqtt_priv.h"

#include "tusb.h"

#include "board_definitions.hpp"

#include "mqtt_lib.hpp"


/**
 * Variables used for cb methods
 */
uint8_t data_len = 0;
uint32_t data_in = 0;
uint8_t buffer[128];


/**
 * This variable determines the final position
 */
int position = 0;

/**
 * These are used to store the publish and receive topic name in, was too lazy to do this at compile-time
 */
char publish_topic[512];
char receive_topic[512];

static void mqtt_pub_start_cb(void *arg, const char *topic, uint32_t tot_len) {
    printf("mqtt_pub_start_cb: topic %s\n", topic);

    if (tot_len > sizeof(buffer)) {
        printf("Message length exceeds buffer size, discarding");
    } else {
        data_in = tot_len;
        data_len = 0;
    }
}

static void mqtt_pub_data_cb(void *arg, const uint8_t *data, uint16_t len, uint8_t flags) {
    if (data_in > 0) {
        data_in -= len;
        memcpy(&buffer[data_len], data, len);
        data_len += len;
        if (data_in == 0) {
            buffer[data_len] = 0;
            printf("Message received: %s\n", &buffer);
            sscanf((char *) buffer, "%d", &position);
            printf("Got position: %d\n", position);
        }
    }
}

const char *get_subscribe_topic(const char *topic_name) {
    snprintf(receive_topic, sizeof(receive_topic), "WinnerSub/%s", group_name);
    return receive_topic;
}

const char *get_publish_topic(const char *topic_name) {
    snprintf(publish_topic, sizeof(publish_topic), "WinnerPub/%s", group_name);
    return publish_topic;
}

void subscribe_for_position(mqtt_client_t *client, const char *topic_name) {
    err_t status;
    status = pico_mqtt_subscribe(client, topic_name);
    if (status != ERR_OK) {
        printf("mqtt subscribe failed %d\n", status);
    } else {
        printf("Mqtt subscribe succes!\n");
    }
}


void publish_for_price(mqtt_client_t *client, const char *topic, const char *message) {
    err_t status;
    status = pico_mqtt_publish(client, publish_topic, message, strlen(message));
    if (status != ERR_OK) {
        printf("Publish error: %d \n", status);
    }
}

int main() {
    stdio_init_all();
    /**
     * Block program until uart console is opened on usb
     */
    printf("waiting for usb host");
    while (!tud_cdc_connected()) {
        printf(".");
        sleep_ms(500);
    }
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();
    printf("Connecting to WiFi...\n");
    do {
        printf(".");
    } while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 3000));

    mqtt_client_t *client = mqtt_client_new();
    if (client == NULL) {
        printf("Failed to create new mqtt client\n");
    }
    static ip_addr_t server_addr = {
            .addr = MQTT_SERVER_IP
    };
    err_t status = pico_mqtt_connect(client, &server_addr, MQTT_SERVER_PORT, group_name);
    sleep_ms(10);
    if (status != ERR_OK) {
        printf("mqtt_connect return %d\n", status);
    }

    const char *publish_topic_name = get_publish_topic(group_name);
    const char *subscribe_topic_name = get_subscribe_topic(group_name);

    mqtt_set_inpub_callback(client, mqtt_pub_start_cb, mqtt_pub_data_cb, 0);

    bool subscribed = false;
    absolute_time_t timeout = nil_time;
    while (true) {
        cyw43_arch_poll();
        absolute_time_t now = get_absolute_time();
        if(is_nil_time(timeout) || absolute_time_diff_us(now, timeout) <= 0) {
        if (mqtt_client_is_connected(client) && position == 0) {
            cyw43_arch_lwip_begin();
            if (!subscribed) {
                subscribe_for_position(client, subscribe_topic_name);
                subscribed = true;
            }
            publish_for_price(client, publish_topic_name, winning_phrase);
            cyw43_arch_lwip_end();
            timeout = make_timeout_time_ms(3000);
        } else if (position != 0) {
            for (uint8_t blink_times = 0; blink_times < position; blink_times++) {
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
                sleep_ms(250);
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
                sleep_ms(250);
            }
            timeout = make_timeout_time_ms(3000);
        }
        }
    }
    cyw43_arch_deinit();

    return 0;
}
