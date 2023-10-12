#include <stdio.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#define MQTT_HOST "192.168.0.10"
#define MQTT_PORT 1883
#define SIGTERM_MSG "SIGTERM received.\n"
#define SLASH_STR "/"


std::vector<std::string> winners;

int message_id;
struct mosquitto *mosq = NULL;


void get_current_winners() {
    std::cout << "Winners: " << '\n';
    int winner_position = 1;
    for (auto winner: winners) {
        std::cout << winner_position << " :" << winner << '\n';
        winner_position++;
    }
}

void sigterm_handler(int signum) {
    printf("Received SIGTERM signal. Exiting...\n");
    mosquitto_loop_stop(mosq, false);
    // Disconnect from broker
    mosquitto_disconnect(mosq);

    get_current_winners();

    // Cleanup
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    exit(0);
}

void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    char winner_pos_str[32];
    if (message->payloadlen) {
        printf("Received message on topic: %s\n", message->topic);
        printf("Message content: %s\n", (char *) message->payload);
        std::string winner_name = message->topic;
        if (winner_name.size() > 0) {
            int slash_pos = winner_name.find(SLASH_STR);
            if (slash_pos) {
                winner_name.erase(0, slash_pos + 1); // +1 We want to erase the slash from the string too
            }
            winners.push_back(winner_name);
            std::string publish_topic_name = "WinnerSub/";
            publish_topic_name.append(winner_name);
            uint32_t payload_size = snprintf(winner_pos_str, sizeof(winner_pos_str), "%d", winners.size());
            mosquitto_publish(mosq, NULL, publish_topic_name.c_str(), payload_size, &winner_pos_str, 0, true);
        }
    } else {
        printf("Received empty message on topic: %s\n", message->topic);
    }
}

int main() {
    int rc;


    // Initialize the Mosquitto library
    mosquitto_lib_init();

    // Create a new Mosquitto runtime instance with a random client ID
    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Can't initialize Mosquitto library\n");
        return 1;
    }

    mosquitto_message_callback_set(mosq, on_message);

    // Connect to the broker
    rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
    if (rc) {
        fprintf(stderr, "Could not connect to Broker - Return code %d\n", rc);
        return 1;
    }
    signal(SIGTERM, sigterm_handler);

    printf("Running in an infinite loop. Send SIGTERM to terminate.\n");

    mosquitto_subscribe(mosq, &message_id, "WinnerPub/#", 0);


    // Loop and wait for messages
    mosquitto_loop_start(mosq);

    while (1) {
        char input = getchar();
        switch (input) {
            case 'r': {
                winners.clear();
                break;
            }
            case 'e': {
                sigterm_handler(SIGTERM);
                return 0;
                break;
            }
            case 'c': {
                get_current_winners();
            }
            default: {
                break;
            }
        }
    }
    return 0;
}
