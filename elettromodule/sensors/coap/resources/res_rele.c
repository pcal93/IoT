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

#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "dev/leds.h"

#include "dev/etc/rgb-led/rgb-led.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "rele-actuator"
#define LOG_LEVEL LOG_LEVEL_APP

static void rele_sys_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_rele,
         "title=\"Status \" PUT mode=on|off;rt=\"status\"",
         NULL,
         NULL,
         rele_sys_put_handler,
         NULL);

static void rele_sys_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	size_t len = 0;
	  const char *mode = NULL;
	  int success = 0;
	  if ((len = coap_get_post_variable(request, "mode", &mode))){
	    LOG_DBG("mode_charging %s\n", mode);

	    if (strncmp(mode, "off", len) == 0){
	      LOG_INFO("off");
	      coap_set_status_code(response,VALID_2_03);
	      rgb_led_set(RGB_LED_RED);
	      success = 1;
	    }

	    else if (strncmp(mode, "on", len) == 0){
	      LOG_INFO("on");
	      coap_set_status_code(response,VALID_2_03);
	      rgb_led_set(RGB_LED_GREEN);
	      success = 1;
	    }
		  else if (strncmp(mode, "man", len) == 0){
	      LOG_INFO("manual");
	      coap_set_status_code(response,VALID_2_03);
	      rgb_led_set(RGB_LED_BLUE);
	      success = 1;
	    }
	  }
	  
	  if (!success){
	    coap_set_status_code(response, BAD_REQUEST_4_00);
	  }
}