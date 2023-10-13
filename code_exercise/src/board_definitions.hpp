/**
* \file            board_definitions.hpp
* \brief           Header file with options for the cmake workshop pico firmware
*/
/*
*  Copyright 2023 (C) HAN University of Applied Sciences
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
* Author:          Victor Hogeweij <vo.hogeweij@student.han.nl>
*/

#ifndef BOARD_DEFINITIONS_HPP
#define BOARD_DEFINITIONS_HPP

/**
 * Fill in your group-name here!
 * Everything has to be lower case and fit in 400 characters!
 */
const char *group_name = "<invalid_group_name>";

/**
 * Add custom phrase for requesting the price!
 * Can be anything..
 * Leave at default if you're out of creativity ;)
 */
const char *winning_phrase = "GimmeDaPrice!";


/**
 * Fill in the Wifi details
 */
#define WIFI_SSID "<Wifi_SSID>"
#define WIFI_PASSWORD "<Wifi_password>"


/**
 * Leave these settings as-is. These are set correctly for rpi zero's MQTT server
 */
#define MQTT_SERVER_IP PP_HTONL(LWIP_MAKEU32(192,168,0,10))
#define MQTT_SERVER_PORT 1883


#endif
