# Winner evaluator

The winner evaluator is an linux/mac application which can be used together with the raspberry pi pico w code. 

## Functionality

The program uses the mosquitto library and connects to a remotely hosted (or locally hosted) MQTT server. Then it subscribes to the topic WinnerPub/  and waits for incoming  messages. When a message gets published to the topic, the application will get the leaderboard position with the topic-name after the WinnerPub prefix. The leaderboard position will then get published  in the corresponding child topic in WinnerSub/ . 



Besides MQTT functionality, the application also waits for commands coming in using the console. 

Commands:

| Command | Short description   | Functionality                                                             |
| ------- | ------------------- | ------------------------------------------------------------------------- |
| c       | current leaderboard | Prints the current leaderboard to the console.                            |
| e       | exit                | Deinitializes dynamically allocated resources and closes the application. |
| r       | reset               | Resets the leaderboard.                                                   |

It works like this:

```mermaid
flowchart TD
    program_start[START] --> init_mosq(Intialize mosquitto library)
    init_mosq --> subscribe_topic(Subscribe to topic WinnerPub)
    subscribe_topic --> poll_user_input(Wait for user input or incoming messages)
    poll_user_input --> |User type option c| show_leaderboard(Show current leaderboard)
    show_leaderboard --> poll_user_input
    poll_user_input --> |User type option r| reset_leaderboard(Reset leaderboard)
    reset_leaderboard --> poll_user_input
    poll_user_input --> |User types option e or sigterm signal| sigterm_received(Sigterm received, deinitialize mosquitto library callback) 
    poll_user_input --> |No user input received| poll_user_input
    sigterm_received --> exit(quit program)
```

The program has a callback function for messages received on the subscribed topic:



```mermaid
flowchart TD
    cb_start[Callback start] --> print_incoming_message(Print incoming message and topic to console)
    print_incoming_message --> subtract_group_name(Remove the WinnerPub/ prefix to get the groupname)
    subtract_group_name --> group_name_has_already_published(Check if this is the first time that this group published to the Winnerpub topic)
    group_name_has_already_published --> |This is the first time| update_the_leaderboard(Add groupname to first available position in leaderboard)
    group_name_has_already_published --> |Not the first time| return_leaderboard_position(Get position in leaderboard)
    update_the_leaderboard --> return_leaderboard_position
    return_leaderboard_position --> publish_leaderboard_position(Publish leaderboard position to the WinnerSub/group_name topic)
    publish_leaderboard_position --> cb_exit[Callback exit]
```

This program has to be compiled with C++17 as it uses pretty modern standard library functions and classes like: std::find and std::iterator.  CMakeLists.txt already has this preconfigured for you.



## Compiling

Compiling requires you to run two commands:

```bash
cmake . -B build
cd build && make
```

Of course you could also use an IDE like CLion or VS code which will automatically recognize the CMakeLists.txt and do the build process for you.



## Running

Running can be simply done by:

```bash
./award_app
```

Again using an IDE like VS code or CLion is even easier as it has a green launch button (Yes I checked it.. Its green :) ).
