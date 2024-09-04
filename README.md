# Heart Rate Detection Smart Band for the Elderly

## Demo Video
[Link](https://drive.google.com/file/d/1Aam6eSixOUvvWoH8iO0sOS4HKPToakpm/view)

## Schematic
![N|Solid](https://github.com/fafnir96/Smart-Band-Pendeteksi-Detak-Jantung-Untuk-Lansia/blob/main/Skematik.jpg?raw=true)

## Purpose
The purpose of this research is to design and develop a wearable device in the form of a smart band, which functions as a heart rate detector for the elderly. This device is expected to make a significant contribution to real-time health monitoring for the elderly, thereby reducing the risk of sudden medical conditions. The primary objectives of this device are:

1. Detecting Heart Rate Anomalies: Ensuring that the device can accurately read and monitor the elderly's heart rate using a pulse heart sensor. If the heart rate exceeds 100 BPM or falls below 60 BPM, the device will identify this condition as an anomaly.
2. Automatically Sending Location Notifications: Developing a system that can automatically send notifications containing the elderly's location data via WhatsApp to family members or caregivers when an abnormal heart rate is detected. This is achieved using the GPS Neo Ubox sensor.
3. Providing Information Through Visual Display: Displaying real-time heart rate information on an OLED screen, allowing the elderly or caregivers to easily monitor their health condition at any time.
4. Enhancing Safety and Quick Response: Developing technology that supports quick responses to emergency situations by integrating an efficient communication system, thereby improving the safety and well-being of the elderly.


## Setup
1. Clone project
2. Build the circuit according to the schematic
3. Open project in Arduino IDE
4. Visit this link to get the WhatsApp API: `https://randomnerdtutorials.com/esp32-send-messages-whatsapp/`
4. Change the Wi-Fi SSID and password, as well as the phone number and API Key in the code.
5. Upload the code to ESP32
