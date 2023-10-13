#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <mosquitto.h>
#include <unistd.h>

namespace mqtt_config {
    /*
     * The ip-address and port of the MQTT server
     */
    constexpr const char* HOST = "192.168.0.10";
    constexpr int PORT = 1883;
    /*
     * Timeout time
     */
    constexpr int KEEP_ALIVE = 60;
    /*
     * The topic to monitor for incoming messages
     */
    constexpr const char* TOPIC_SUBSCRIBE = "WinnerPub/#";
}

namespace messages {
    /*
     * SIGTERM for the signal handler
     */
    constexpr const char* SIGTERM_RECEIVED = "Received SIGTERM signal. Exiting...\n";
    /*
     * Error messages
     */
    constexpr const char* INIT_FAIL = "Can't initialize Mosquitto library\n";
    constexpr const char* CONNECT_FAIL = "Could not connect to Broker - Return code %d\n";
    /*
     * Up and running message
     */
    constexpr const char* RUNNING = "Running in an infinite loop. Send SIGTERM to terminate.\n";
}

std::vector<std::string> winners;
struct mosquitto *mosq = nullptr;

/**
 * @brief show the leaderboard in the terminal
 */
void display_current_winners() {
    std::cout << "Winners: " << '\n';
    int winner_position = 1;
    for (const auto& winner : winners) {
        std::cout << winner_position << " :" << winner << '\n';
        winner_position++;
    }
}

/**
 * @brief Sigterm handler for safely closing and de-initializing the application
 * @param signum The sigterm received
 */
void sigterm_handler(int signum) {
    std::cout << messages::SIGTERM_RECEIVED;
    mosquitto_loop_stop(mosq, false);
    mosquitto_disconnect(mosq);
    display_current_winners();
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    exit(0);
}

/**
 * @brief Callback function for MQTT incoming message on subscribed topics
 * @param mosq Handler to mosquitto instance
 * @param userdata Not used in this context
 * @param message The message received
 */
void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    char winner_pos_str[32];
    if (message->payloadlen) {
        /*
         * Show that the message was successfully received
         */
        printf("Received message on topic: %s\n", message->topic);
        printf("Message content: %s\n", (char *) message->payload);

        std::string winner_name = message->topic;
        // Remove the topic-name to retrieve the group-name
        auto slash_pos = winner_name.find("/");
        if (slash_pos != std::string::npos) {
            winner_name.erase(0, slash_pos + 1);
        }

        // Detect if the winner hasn't presented himself/herself already
        auto winner_it = std::find(winners.begin(), winners.end(), winner_name);
        uint32_t winner_pos;
        if (winner_it == winners.end()) {
            // new winner
            winners.push_back(winner_name);
            winner_pos = winners.size();
        } else {
            // old winner
            winner_pos = std::distance(winners.begin(), winner_it) + 1;
        }

        // Create the publish topic from the winner_name
        std::string publish_topic_name = "WinnerSub/" + winner_name;
        uint32_t payload_size = snprintf(winner_pos_str, sizeof(winner_pos_str), "%d", winner_pos);
        // Publish
        mosquitto_publish(mosq, nullptr, publish_topic_name.c_str(), payload_size, &winner_pos_str, 0, true);
    } else {
        // Invalid message received
        printf("Received empty message on topic: %s\n", message->topic);
    }
}

int main() {
    mosquitto_lib_init();
    mosq = mosquitto_new(nullptr, true, nullptr);

    if (!mosq) {
        fprintf(stderr, messages::INIT_FAIL);
        return 1;
    }

    mosquitto_message_callback_set(mosq, on_message);
    int rc = mosquitto_connect(mosq, mqtt_config::HOST, mqtt_config::PORT, mqtt_config::KEEP_ALIVE);

    if (rc) {
        fprintf(stderr, messages::CONNECT_FAIL, rc);
        return 1;
    }

    signal(SIGTERM, sigterm_handler);
    printf(messages::RUNNING);
    mosquitto_subscribe(mosq, nullptr, mqtt_config::TOPIC_SUBSCRIBE, 0);
    mosquitto_loop_start(mosq);

    while (true) {
        /**
         * Command line interface,
         * r : reset
         * e: exit
         * c : show current leaderboard
         */
        char input = getchar();
        switch (input) {
            case 'r': winners.clear(); break;
            case 'e': sigterm_handler(SIGTERM); break;
            case 'c': display_current_winners(); break;
            default: break;
        }
    }
    return 0;
}
