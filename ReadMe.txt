OfflineSolution uses the Arduino IDE and its libraries to facilitate an automatic irrigation solution, offline. This was use for testing throughout the project.

BlynkOnlineSolution uses the Aruino IDE and its libraries alongside the Blynk libraries to take the functionality of the OfflineSolution code, create a connection
with a cloud server, and upload sensor data to the cloud via the virtualWrite command offered by the Blynk library.
Note that the network details need to be configured to a nearby network that the ESP32 can connect (has to be 2.4ghz/ can't be 5ghz netwrok). 
The authentication token is received from the Blynk website when a new template is made, to connect the code to a cloud server, so it can be hosted on the cloud.

To test the code, it can be verified in the IDE to check if any errors are present or it can be uploaded to a connected ESP32 module by selecting
the board, "ESP32 Dev Module"

To open the file containing the code Arduino IDE needs to be installed at: https://www.arduino.cc/en/software

Or check the code here on github: https://github.com/19705317/ESP32-Smart-Irrigation




