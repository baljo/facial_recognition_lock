# How to unlock a door with your face

![](/images/IMG_4468_cropped.jpg)


## Problem statement

There are moments when you wish you had three arms. Imagine arriving home with bags of groceries in both hands, rain pouring down, and needing to find your keys to unlock the door—without letting your groceries get wet and dirty.

There are similarly cases when you'd like something to happen when your face is identified. E.g. unlocking a medal cabinet only if you are in front of it, or starting a small fan when you are sitting at your desk.

## Solution

A solution to the grocery shopping and medal cabinet use cases is to install a facial recognition doorbell and door unlocking mechanism. As an additional benefit, you can receive notifications when someone, like a family member, is recognized and opens the door, or if an unknown person appears at the door. With only minor modifications you can modify the concept to the desk fan use case, or anything else you can imagine.

To build this facial recognition proof of concept (PoC), an M2 SoM Evaluation Board, a B524 SoM, and the Person Sensor from Useful Sensors were used. Initially, the plan was to create a facial recognition doorbell that signals and sends notifications when someone is in front of the sensor. To take this further, I aimed to enable facial identification, meaning identifying **who** the person is, rather than just recognizing the presence of a person.


## How does it work?

From a user perspective, the process is straightforward:

- The sensor continuously checks for faces in its view.
- If a face is detected:
  - It checks whether the face matches one of the eight identities it has been calibrated to recognize.
  - If a matching face is found, the Particle device verifies if that person is allowed to unlock the door and signals the servo accordingly.
  - A mobile notification is sent through Pushover, identifying the person detected.


## Bill of materials

