// This program detects faces with a Person Sensor, publishes signal and battery data,
// and blinks the user LED when a face is found. 
// Created on: 2024-10-08 14:00:00, by Thomas Vikström

#include <Wire.h>
#include "person_sensor.h"
#include "Particle.h"


// How long to wait between reading the sensor. The sensor can be read as
// frequently as you like, but the results only change at about 5FPS, so
// waiting for 200ms is reasonable.
const int32_t SAMPLE_DELAY_MS = 200;

#define USER_LED_PIN D7  // User LED pin
#define MODE_REGISTER 0x01
#define ENABLE_ID_REGISTER 0x02
#define NEXT_ID_REGISTER 0x04
#define PERSIST_ID_REGISTER 0x05

const int MAX_FACES = 8; // Maximum number of faces to track
int faceIDs[MAX_FACES]; // Array to hold IDs for detected faces
int nextFaceID = 0; // Next available ID
int lastFaceID = 99;

unsigned long lastPublishTime = 0;                  // Stores the last publish time
const unsigned long publishInterval = 10000;        // Minimum interval between publishes (in milliseconds)
unsigned long lastOpenTime = 0;                     // Stores the last publish time
const unsigned long openInterval = 10000;           // Minimum interval between publishes (in milliseconds)
bool door_open = false;

Servo myservo;  // create servo object to control a servo

int servo_pos = 0;    // variable to store the servo position


void button_clicked(system_event_t event, int param)
{
    int times = system_button_clicks(param);

    faceIDs[nextFaceID] = nextFaceID;                   // Assign new ID to face
    person_sensor_write_reg(NEXT_ID_REGISTER, nextFaceID);
    Serial.printlnf("******** %d ********", nextFaceID);
    nextFaceID++;
}

void open()  {
  for(servo_pos = 0; servo_pos < 170; servo_pos += 1)   // goes from 0 degrees to xxx degrees
  {                                                     // in steps of 1 degree
    myservo.write(servo_pos);                           // tell servo to go to position in variable 'pos'
    delay(5);                                           // waits for the servo to reach the position
  }

}

void close()  {
  for(servo_pos = 170; servo_pos>=1; servo_pos-=1)      // goes from xxx degrees to 0 degrees
  {
    myservo.write(servo_pos);                           // tell servo to go to position in variable 'pos'
    delay(5);                                           // waits  for the servo to reach the position
  }
}

void setup() {
    myservo.attach(D4);                                 // attaches the servo on the D0 pin to the servo object

    Wire.begin();                                       // Initialize I2C
    Serial.begin(9600);                                 // Start Serial communication
    pinMode(USER_LED_PIN, OUTPUT);                      // Set user LED pin as output

    Wire.beginTransmission(PERSON_SENSOR_I2C_ADDRESS);
    Wire.write(MODE_REGISTER);
    Wire.write(0x01);                                   // continuous moude
    Wire.write(ENABLE_ID_REGISTER);
    Wire.write(0x01);                                   // enable ID model
    Wire.write(PERSIST_ID_REGISTER);    
    Wire.write(0x01);                                   // Set persist IDs to true
    Wire.endTransmission();

    Serial.println("Person Sensor setup complete. ID persistence enabled.");
    System.on(button_click, button_clicked);
}

void blink(int times, int pause)  {
  for (int i = 1; i < times + 1; i++)  {
    digitalWrite(USER_LED_PIN, HIGH);
    delay(pause);                                       // LED on for pause ms
    digitalWrite(USER_LED_PIN, LOW);  
    delay(pause);                                       // LED off for pause ms
  }
}

void loop() {

    person_sensor_results_t results = {};

    if (!person_sensor_read(&results)) {
        Serial.println("No person sensor results found on the I2C bus");
        delay(SAMPLE_DELAY_MS);
        return;
    }

    // blinking if new face found
    if (nextFaceID != lastFaceID) {
      blink(nextFaceID, 50);
      lastFaceID = nextFaceID;
    }

 
    for (int i = 0; i < results.num_faces; ++i) {
        const person_sensor_face_t* face = &results.faces[i];
        Serial.print("Face #");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(face->box_confidence);
        Serial.print(" confidence, (");
        Serial.print(face->box_left);
        Serial.print(", ");
        Serial.print(face->box_top);
        Serial.print("), (");
        Serial.print(face->box_right);
        Serial.print(", ");
        Serial.print(face->box_bottom);
        Serial.print("), ");
        if (face->is_facing) {
            Serial.println("facing");
        } else {
            Serial.println("not facing");
        }
        Serial.print("Recognition ID: ");
        Serial.println(face->id);

        // opening the door if it's closed and enough time has passed, also publishing to Particle
        unsigned long currentTime = millis();
        if ( (currentTime - lastPublishTime >= publishInterval) && (face-> id == 3 || face -> id == 0) && door_open == false) {
            Particle.publish("face_detected", String(face->id), PRIVATE);
            open();
            Serial.println("** OPENING **");
            door_open = true;
            lastPublishTime = currentTime;
            lastOpenTime = currentTime;

        }
     }


    // don't want to close the door right away, instead waiting for a defined time
    unsigned long currentTime2 = millis();
    if (currentTime2 - lastOpenTime >= openInterval) {
        if (door_open == true) {
            close();
            Serial.println("** CLOSING **");
            door_open = false;
        }
    }

    delay(SAMPLE_DELAY_MS); 
}

