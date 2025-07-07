
---

# 🌟 **Smart Battery Management System** 🌟

## 💡 **Introduction**

In today's world, home energy management is crucial, especially as we transition towards renewable energy sources like **solar panels**. **Smart home batteries** play a pivotal role by storing excess energy for later use, reducing our reliance on the grid, and extending the lifespan of renewable resources. But what if these batteries could be even smarter?

Enter this project! 🚀 Our **Smart Battery Management System** uses **IoT technologies** to create a fully automated and self-managing battery system. By gathering real-time data on the **State of Charge (SoC)** and **internal temperature** of the battery, the system intelligently adjusts the charging and discharging cycles, ensuring that the battery operates optimally and lasts longer. 🔋✨

The ultimate goal is to make the battery smarter, reducing the need for user intervention while keeping track of performance and health. This project makes that vision a reality! 🌍💪

### **Key Features**

* 🔋 **Autonomous Battery Management**: Automatically optimizes charging and discharging cycles based on real-time data.
* 📊 **Battery Monitoring**: Monitors critical parameters like **SoC** and **temperature** to ensure the battery stays in top condition.
* 🌐 **IoT-Enabled**: Uses **MQTT** and **CoAP** protocols to seamlessly gather sensor data and control actuators.
* 🗄️ **Data Logging**: Stores all sensor data in a **MySQL** database for easy analysis and tracking.
* 🔧 **Smart Actuators**: Uses smart switches to control the energy flow from external sources, allowing for dynamic battery charging and discharging.

---

## 🖥️ **System Architecture**

The system operates based on a **two-network architecture** that collects data and controls energy flow. The data collection is powered by **MQTT** and **CoAP** protocols, both of which allow for real-time communication with sensors.

#### **Architecture Overview:**

```
+-------------------+       +-------------------+       +---------------------+
|    Battery        |       |   Rele Actuator    |       |     Python Server    |
|    Sensor         |<----->|   (Smart Switch)   |<----->|      (Data Collector) |
+-------------------+       +-------------------+       +---------------------+
           |                        |                        |
      MQTT Protocol               CoAP Protocol            MySQL Database
```

---

## 🔧 **Components & Technologies**

### **1. Sensors**

* **Battery Sensor**: Measures the **State of Charge (SoC)** and **internal temperature** of the battery.
* **Rele Actuator**: A smart switch that manages energy flow from an external source, such as solar panels.

### **2. Data Collection**

* The **Python Collector** gathers data from **CoAP** and **MQTT** sensors and stores it in a **MySQL database**. It also handles the system logic to control the smart actuators and adjust their state based on collected data.

### **3. Communication Protocols**

* **MQTT**: Used for real-time data retrieval from sensors.
* **CoAP**: Allows direct communication with sensors to trigger actions and retrieve data.

### **4. Data Encoding**

The system uses **JSON** as the data exchange format for simplicity and efficiency. The **Battery Sensor** sends data in the following format:

```json
{
  "node": "node_id",
  "SoC": soc_value,
  "Temp": temp_value
}
```

---

## 🗃️ **Data Storage**

All collected data is stored in the **MySQL database** with the following two main entities:

1. **battery\_data**: Stores all data received from the battery sensor.
2. **rele\_data**: Stores the state of the **Rele Actuator** (whether it’s on or off).

The database is used to maintain a historical log of battery performance over time.


The system will begin collecting data, and you can interact with it using the **CLI**.

---

## 🎮 **Usage**

Once the system is running, interact with it via the **CLI**. Here are some commands you can use:

* **View System Data**: Check real-time data from your battery sensors.
* **Set Parameters**: Adjust settings, such as data collection intervals and actuator control logic.
* **Shutdown**: Gracefully stop the system and close database connections.

---

## 📊 **Future Enhancements**

Here are some features we are planning to implement next:

* **Real-time Web Dashboard**: Create a web-based interface to monitor battery data in real-time.
* **AI-Driven Optimization**: Use machine learning algorithms to predict battery states and optimize performance.
* **Mobile App Integration**: Control and monitor the system remotely via a mobile app.

---

## **Conclusion**

The **Smart Battery Management System** is a significant step toward automating home energy management, utilizing **IoT technologies** to optimize battery performance and extend lifespan. By leveraging **MQTT**, **CoAP**, and a **MySQL database**, the system makes it easy to monitor and control your home battery with minimal effort.

---

## **Author**
* **Pietro Calabrese**

---
