from coap_handler import Put_charging_state as put


class SystemStatus:

    minSOC = 21
    maxSOC = 85
    minTemp = 10
    maxTemp = 42

    SOC = 0
    temp = 0

    manual = 0

    charging = "0"


    def get_system_status():
        mode = "ON" if SystemStatus.manual == 1 else "OFF"
        print("---------- Il Sistema è nello stato ---------- \n"\
              "State of Charge: " + str(SystemStatus.SOC) + "\n"
              "Battery Temperature: " + str(SystemStatus.temp) + "\n"\
              "Charging: " + str(SystemStatus.charging) + "\n"\
              "Manual Mode: " + mode + "\n"\
              "---------------------------------------------- \n")

    def get_parameters():
        print("---------- Paramentri Attuali ---------- \n"\
              "min SOC: " + str(SystemStatus.minSOC) + "\n"
              "Max SOC: " + str(SystemStatus.maxSOC) + "\n"
              "min Temp: " + str(SystemStatus.minTemp) + "\n"
              "Max Temp: " + str(SystemStatus.maxTemp) + "\n"
               "---------------------------------------------- \n")
    def change_parameters():

        SystemStatus.get_parameters()

        SystemStatus.minSOC, SystemStatus.maxSOC, SystemStatus.minTemp, SystemStatus.maxTemp = map(int, input("Enter 4 integers separated by spaces, minSoC MaxSoC minTemp MaxTemp: ").split())

        print("---------- Nuovi Paramentri ---------- \n"\
              "min SOC: " + str(SystemStatus.minSOC) + "\n"
              "Max SOC: " + str(SystemStatus.maxSOC) + "\n"
              "min Temp: " + str(SystemStatus.minTemp) + "\n"
              "Max Temp: " + str(SystemStatus.maxTemp) + "\n"
               "---------------------------------------------- \n")
            
    def manual_override(client):

        if SystemStatus.manual == 0:
            SystemStatus.manual = 1
            SystemStatus.charging = "1"
            put.put_charging_state("man", "1")
            client.publish("charging","1")
        else:
            SystemStatus.manual = 0
            SystemStatus.charging = "0"
            put.put_charging_state("off", "0")
            client.publish("charging","0")
