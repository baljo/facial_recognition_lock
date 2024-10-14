# How to unlock a door with your face

![](/images/IMG_4443_cropped.jpg)


## Problem statement

There are times when you wish you'd have three arms. For example when you arrive home with grocery shopping bags in both hands, rain is pouring down, and you need to find the door key to unlock and open the door, without letting the shopping bags get wet and dirty. One way to solve this is to use a facial recognition doorbell and door unlocking mechanism. As an additional benefit you can even get notifications when someone, e.g. a family member, has been recognized and opened the door as well as when an unknown person has stood in front of the door.

## Solution

To build this facial recognition PoC, a M2 SoM Evaluation Board and a B524 SoM was used, together with the Person Sensor from Useful Sensors. Initially the plan was to only create a facial recognition doorbell, i.e. signalling somehow and sending a notification when a person was in front of the sensor. But to take it a step further, I wanted to see if it was possible to also do facial identification and not only recognition, meaning identifying **who** a person is, not only that it is a person. 


## How does it work?

From a user perspective, this is fairly simple:
- The sensor is continously checking for persons (faces) in its view
- If a person is in front of the Person Sensor
  - It furcher checks if the face is one of the maximum eight identities it's been calibrated with, and if a matching face is found
    - The Particle device further checks if that person is allowed to open the lock, and signals the servo accordingly
    - A mobile device is notified through the Pushover service, the notification is containing the identity of the person seen


## Bill of materials

- MCU: [Particle M.2 SoM Evaluation Board](https://store.particle.io/products/som-evaluation-board?_pos=1&_sid=a6a33f54b&_ss=r) + [B524](https://store.particle.io/products/b-series-lte-cat1-3g-2g-europe-ethersim?_pos=1&_sid=c3651bdc9&_ss=r)
  - The B524 has cellular and Bluetooth connectivity (no Wi-Fi), but if you want Wi-Fi instead of cellular connectivity, a [Photon 2 dev board](https://store.particle.io/products/photon-2?_pos=1&_sid=5598553d5&_ss=r) can be used and is more cost effective 
- [Person Sensor by Useful Sensors](https://www.sparkfun.com/products/21231), price only ~$10 USD!
- Optional, but recommended
  - separate power source for the servo or actuator, especially if the latter is power hungry or under load 
- Optional
  - LiPo battery, some [options](https://docs.particle.io/hardware/power/batteries/)
  - [Linear actuator kit](https://kitronik.co.uk/products/2595-linear-actuator?_pos=3&_sid=e648f346a&_ss=r), you can replace this with any servo, push rod, or larger linear actuator
  - [Qwiic Cable - Grove Adapter (100mm)](https://www.sparkfun.com/products/15109), this if you want an easy way of connecting the Person Sensor to the Grove port on the M.2 SoM Eval Board, another option is to use separate jumper wires.

## Skills & tools required

For this project, you’ll need

- A Particle account and a basic understanding of the Particle platform capability
- Particle-flavored, Arduino-style, C++ development experience
- No soldering needed for this PoC
- Optional: 3D-printed case for the Eval Board, and a [3D-printed enclosure](https://thangs.com/designer/ZackFreedman/3d-model/Person%20Sensor%20Mount%20-%20Print-in-place%2C%20nothing%20but%20filament%21-836098) for the Person Sensor 


## Assembly

There's only a few things to assemble, see the top picture for a visual overview
- Attach the B524 to the Eval Board
- Attach at least the cellular antenna to the Eval Board, I've left the Bluetooth antenna unconnected
- Connect the Person Sensor to the Eval Board, ensure you are connecting to the I2C Grove port and not the analog!
- Optional: 
  - Connect a battery 
  - If you use a 6V servo or actuator, like in this project, you can temporarily power it from the Eval Board. For a long-term solution, I strongly recommend a separate power source!
    - Connect the servo signal wire to **PWM0** (= **D4** in the program)
    - Connect the ground wire to **GND** on the board
    - Connect the power wire to **VCC** on the board
- Power the B524 with the SOM power switch
- Connect the board to your computer with an USB-cable
- If you use a battery, charge it with the BAT POWER switch

**Picture of servo wiring: white = signal, black = ground, red = power**

![](/images/IMG_4446_cropped.jpg)


**Battery & USB cables, switches**

![](/images/IMG_4451_annotated_cropped.jpg)


### Optional: Test servo

This is optional, but if you want to verify the servo connection is working, you can test it with this [program](/backup/Servo.cpp). which is simply moving the servo horn between its two endpoints. The program is optimized for the servo I used, verify the specifications of your own serve before you run the program!

## Software apps and online services

- Visual Studio Code with the Particle Workbench extension installed
- Optional
  - Particle’s Webhook Integration for sending notifications to your mobile
  - Twilio, Pushover, or similar service to send SMS or notifications
- The whole program is [here](/src/Person_sensor.cpp)

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

### Calibrate the Person Sensor

The idea with the Person Sensor is that it should behave similarly as any other sensor, i.e. send data  back to the calling device. It is pre-programmed with algorithms that detect nearby faces, and return information over a simple  I2C interface. It is designed to be used as an input to a larger system and be treated like any other sensor where power and information are provided through the I2C interface.

All this means that apart from a few configurations options, the sensor can't be programmed, which also makes it extremely tamperproof (100 %?). But how do we then get it to not only **recognize if a face is seen**, but also to **identify the face**? The solution here is called calibration.

#### Calibration

The calibration can in theory be done stand-alone, but for practical purposes a computer should be connected. This way you can verify the functionality via the terminal window: 
- First the person whose face is to be stored needs to be in front of the Person Sensor. When the sensor sees a face, its green led is lit.
- As quickly as possible (within ~200 ms), the `Mode` button on the Eval board needs to be briefly pressed. This stores the features of the face together with a corresponding ID into the sensor's EEPROM. Eight faces can be stored, so the ID's are 0 - 7.
- Repeat above for each new face you want to store in the sensor.

Below the function which is reacting on the button event. The "magic" happens in this code line - `person_sensor_write_reg(NEXT_ID_REGISTER, nextFaceID);` - which stores the face features and corresponding ID into EEPROM.

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

![](/images/IMG_4453_cropped.jpg)



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