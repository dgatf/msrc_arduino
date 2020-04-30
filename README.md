## Remark
Code has been completely rewritten from v0.3 to v0.4. Name of the project has been changed. This project started around esc telemetry, but now is a general sensors telemetry with focus on esc and helis

# MSRC - Multi Sensor for RC - FrSky Smartport

This is a DIY project to send multiple sensor telemetry to Frsky Smartport using an Arduino Pro Mini 328P (3.3v or 5v)

## Telemetry

### ESC

The ESC telemetry can be ESC serial data or PWM signal (from the ESC or RPM sensor)

ESC protocols implemented:

- Hobbywing Platinum V3: RPM
- Hobbywing Platinum V4, Hobbywing Flyfun V5: RPM, temperature, voltage and current
- PWM signal: RPM
- Castle Link Live: RPM , voltage, ripple voltage, current, bec voltage, bec current, temperature, temperature NTC

Average cell voltage for HW V4/V5 is calculated for 3S,4S,5S,6S,7S,8S,10S and 12S batteries. 10 seconds after power on cell count is autodetected and fixed (average cell voltage to be >3.8v for proper cell count)

### Analog sensors

The following analog sensors are available:

- 2 x voltage divider can be added to read the battery voltage (A2, A3)
- 2 x temperature sensors (thermistors) (A0, A1)
- Current sensor (A6)

### I2C sensors

Multiple I2C sensors can be added (A4, A5)

Currently supported:

- Barometer: BMP180, BMP280

## PWM output

