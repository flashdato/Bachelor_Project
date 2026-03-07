# Bachelor Project: Automated and Remotely Controlled Irrigation Systems

This repository serves as the central hub for my bachelor's project development, including hardware architecture, firmware, and research documentation.

> **Project Start Date:** March 7, 2026

## 📄 Live Documentation
The most up-to-date version of the project documentation (Research, Market Analysis, and Technical Specifications) is maintained on Google Docs:
👉 [View Live Document Here](https://docs.google.com/document/d/1qkrIf-M12A-TOWwefRMs7QSAykDUy9xsk56rvhWRvOg/edit?usp=sharing)

---

## 🛠 Project Components & Roadmap

### 1. Hardware Architecture (`/Hardware`)
This project utilizes a dual-processor approach for optimal performance:
* **STM32:** Primary low-level controller handling sensor data acquisition and real-time valve control. Includes custom-built libraries for hardware abstraction.
* **ESP32:** Communication gateway managing Wi-Fi connectivity, remote access protocols, and cloud integration.

### 2. Firmware & Libraries (`/Firmware`)
* **STM32-Library:** Custom drivers for sensors and actuators being developed during the project lifecycle.
* **ESP-Wireless:** Scripts and logic for MQTT/Websocket remote control.

### 3. Visuals & Demonstrations (`/Media`)
* **Handmade Prototype Photos:** Real-time progress photos of the hardware assembly and PCB design.
* **Video Demos:** Demonstrations showing the system responding to remote triggers and sensor thresholds.

---

## 📁 Repository Structure

### 📚 Research & Resources (`/Research_and_Sources`)
This folder contains PDF archives and documentation used for benchmarking and technical study:
- **/Market_Analysis**: Reports and PDFs regarding the global irrigation market size and trends.
- **/Industry_Competitors**: Documentation, whitepapers, and product specs from leading companies (Netafim, Rachio, Hunter, etc.).
- **/Technical_Modules**: Datasheets and technical manuals for specific sensors, solenoids, and wireless modules chosen for this project.

### 📁 Core Files
- **/Manual_Uploads**: Periodic snapshot exports (PDF/Markdown) of the Google Doc.
- **/Hardware**: STM32/ESP32 schematics, pinout maps, and wiring diagrams.
- **/Firmware**: Source code and custom-built libraries for the controllers.
- **/Media**: Photos and videos of the "Handmade" physical prototype.