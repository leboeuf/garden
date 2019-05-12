# Garden (Raspberry Pi)

This project describes how I automated watering my plants.

## Components

* Raspberry Pi
* MSP430
* 2 solenoid valves
* 1 submersible pump
* 1 breadboard
* 1 moisture sensor (custom made)
* 1 water level sensor
* tubing

# MSP430

The MSP430 (specifically the MSP430G2231 Launchpad) is a low-cost, low-power programmable microcontroller. It is used in this project for its ADC (Analog-to-digital converter) peripheral.

# Raspberry pi setup

1. Download the latest minimal version of raspbian and flash it to an SD card with balenaEtcher.

2. Create an empty file called `ssh` at the root of the SD card.

3. Create a file called `wpa_supplicant.conf` at the root of the SD card with the following content:

```
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=CA

network={
    ssid="your wifi SSID"
    psk="your wifi password"
    key_mgmt=WPA-PSK
}
```

4. Boot up the Raspberry pi and login using the credentials: `pi:raspberry`. Change the default password using `passwd`.

5. (Optionally) Run the following command and configure everything: `raspi-config`.

