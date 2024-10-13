// // This program detects faces with a Person Sensor, publishes signal and battery data,
// // and blinks the user LED when a face is found. 
// // Created on: 2024-10-08 14:00:00, by Thomas Vikström

// #include <Wire.h>
// #include "person_sensor.h"
// #include "Particle.h"

// // Function declarations
// int getBatteryLevel(); // Function to get battery level
// void publishSignalAndBatteryData(); // Function to publish signal and battery data

// // How long to wait between reading the sensor. The sensor can be read as
// // frequently as you like, but the results only change at about 5FPS, so
// // waiting for 200ms is reasonable.
// const int32_t SAMPLE_DELAY_MS = 200;

// #define USER_LED_PIN D7  // User LED pin
// #define PERSIST_ID_REGISTER 0x05

// const int MAX_FACES = 10; // Maximum number of faces to track
// int faceIDs[MAX_FACES]; // Array to hold IDs for detected faces
// int nextFaceID = 0; // Next available ID



// void setup() {
//     Wire.begin(); // Initialize I2C
//     Serial.begin(9600); // Start Serial communication
//     pinMode(USER_LED_PIN, OUTPUT); // Set user LED pin as output

//     // Enable ID persistence
//     Wire.beginTransmission(PERSON_SENSOR_I2C_ADDRESS);
//     Wire.write(PERSIST_ID_REGISTER); // Register for persisting IDs
//     Wire.write(0x01); // Set persist IDs to true
//     Wire.endTransmission();

//     Serial.println("Person Sensor setup complete. ID persistence enabled.");
//     Particle.variable("battery", getBatteryLevel()); // Register battery variable
//     Particle.publish("status", "Device is online", PRIVATE); // Initial status publish
// }

// void loop() {
//     //publishSignalAndBatteryData(); // Publish signal and battery data

//     person_sensor_results_t results = {};
//     // Read current face information from the sensor
//     if (!person_sensor_read(&results)) {
//         Serial.println("No person sensor results found on the I2C bus");
//         delay(SAMPLE_DELAY_MS);
//         return;
//     }

//     Serial.println("********");
//     Serial.print(results.num_faces);
//     Serial.println(" faces found");

//     if (results.num_faces > 0) {
//         // Blink user LED if a face is found
//         digitalWrite(USER_LED_PIN, HIGH);
//         delay(100);  // LED on for 100ms
//         digitalWrite(USER_LED_PIN, LOW);
//     }

//     for (int i = 0; i < results.num_faces; ++i) {
//         const person_sensor_face_t* face = &results.faces[i];
//         Serial.print("Face #");
//         Serial.print(i);
//         Serial.print(": ");
//         Serial.print(face->box_confidence);
//         Serial.print(" confidence, (");
//         Serial.print(face->box_left);
//         Serial.print(", ");
//         Serial.print(face->box_top);
//         Serial.print("), (");
//         Serial.print(face->box_right);
//         Serial.print(", ");
//         Serial.print(face->box_bottom);
//         Serial.print("), ");
//         if (face->is_facing) {
//             Serial.println("facing");
//         } else {
//             Serial.println("not facing");
//         }
//         Serial.print("Recognition ID: ");
//         Serial.println(face->id);

//         // Serial.println("face_detected" + String(face->id));
//         // Particle.publish("face_detected", String(face->id), PRIVATE);

//         // Check if the face is already recognized
//         bool found = false;
//         for (int j = 0; j < MAX_FACES; ++j) {
//             if (faceIDs[j] == face->id) {
//                 found = true;
//                 Serial.print("Recognition ID: ");
//                 Serial.println(face->id); // Print existing ID
//                 Serial.println("face_detected" + String(face->id));
//                 Particle.publish("face_detected", String(face->id), PRIVATE);
//                 break;
//             }
//         }

//         // If face is new, assign a new ID
//         if (!found && nextFaceID < MAX_FACES) {
//             faceIDs[nextFaceID] = face->id; // Assign new ID to face
//             Serial.print("New Face Assigned ID: ");
//             Serial.println(nextFaceID);
//             Serial.println("face_detected" + String(nextFaceID));
//             Particle.publish("face_detected", String(nextFaceID), PRIVATE);
//             nextFaceID++; // Increment for the next face
//             delay(200); // Delay to avoid duplicates
//         }


// /*
//         // Publish an event if face ID 0 is detected
//         if (face->id == 0) {
//             Particle.publish("face_0_detected", "Face ID 0 has been detected", PRIVATE);
//             delay(2000); // Delay to avoid duplicates
//         }
// */
//     }
//     delay(SAMPLE_DELAY_MS); // Delay for the next loop
// }

// void publishSignalAndBatteryData() {
//     // Get battery level
//     int batteryLevel = getBatteryLevel(); 
//     // Get signal strength
//     int signalStrength = Cellular.RSSI();

//     // Publish battery level
//     Particle.publish("battery_level", String(batteryLevel), PRIVATE);
    
//     // Publish signal strength
//     Particle.publish("signal_strength", String(signalStrength), PRIVATE);

//     // Log to Serial for debugging
//     Serial.print("Battery Level: ");
//     Serial.println(batteryLevel);
//     Serial.print("Signal Strength: ");
//     Serial.println(signalStrength);
// }

// int getBatteryLevel() {
//     // Replace this with the actual code to read battery level
//     return System.batteryCharge(); // Placeholder value
// }
