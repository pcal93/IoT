#include "contiki.h"
#include "net/routing/routing.h"
#include "mqtt.h"
#include "net/ipv6/uip.h"
#include "net/ipv6/uip-icmp6.h"
#include "net/ipv6/sicslowpan.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "lib/sensors.h"
#include "dev/button-hal.h"
#include "dev/leds.h"
#include "os/sys/log.h"
#include "mqtt-client.h"
#include <string.h>
#include <strings.h>


#include <time.h>
#include "dev/etc/rgb-led/rgb-led.h"




/*---------------------------------------------------------------------------*/
#define LOG_MODULE "mqtt-client"
#ifdef MQTT_CLIENT_CONF_LOG_LEVEL
#define LOG_LEVEL MQTT_CLIENT_CONF_LOG_LEVEL
#else
#define LOG_LEVEL LOG_LEVEL_DBG
#endif

/*---------------------------------------------------------------------------*/
/* MQTT broker address. */
#define MQTT_CLIENT_BROKER_IP_ADDR "fd00::1"

static const char *broker_ip = MQTT_CLIENT_BROKER_IP_ADDR;

// Default config values
#define DEFAULT_BROKER_PORT         1883
#define DEFAULT_PUBLISH_INTERVAL    (5 * CLOCK_SECOND) //Set to 1 second


// We assume that the broker does not require authentication


/*---------------------------------------------------------------------------*/
/* Various states */
static uint8_t state;

#define STATE_INIT    		  0
#define STATE_NET_OK    	  1
#define STATE_CONNECTING      2
#define STATE_CONNECTED       3
#define STATE_SUBSCRIBED      4
#define STATE_DISCONNECTED    5

/*---------------------------------------------------------------------------*/
PROCESS_NAME(mqtt_client_process);
AUTOSTART_PROCESSES(&mqtt_client_process);

/*---------------------------------------------------------------------------*/
/* Maximum TCP segment size for outgoing segments of our socket */
#define MAX_TCP_SEGMENT_SIZE    32
#define CONFIG_IP_ADDR_STR_LEN   64
/*---------------------------------------------------------------------------*/
/*
 * Buffers for Client ID and Topics.
 * Make sure they are large enough to hold the entire respective string
 */
#define BUFFER_SIZE 64
//DEFINING PROJECT VARIABLES

// Global variables to store sensor values
static int soc = 0;
static int temp = 0;

static bool charging = false;



static char pub_topic[BUFFER_SIZE];

static char sub_topic[BUFFER_SIZE];

//
static char client_id[BUFFER_SIZE];

// Periodic timer to check the state of the MQTT client
//#define STATE_MACHINE_PERIODIC     (CLOCK_SECOND >> 1)
static struct etimer periodic_timer;
static struct etimer reset_timer;

/*---------------------------------------------------------------------------*/
/*
 * The main MQTT buffers.
 * We will need to increase if we start publishing more data.
 */
#define APP_BUFFER_SIZE 512

static char app_buffer[APP_BUFFER_SIZE];

/*---------------------------------------------------------------------------*/

static struct mqtt_message *msg_ptr = 0;

static struct mqtt_connection conn;

mqtt_status_t status;

char broker_address[CONFIG_IP_ADDR_STR_LEN];


/*---------------------------------------------------------------------------*/
PROCESS(mqtt_client_process, "MQTT Client");

/*---------------------------------------------------------------------------*/

static void pub_handler(const char *topic, uint16_t topic_len, const uint8_t *chunk, uint16_t chunk_len)
{
  printf("Pub Handler: topic='%s' (len=%u), chunk_len=%u\n", topic,
          topic_len, chunk_len);
  if(strcmp((const char *)topic, "charging")==0){
    printf("Received Charging state\n");
    if(strcmp((const char *)chunk, "1")==0){
        charging = true;
    }else{
        charging = false;
    }
    return;
  }
}
/*---------------------------------------------------------------------------*/

static void mqtt_event(struct mqtt_connection *m, mqtt_event_t event, void *data)
{
  switch(event) {
  case MQTT_EVENT_CONNECTED: {
    printf("Application has a MQTT connection\n");
    leds_on(1);
    state = STATE_CONNECTED;
    break;
  }
  case MQTT_EVENT_DISCONNECTED: {
    printf("MQTT Disconnect. Reason %u\n", *((mqtt_event_t *)data));
    leds_off(15);
    state = STATE_DISCONNECTED;
    process_poll(&mqtt_client_process);
    break;
  }
  case MQTT_EVENT_PUBLISH: {
    msg_ptr = data;

    pub_handler(msg_ptr->topic, strlen(msg_ptr->topic),
                msg_ptr->payload_chunk, msg_ptr->payload_length);
    break;
  }
  case MQTT_EVENT_SUBACK: {
#if MQTT_311
    mqtt_suback_event_t *suback_event = (mqtt_suback_event_t *)data;

    if(suback_event->success) {
      printf("Application is subscribed to topic successfully\n");
    } else {
      printf("Application failed to subscribe to topic (ret code %x)\n", suback_event->return_code);
    }
#else
    printf("Application is subscribed to topic successfully\n");
#endif
    break;
  }
  case MQTT_EVENT_UNSUBACK: {
    printf("Application is unsubscribed to topic successfully\n");
    break;
  }
  case MQTT_EVENT_PUBACK: {
    printf("Publishing complete.\n");
    break;
  }
  default:
    printf("Application got a unhandled MQTT event: %i\n", event);
    break;
  }
}