- MCU: [Particle M.2 SoM Evaluation Board](https://store.particle.io/products/som-evaluation-board?_pos=1&_sid=a6a33f54b&_ss=r) 

![](/images/M2_eval_board_cropped_2.jpg)

- [B524](https://store.particle.io/products/b-series-lte-cat1-3g-2g-europe-ethersim?_pos=1&_sid=c3651bdc9&_ss=r), it has cellular and Bluetooth connectivity (no Wi-Fi), but if you want Wi-Fi instead of cellular connectivity, a [Photon 2 dev board](https://store.particle.io/products/photon-2?_pos=1&_sid=5598553d5&_ss=r) can be used and is more cost effective 

![](/images/B524_cropped.jpg)

- [Person Sensor by Useful Sensors](https://www.sparkfun.com/products/21231), priced at ~$10 USD!

![](/images/Person_Sensor_cropped.jpg)


- Optional, but recommended
  - separate power source for the servo or actuator, especially if the latter is power hungry or under load 
- Optional
  - LiPo battery, some [options](https://docs.particle.io/hardware/power/batteries/)
  - [Linear actuator kit](https://kitronik.co.uk/products/2595-linear-actuator?_pos=3&_sid=e648f346a&_ss=r), you can replace this with any servo, push rod, or larger linear actuator

![](/images/Linear_actuator_cropped.jpg)


  - [Qwiic Cable - Grove Adapter (100mm)](https://www.sparkfun.com/products/15109), this if you want an easy way of connecting the Person Sensor to the Grove port on the M.2 SoM Eval Board, another option is to use separate jumper wires.

![](/images/Qwiic_Grove_cable_cropped.jpg)


## Skills & tools required

For this project, you’ll need

- A Particle account and basic understanding of the Particle platform's capabilities.
- Particle-flavored, Arduino-style, C++ development.
- No soldering is required for this PoC
- Optional: 3D-printed case for the Eval Board, and a [3D-printed enclosure](https://thangs.com/designer/ZackFreedman/3d-model/Person%20Sensor%20Mount%20-%20Print-in-place%2C%20nothing%20but%20filament%21-836098) for the Person Sensor 


# Assembly

Only a few steps are needed to assemble the system (see the top image for a visual overview):
- Attach the B524 to the Eval Board
- Connect the cellular antenna to the Eval Board (the Bluetooth antenna can be left unconnected for this project).
- Connect the Person Sensor to the Eval Board, ensuring that you connect it to the I2C Grove port, not the analog port!

![](/images/Grove_connector_cropped.jpg)


- Optional: 
  - Connect a battery 
  - If using a 6V servo or actuator like in this project, it can be powered temporarily from the Eval Board. For long-term use, a separate power source is recommended.
    - In following images following wire colors are used: white = signal, black = ground, red = power
    - Connect the servo signal wire to **PWM0** (**D4** in the program)
    - Connect the ground wire to **GND** on the board


![](/images/PWM_GND_cropped.jpg)


  - Connect the power wire to **VCC** on the board

![](/images/Power_cropped.jpg)


- Power the B524 using the SOM power switch
- Connect the Eval board to your computer via an USB-cable
- If using a battery, charge it using the BAT POWER switch

![](/images/Switches_cropped_2.jpg)



### Optional: Test the servo

You can optionally verify the servo connection using [this test program](/backup/Servo.cpp), which moves the servo horn between its two endpoints. The program is optimized for the specific servo used in this project, so verify your servo's specifications before running it.

## Software apps and online services

- Visual Studio Code with the Particle Workbench extension installed
- Optional
  - Particle’s Webhook Integration for sending notifications to your mobile
  - Twilio, Pushover, or similar services for sending SMS or notifications
- The complete program can be found [here](/src/Person_sensor.cpp)

### Set up a Webhook integration

This assumes you are using the Pushover service, but the overall principle is similar, regardless of chosen service.

- Create an account at Pushover (or similar service, e.g. Twilio)
  - Take a note of the User Key and API token
- Also install the Pushover app on your mobile device
- In the Particle console, Go to `Integrations`
- Add a new integration
- Scroll down and select `Custom Webhook`
- Select ´Custom template`
- Paste in the code below
- Replace the `token` and `user` in the code with the ones from your chosen service 
- `event` - in this case *"face_detected"* - should be same event as you are publishing from your code
- Test the integration, if everything is set up correctly, you should get a notification on your mobile device
  - If you use Pushover, the notification on your mobile is received through the Pushover service, not as a SMS.


```
{
    "name": "Identified as person: ",
    "event": "face_detected",
    "responseTopic": "{{PARTICLE_DEVICE_ID}}/hook-response/{{PARTICLE_EVENT_NAME}}",
    "disabled": false,
    "template": "webhook",
    "url": "https://api.pushover.net/1/messages.json",
    "requestType": "POST",
    "noDefaults": true,
    "rejectUnauthorized": true,
    "unchunked": false,
    "dataUriResponseEvent": false,
    "form": {
        "token": "token to selected service",
        "user": "user id at selected service",
        "title": "Door opened by person ",
        "message": "{{{PARTICLE_EVENT_VALUE}}}"
    }
}
```

![](/images/webhook.jpg)



### Calibrate the Person Sensor

The idea with the Person Sensor is that it should behave similarly as any other sensor, i.e. send data  back to the calling device. It is pre-programmed with algorithms that detect nearby faces, and return information over a simple  I2C interface. It is designed to be used as an input to a larger system and be treated like any other sensor where power and information are provided through the I2C interface.

All this means that apart from a few configurations options, the sensor can't be programmed, which also makes it extremely tamperproof (100 %?). But how do we then get it to not only **recognize if a face is seen**, but also to **identify the face**? The solution here is called calibration.

#### Calibration

The calibration can in theory be done stand-alone, but for practical purposes a computer should be connected. This way you can verify the functionality via the terminal window: 
- Position the person whose face you want to store in front of the sensor. When the face is detected, the green LED will light up.
- Within ~200 ms, press the `Mode` button on the Eval Board to store the face's features and assign it an ID in the sensor's EEPROM. Up to eight faces can be stored, with IDs ranging from 0 to 7.
- Repeat this process for each new face you want to store.

Below the function which is reacting on the button event. The "magic" happens in this code line - `person_sensor_write_reg(NEXT_ID_REGISTER, nextFaceID);` - which stores the face's features and corresponding ID into EEPROM.

```
void button_clicked(system_event_t event, int param)
{
    int times = system_button_clicks(param);

    faceIDs[nextFaceID] = nextFaceID;                   // Assign new ID to face
    person_sensor_write_reg(NEXT_ID_REGISTER, nextFaceID);
    Serial.printlnf("******** %d ********", nextFaceID);
    nextFaceID++;
}
```


**Mode button**

![](/images/Mode_button_cropped.jpg)



### Verify facial identification ###

Both during the calibration process and under normal use you can follow what the program is doing via the terminal window. To open this, open the Command Palette (Ctrl+Shift+P) and search for `Particle: Serial Monitor`. If you want to redo the calibration, just recycle power to the device and start the calibration again.

Apart from verification and troubleshooting purposes, you should also take a note of which face is having which ID. E.g. if your face were registered with ID 0, and your childrens with 1 respectively 2, you need to remember this for next step.

### Set up the facial unlock configuration ###

If you want the servo to react - e.g. open a door - to specific faces, you need to change this code (around line 135):
`(face-> id == 3 || face -> id == 0)` to include the face or faces you want to be able to unlock the door. In this case my own face is registered twice, once with glasses and once without.

```
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
```

### Notification settings ###

With the Pushover service you can send 10000 notifications/month for free. Especially while setting up and testing this concept, I could easily consume close to 100 notifications per day. From a quota point of view this was still not an issue, but I got tired of the constant pings on my mobile and smartwatch. Hence I decided to send a notification if at least 10 seconds have gone since previous notification.

Furthermore, while you are testing the code, you can comment out the `Particle.puhlish...` function call.


## Security notes ##

According to Useful Sensors you shouldn’t rely on the facial identification results as a **sole** security factor, since it’s not designed to be that accurate. It’s intended for less safety-critical applications like personalizing a user experience automatically, where the cost of mistakes is lower.

You of course also needs to consider physical security of the device, and weather-proofing everything as per the location.

## Real-Life Solution Demonstration ##

In this video you can see how the blue "lock" on the left hand is ensuring no-one can steal my marathon medals. The hinge only unlocks when the sensor sees my face, and in this case automatically locks itself after 10 seconds.

![](/images/Facial_recognition_door_unlock.gif)

At the same time a notification is sent to my phone, informing who is opening the cabinet. This is in this use case useful as I can see if someone, apart from me, is in front of the sensor and perhaps trying to break in.


![](/images/IMG_4441_cropped.jpg)



# CONCLUSION #

This PoC and tutorial demonstrated how you with fairly simple devices can build a facial recognition and identification doorbell/notification device, and in addition how you can utilize it for e.g. unlocking purposes. This was also the first time I used a Particle device together with their IoT-platform, and although I didn't understand everything right away, I'm surprised how logical everything is, once I got over the initial hurdle!