/*
 * Copyright (c) 2020, Carlo Vallati, University of Pisa
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contiki.h"
#include "coap-engine.h"
#include "sys/etimer.h"
#include "dev/leds.h"
#include "coap-blocking-api.h"

#include "node-id.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-debug.h"
#include "routing/routing.h"

#include "dev/etc/rgb-led/rgb-led.h"

#define SERVER_EP "coap://[fd00::1]:5683"
#define CONNECTION_TRY_INTERVAL 1
#define REGISTRATION_TRY_INTERVAL 1
#define SIMULATION_INTERVAL 1

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "rele-actuator"
#define LOG_LEVEL LOG_LEVEL_APP

#define INTERVAL_BETWEEN_SIMULATIONS 3
#define INTERVAL_BETWEEN_CONNECTION_TESTS 1
/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern coap_resource_t res_rele;

char *service_url = "registry";
static bool registered = false;

static struct etimer connectivity_timer;
static struct etimer wait_registration;

PROCESS(rele_actuator, "Rele Actuator Coap Server");
AUTOSTART_PROCESSES(&rele_actuator);

static bool is_connected() {
	if(NETSTACK_ROUTING.node_is_reachable()) {
		LOG_INFO("The Border Router is reachable\n");
		return true;
  	} else {
		LOG_INFO("Waiting for connection with the Border Router\n");
	}
	return false;
}

void client_chunk_handler(coap_message_t *response)
{
	const uint8_t *chunk;

	if(response == NULL) {
		LOG_INFO("Request timed out");
		return;
	}
	registered = true;
	int len = coap_get_payload(response, &chunk);
	LOG_INFO("|%.*s \n", len, (char *)chunk);
}

PROCESS_THREAD(rele_actuator, ev, data)
{
  PROCESS_BEGIN();

  static coap_endpoint_t server_ep;
  static coap_message_t request[1]; // This way the packet can be treated as pointer as usual

  rgb_led_set(RGB_LED_RED);

  PROCESS_PAUSE();

  LOG_INFO("Starting Rele Actuator Coap CoAP-Server\n");
  coap_activate_resource(&res_rele, "rele");

  // try to connect to the border router
  etimer_set(&connectivity_timer, CLOCK_SECOND * INTERVAL_BETWEEN_CONNECTION_TESTS);
  PROCESS_WAIT_UNTIL(etimer_expired(&connectivity_timer));

  while(!is_connected()){
    etimer_reset(&connectivity_timer);
    PROCESS_WAIT_UNTIL(etimer_expired(&connectivity_timer));
  }

  while(!registered) {
    	LOG_INFO("Sending registration message\n");
    	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
    	// Prepare the message
    	coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);  
  		coap_set_header_uri_path(request, "registry");
		const char msg[] = "rele";

      	coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

    	COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);

    	PROCESS_WAIT_UNTIL(etimer_expired(&wait_registration));
    }                 

  PROCESS_END();
}