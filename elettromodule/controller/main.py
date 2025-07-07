import threading
import os
import time
from mqtt_subscriber import MQTTClient
import logging
from coap_handler import *
from system_status import SystemStatus as ss
from database import Database as db

# Unspecified IPv6 address
ip = "::"
port = 5683

class CoAPServer(CoAP):
    def __init__(self, host, port):
        CoAP.__init__(self, (host, port), False)
        self.add_resource("registry", ResExample())


def menu(command, client):
    if command == "1":
        ss.get_system_status()
    elif command == "2":
        ss.get_parameters()
    elif command == "3":
        db.show_battery_data()
    elif command == "4":
        db.show_rele_data()
    elif command == "5":
        ss.manual_override(client)
    elif command == "6":
        ss.change_parameters()
    elif command == "exit":
        print("SHUTDOWN")
        os._exit(0)
    else:
        print("Command not found")
        helper()

def helper():
    print("AVAILABLE COMMANDS--->\n")
    print(
        "1 -- Get System Status \n"\
        "2 -- Get System Paramenters\n"\
        "3 -- Show Battery Data\n"\
        "4 -- Show Rele Data\n"\
        "5 -- Manual Mode\n"\
        "6 -- Cahnge Parameters\n"\
        "exit -- Exit\n\n")





def main_loop():
     
    client = MQTTClient()
    thread = threading.Thread(target=client.mqtt_client, args=(), kwargs={})
    thread.start()

    server_coap = CoAPServer(ip, port)
    
    thread2 = threading.Thread(target=server_coap.listen, args=(), kwargs={})
    thread2.start()

    print("\nSystem Started\n\n ")
    helper()

    try:
        while 1:

            command = input("COMMAND>")
            command = command.lower()
                
            menu(command, client)


    except KeyboardInterrupt:
        print("SHUTDOWN")
        os._exit(0)


if __name__ == '__main__':
    main_loop()