PWM signal generation from ESC serial (some HW V5 don't have RPM PWM output) 

PWM signal properties: logic level 3.3V and default duty cycle 50%

## Circuit

Minimum circuit is the arduino connected to esc or rpm sensor and smartport. Although it is not mandatory to connect to the esc or rpm sensor 

<p align="center"><img src="./images/msrc_serial.png" width="600"><br>
  <i>Minimum circuit ESC serial</i><br><br></p>

<p align="center"><img src="./images/msrc_pwm.png" width="600"><br>
  <i>Minimum circuit PWM signal</i><br><br></p>

<p align="center"><img src="./images/msrc_castle.png" width="600"><br>
  <i>Minimum circuit Castle Link</i><br><br></p>

<p align="center"><img src="./images/msrc_full.png" width="600"><br>
  <i>Additional sensors</i><br><br></p>

## Flash to Arduino

Using Arduino IDE copy folder *esc_smartport* and open *esc_smartport.ino*. Select board *Arduino Pro or Pro Mini*, processor *ATMega328P (3.3V 8MHz or 5V 16MHz)* and flash

## Configuration

The configuration is modified with a lua script (X7, X9, X-lite and Horus with openTx 2.2 or higher)

<p align="center"><img src="./images/lua_x7.png" height="128">   <img src="./images/lua_x9.png" height="128">   <img src="./images/lua_x10.png" height="200"></p>

Copy the file escSp.lua to the SCRIPTS folder in the sdcard of the Tx and execute as one-time script from SD-HD-CARD screen (long press and Execute). Since opentx 2.3 copy to SCRIPTS/TOOLS for easier access. It can be executed also as telemetry script if copied to TELEMETRY folder and assigned to a model telemetry screen

If not using lua script comment *#define CONFIG_LUA* and assign config values in esc_smartport.h

Options:

- ESC protocol. HobbyWing Platinum V3, HobbyWing Platinum V4/Hobbywing Flyfun V5 or PWM signal
- Voltage1. For voltage divider 1
- Voltage2. For voltage divider 2
- Ntc1. Thermistor 1
- Ntc2. Thermistor 2
- Current
- PWM out. To generate PWM output from ESC serial  (for obbywing Flyfun V5)
- Averaging queue size: 1 to 16
- Refresh rate (ms): 0 to 1600

## OpenTx sensors

The arduino default sensor id is 10. This can be changed with [change_id_frsky](https://github.com/dgatf/change_id_frsky)

Depending on your configuration you may have some or all of the following sensors in Opentx:
 
ESC telemetry:

- ESC RPM: EscR (0x0b60)
- ESC voltage: EscV (0x0b50)
- ESC cell average: VFAS (0x0210)
- ESC current: EscA (0x0b50)
- ESC temp FET (HW) or ESC temp linear (Castle): EscT (0x0b70)
- ESC temp BEC (HW) or ESC temp NTC (Castle): EscT (0x0b71)
- ESC ripple voltage: EscV (0x0b51)
- ESC BEC voltage: EscV (0x0e50)
- ESC BEC current: EscC (0x0e50)

Analog telemetry:

- Voltage 1: A3 (0x0900)
- Voltage 2: A4 (0x0910)
- Thermistor 1: Tmp1 (0x0400)
- Thermistor 2: Tmp2 (0x0410)
- Current: Curr (0x020f)

I2C telemetry:

 - Alt
 - Temp
 
Some of them needs to be adusted

### Adjust RPM sensor (EscR)

- Blades/poles: number of pair of poles * main gear teeth  
- Multiplier: pinion gear teeth

### Adjust voltage sensors (A3, A4)

Measure the voltage of the battery with a voltmeter and adjust *Ratio* in A3, A4 sensor

### Adjust current sensor (Curr)

Adjust sensor ratio: *1000 / output sensitivity (mV/A)*

To get battery consumption add a new sensor:

- Type: Calculated
- Formula: Consumption
- Sensor: Curr

## Images

<p align="center"><img src="./images/top.jpg" width="300">  <img src="./images/bottom.jpg" width="300"></p>

<p align="center"><img src="./images/450_1.jpg" width="300">  <img src="./images/450_2.jpg" width="300"></p>

<p align="center"><img src="./images/450_3.jpg" width="300">  <img src="./images/450_x7.bmp" width="300"><br><i>MSRC on Align 450 connected to Hobbywing V3 Platinum and two thermistors for ESC and motor</i><br></p>



## Video

[Video](https://youtu.be/Mby2rlmAMlU)


## Annex

### ESC protocol specifications Hobbywing

Serial parameters:

- 19200 bps
- 1 packet every 20 ms
- Big endian


#### Hobbywing V3

Byte | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10
--- | --- | --- | --- | --- | --- | --- | --- | --- | --- | ---
Value | Package Head (0x9B) | Package Number 1 | Package Number 2 | Package Number 3 | Rx Throttle 1 | Rx Throttle  2 | Output PWM 1 | Output PWM 2 | RPM Cycle 1 | RPM Cycle 2

*RPM = 60000000 / RPM Cycle*

rpm, pwm: 0-255

#### Hobbywing V4/V5

Byte | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19
--- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | ---
Value | Package Head (0x9B) | Package Number 1 |	Package Number 2 | Package Number 3	| Rx Throttle 1	| Rx Throttle  2 | Output PWM 1 | Output PWM 2	| RPM 1 | RPM 2	| RPM 3	| Voltage 1 |	Voltage 2	| Current 1	| Current 2	| TempFET 1	| TempFET 2	| Temp 1 |	Temp 2

Voltage, current and temperature are raw sensor data. Actual values requires transformation

rpm, pwm: 0-1024

Remark: Before throttle is raised from 0, programming packets are sent between telemetry packets:

Byte | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13
--- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | ---
Value | 0x9B | 0x9B	| 0x3	| 0xE8	| 0x1	| 0xB	| 0x41	| 0x21	| 0x44	| 0xB9	| 0x21	| 0x21	| 0xB9

### ESC protocol specifications Castle Link

See [Castle Link Live](https://dzf8vqv24eqhg.cloudfront.net/userfiles/4671/6540/ckfinder/files/Product%20Manuals/Accessories%20and%20replacement%20parts/castle_link_live_2_0.pdf?dc=201606221536-537)

### Analog voltage sensors. Voltage divider circuit

Two battery voltages can be measured through the analog pins A2 and A3
Metal resistors are recommended as gives more accurate readings (0.1W or higher)
Arduino can read up to 3.3V/5V and is optimized for signal inputs with 10K impedance

To select R values apply formulas:

*Vo = Vi\*R2/(R1+R2) < 3.3V or 5V*

*Z = 1/((1/R1)+(1/R2)) < 10K*

<p align="center"><img src="./images/Resistive_divider.png" width="200"></p>

For 6S battery (or lower) and Pro Mini 3.3v:

 - R1 68k
 - R2 10k

If more than 6S change R values or you may burn the Arduino!

### Temperature sensors. Thermistors

Two temperature sensors can be installed through the analog pins A0 and A1
Temperature is measured with NTC thermistors (100k). Adjust thermistor Beta in esc_smartport.h if needed (NTC_BETA, default is 4190). Sensor output in Celsius

Using Beta formula:

*T = 1/[ln(R/Ro)/B+1/To]*

More accurate formula (Steinhart and Hart Equation) if data available:

*T = 1/[A+Bln(R/Ro)+Cln(R/Ro)²+Dln(R/Ro)³]*

## Change log

v0.4.1

- Add Castle Link Live protocol

[v0.4](https://github.com/dgatf/msrc/tree/v0.4)

- Change R3 resistor to 3.3k
- Support for [change_id_frsky](https://github.com/dgatf/change_id_frsky) to change the sensor id
- Support for I2C sensors 
- Improved code quality and performance
- [Smartport_library](https://github.com/dgatf/smartport) improved performance and abstract from the smartport protocol

[v0.3.1](https://github.com/dgatf/msrc/tree/v0.3.1)

- Added cell voltage average (HW V4/V5, VFAS sensor)
- Applied correct transformation for esc voltage, current and temperature (HW V4/V5)
- Changed averaging type from SMA to EMA
- Added esc protocol NONE
- Smartport protocol. Minor improvements

[v0.3](https://github.com/dgatf/msrc/tree/v0.3)

- Esc current sensor (EscA) added (HW V4/V5, 80A or higher)
- Averaging telemetry added
- Voltage2 sensor changed from A3 to A4
- Ntc2 sensor changed from Tmp1 to Tmp2
- Averaging governor added
- Refresh rate and averaging added to lua config script

## Support

For questions, issues or new protocol request (use this [sketch](./sniffer/sniffer.ino)) please post in the forums:

[Helifreak](https://www.helifreak.com/showthread.php?t=835243)

[Openrcforums](https://www.openrcforums.com/forum/viewtopic.php?f=84&t=11911)

Or open an [Issue](https://github.com/dgatf/msrc/issues) in Github
