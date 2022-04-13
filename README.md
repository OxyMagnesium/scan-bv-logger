# SCAN voltage logger

## Introduction

This is a simple Arduino-based battery monitoring system that simply records the voltage of a 12 V battery (LiFePO<sub>4</sub> in this case) and stores it on a Micro SD card. This was made due to unreliable transmission from the subject scanner unit's LoRa system; the proven and extensible nature of an Arduino platform is a great interim tool while custom monitoring hardware is developed.

## Voltage divider

A voltage divider is used to drop the battery voltage down to a level that can be read by the Arduino's analog inputs. Since the battery has a maximum voltage of around 13.8 V, a ratio of 5:14 was chosen for the design goal in order to get maximum resolution. This was achieved using a 51k resistor and a 91k resistor. However, for reasons unknown, it was found after assembly that the Arduino analog input was saturating at 4.1 V instead of the 5 V it should be capable of achieving. Thus, a 68k resistor was soldered in parallel to the 51k resistor in order to bring the ratio down to 3.4:14.

## Future expansion

As mentioned before, this system is highly extensible, and though it was initially deployed with bare minimum functionality, without too much effort it is possible to add:

- A second voltage divider for solar panel output voltage measurement

- Temperature measurement using a thermistor

- Ambient light sensing using an LDR

- Current measurement using a shunt
