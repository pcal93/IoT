import paho.mqtt.client as mqtt
import json
from database import Database
import tabulate
import datetime
from system_status import SystemStatus as ss
from coap_handler import Put_charging_state as put

class MQTTClient:

    def on_connect(self, client, userdata, flags, rc):
        #print("connected with code: " + str(rc))
        self.client.subscribe("battery_data")

    def on_message(self,client,userdata,msg):
        #print("msg topic: " + str(msg.payload))
        data = json.loads(msg.payload)
        node_id = data["node"]
        soc = data["SOC"]
        temp = data["temp"]
        timestamp = datetime.datetime.now()

        ss.SOC = soc
        ss.temp = temp
        
        charge = 0

        with self.connection.cursor() as cursor:
            sql = "INSERT INTO `battery_data` (`client_id`, `timestamp`, `SOC`, `temp`) VALUES (%s, %s, %s, %s)"
            cursor.execute(sql, (node_id, timestamp, soc, temp))

        self.connection.commit()
        if soc < ss.minSOC:
            charge = "1"
        elif soc > ss.maxSOC:
            charge = "0"

        if ss.manual == 0:
            if ss.charging != charge:
                    ss.charging = charge
                    self.client.publish("charging",charge)
                    put.put_charging_state(charge, charge)
    
    def publish(self, resource, value):
        self.client.publish(resource, value)
            

    def mqtt_client(self):
        self.db = Database()
        self.connection = self.db.connect_db()
        #print("Mqtt client starting")
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        try:
            self.client.connect("127.0.0.1", 1883, 60)
        except Exception as e:
            print(str(e))
        self.client.loop_forever()