/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"
#include "gpio.h"
#include "lwip/lwip/sockets.h"
#include "uart.h"

#define SERVER_IP "0.0.0.0"
#define SERVER_PORT 4242
#define AP_SSID "ssid"
#define AP_PASSWORD "password"
#define pbuf "hey"


void tcp_send()
{
    int sta_socket;
    struct sockaddr_in remote_ip;
    while (1) {
        sta_socket = socket(PF_INET, SOCK_STREAM, 0);
        if (-1 == sta_socket) {
            close(sta_socket);
            vTaskDelay(1000 / portTICK_RATE_MS);
            printf("ESP8266 TCP client task > socket fail!\n");
            continue;
        }
        printf("ESP8266 TCP client task > socket ok!\n");

        bzero(&remote_ip, sizeof(struct sockaddr_in));
        remote_ip.sin_family = AF_INET;
        remote_ip.sin_addr.s_addr = inet_addr(SERVER_IP);
        remote_ip.sin_port = htons(SERVER_PORT);
        if (0 != connect(sta_socket, (struct sockaddr *)(&remote_ip), sizeof(struct
        sockaddr))) {
            close(sta_socket);
            vTaskDelay(1000 / portTICK_RATE_MS);
            printf("ESP8266 TCP client task > connect fail!\n");
            continue;
        }
        printf("ESP8266 TCP client task > connect ok!\n");

        if (write(sta_socket, pbuf, strlen(pbuf) + 1) < 0){
            close(sta_socket);
            vTaskDelay(1000 / portTICK_RATE_MS);
            printf("ESP8266 TCP client task > send fail\n");
            continue;
        }
        printf("ESP8266 TCP client task > send success\n");
        break;
    }

    GPIO_OUTPUT_SET(2, 0);
}


void wifi_handle_event_cb(System_Event_t *evt)
{
    printf("event %x\n", evt->event_id);
    switch (evt->event_id) {
        case EVENT_STAMODE_CONNECTED:
            printf("connect to ssid %s, channel %d\n",
                    evt->event_info.connected.ssid,
                    evt->event_info.connected.channel);
            break;
        case EVENT_STAMODE_DISCONNECTED:
            printf("disconnect from ssid %s, reason %d\n",
                    evt->event_info.disconnected.ssid,
                    evt->event_info.disconnected.reason);
            break;
        case EVENT_STAMODE_AUTHMODE_CHANGE:
            printf("mode: %d -> %d\n",
                    evt->event_info.auth_change.old_mode,
                    evt->event_info.auth_change.new_mode);
            break;
        case EVENT_STAMODE_GOT_IP:
            printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
                    IP2STR(&evt->event_info.got_ip.ip),
                    IP2STR(&evt->event_info.got_ip.mask),
                    IP2STR(&evt->event_info.got_ip.gw));
            printf("\n");
            GPIO_OUTPUT_SET(2, 1);
            tcp_send();

            break;
        default:
            break;
    }
}


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    uart_div_modify(UART0, UART_CLK_FREQ / (BIT_RATE_38400));
    //gdbstub_init();
    GPIO_OUTPUT_SET(2, 0);
    printf("\nSDK version:%s\n", system_get_sdk_version());
    wifi_set_event_handler_cb(wifi_handle_event_cb);
    wifi_set_opmode(STATION_MODE);

    struct station_config* config = (struct station_config *)zalloc(sizeof(struct
        station_config));
        sprintf(config->ssid, AP_SSID);
        sprintf(config->password, AP_PASSWORD);
        wifi_station_set_config(config);
        free(config);
    wifi_station_connect();
}