static bool
have_connectivity(void)
{
  if(uip_ds6_get_global(ADDR_PREFERRED) == NULL ||
     uip_ds6_defrt_choose() == NULL) {
    return false;
  }
  return true;
}
/*--------------------SUPPORT METHODS--------------------------------*/

double random_double(double min, double max) {
    return min + ((double)rand() / ((double)RAND_MAX + 1)) * (max - min);
}


/*--------------------BEGIN--------------------------------------*/

PROCESS_THREAD(mqtt_client_process, ev, data)
{

  PROCESS_BEGIN();
  
  printf("MQTT Client Process\n");

  // Initialize the ClientID as MAC address
  snprintf(client_id, BUFFER_SIZE, "%02x%02x%02x%02x%02x%02x",
                     linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1],
                     linkaddr_node_addr.u8[2], linkaddr_node_addr.u8[5],
                     linkaddr_node_addr.u8[6], linkaddr_node_addr.u8[7]);

  // Broker registration					 
  mqtt_register(&conn, &mqtt_client_process, client_id, mqtt_event,
                  MAX_TCP_SEGMENT_SIZE);
				  
  state=STATE_INIT;
				    
 // Initialize periodic timer to check the status 

  etimer_set(&periodic_timer, DEFAULT_PUBLISH_INTERVAL);
  etimer_set(&reset_timer, CLOCK_SECOND);
  rgb_led_set(RGB_LED_GREEN);
  soc = (int)random_double(0.0, 100);
  /* Main loop */
  while(1) {

    PROCESS_YIELD();
    			
    if(soc<21){
    	rgb_led_set(RGB_LED_RED);
	} else if (soc>=21 && soc<=79){
		rgb_led_set(RGB_LED_YELLOW);
	} else {
		rgb_led_set(RGB_LED_GREEN);
	}
		
		if((ev == PROCESS_EVENT_TIMER && data == &periodic_timer) || ev == PROCESS_EVENT_POLL){
			if(state==STATE_INIT && have_connectivity()==true) {
				 state = STATE_NET_OK;
		    } 
		  
		  if(state == STATE_NET_OK){
			  // Connect to MQTT server
			  printf("Connecting!\n");
			  
			  memcpy(broker_address, broker_ip, strlen(broker_ip));
			  
			  mqtt_connect(&conn, broker_address, DEFAULT_BROKER_PORT, (DEFAULT_PUBLISH_INTERVAL * 1) / CLOCK_SECOND, MQTT_CLEAN_SESSION_ON);
			  state = STATE_CONNECTING;
		    }
		  
		  if(state==STATE_CONNECTED){
		  
			  // Subscribe to a  topic
			  strcpy(sub_topic,"charging");

			  status = mqtt_subscribe(&conn, NULL, sub_topic, MQTT_QOS_LEVEL_0);

			  printf("Subscribing!\n");
			  if(status == MQTT_STATUS_OUT_QUEUE_FULL ) {
				LOG_ERR("Tried to subscribe but command queue was full!\n");
				
			  }
			  
			  state = STATE_SUBSCRIBED;
		    }

			  
		if(state == STATE_SUBSCRIBED){
		// Publish Samples
		    if(charging == true){
        soc = (int)random_double(soc, 100);
    } else {
        soc = (int)random_double(0.0, soc);
    }
        
        
		temp = (int)random_double(15.0, 50);
 		

		sprintf(pub_topic, "%s", "battery_data");
		//Movement app buffer
		sprintf(app_buffer,"{\"node\": \"%s\", \"SOC\": %d, \"temp\": %d}", client_id, (int)soc, (int)temp);
		printf("Publishing: %s\nTopic: %s\n", app_buffer,pub_topic);
		

		//MQTT publishing	
		mqtt_publish(&conn, NULL, pub_topic, (uint8_t *)app_buffer, strlen(app_buffer), MQTT_QOS_LEVEL_0, MQTT_RETAIN_OFF);
		} else if ( state == STATE_DISCONNECTED ){
		   LOG_ERR("Disconnected form MQTT broker\n");	
		   // Recover from error
         	state = STATE_INIT;

		}
		etimer_set(&periodic_timer, DEFAULT_PUBLISH_INTERVAL);

		}
        if(ev == PROCESS_EVENT_TIMER && data == &reset_timer) {      
            etimer_set(&reset_timer, CLOCK_SECOND);
        }
	}

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/