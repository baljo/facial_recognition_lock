# How to unlock a door with facial recognition

![](/images/IMG_4443_cropped.jpg)


## Problem statement

There are times when you wish you'd have three arms. For example when you arrive home with grocery shopping bags in both hands, rain is pouring down, and you need to find the door key to unlock and open the door, without letting the shopping bags get wet and dirty. One way to solve this is to use a facial recognition doorbell and door unlocking mechanism. As an additional benefit you can even get notifications when someone, e.g. a family member, has been recognized and opened the door as well as when an unknown person has stood in front of the door.

## Solution

To build this facial recognition PoC, a M2 SoM Evaluation Board and a B524 SoM was used, together with the Person Sensor from Useful Sensors. Initially the plan was to only create a facial recognition doorbell, i.e. signalling somehow and sending a notification when a person was in front of the sensor. But to take it a step further, I wanted to see if it was possible to also do facial identification and not only recognition, meaning identifying **who** a person is, not only that it is a person. 


## Background

## Project plan

## Bill of materials

- MCU: [Particle M.2 SoM Evaluation Board](https://store.particle.io/products/som-evaluation-board?_pos=1&_sid=a6a33f54b&_ss=r) + [B524](https://store.particle.io/products/b-series-lte-cat1-3g-2g-europe-ethersim?_pos=1&_sid=c3651bdc9&_ss=r)
  - The B524 has cellular and Bluetooth connectivity (no Wi-Fi), but if you want Wi-Fi instead of cellular connectivity, a [Photon 2 dev board](https://store.particle.io/products/photon-2?_pos=1&_sid=5598553d5&_ss=r) can be used and is more cost effective 
- [Person Sensor by Useful Sensors](https://www.sparkfun.com/products/21231), price only ~$10 USD
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

There's only a few things to assemble
- Attach the B524 to the Eval Board
- Attach at least the cellular antenna to the Eval Board, I've left the Bluetooth antenna unconnected
- Connect the Person Sensor to the Eval Board, ensure you are connecting to the digital Grove port and not the analog!
- If you use a 6V servo or actuator, like in this project, you can temporarily power it from the Eval Board. For a long-term solution, I strongly recommend a separate power source!
  - Connect the servo signal wire to PWM0 (= D4 in the program)
  - Connect the ground wire to GND on the board
  - Connect the power wire to VCC on the board

## Software apps and online services

## The code

AAA
![](/images/IMG_4441_cropped.jpg)

BBB
![](/images/Facial_recognition_door_unlock.gif)
