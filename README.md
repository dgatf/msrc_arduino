# MSRC - Multi Sensor for RC - Smartport, Frsky D, XBUS, SRXL, IBUS, SBUS2, Multiplex Sensor Bus, Jeti Ex Bus

This is a DIY project to send sensors telemetry for a fraction of the weight and cost of the stock sensors. It adds support for several ESC telemetry protocols

Compatible RX protocols: Frsky Smartport, FrSky D, Spektrum XBUS, Spektrum SRXL V5, Flysky IBUS, Futaba SBUS2, Multiplex Sensor Bus, Jeti Ex Bus

Compatible ESCs: Hobbywing V3/V4/V5, Kontronik Kosmic/Kolibri/JivePro, Castle Link, APD F/HV/UHV, phase sensor/PWM signal

Compatible MCUs: ATmega328P, ATmega328PB, ATmega2560 and ATmega32U4 and Teensy LC/3.x

Implemented sensors:

- ESC
  - ESCs with serial telemetry (Hobbywing V3/V4/V5<sup>(1)</sup>, Kontronik<sup>(2)</sup>), APD F/HV/UHV
  - ESC with PWM signal or phase sensor
  - ESC Castle Link
- GPS serial (NMEA)
- I2C sensors: BMP280
- Analog sensors: voltage, temperature, current, air speed

All sensors are optional. Make the circuit with the desired sensors and enable them in the configuration, with a lua script if using smartport or in config.h for the rest of Rx protocols 

(1) HW V5 = HW FlyFun. If ESC doesn't arm, in config.h enable ESC_INIT_DELAY or connect arduino after the ESC is armed  
(2) Supported: Kosmic, Kolibri, JivePro. Not supported: Jive

## 1. Boards

### 1.1 MCU

| MCU | Recommended board | Castle telemetry | UART<sup>(1)</sup> | Inverted UART<sup>(2)</sup> |
| :---: | :---: | :---: | :---: | :---: |
| ATmega328P<sup>(3)</sup> | Arduino Pro Mini<sup>(5)</sup> | Innacurate | 1 | :x: |
| ATmega328PB<sup>(3)</sup> | Pololu ATmega328PB<sup>(5)</sup> | :heavy_check_mark: | 2 | :x: |
| ATmega2560 | ATmega2560 Pro Mini | :heavy_check_mark: | 4 | :x: |
| ATmega32U4<sup>(3)</sup> | Teensy 2 | :heavy_check_mark: | 1 | :x: |
| ARM CortexM0+ | Teensy LC<sup>(4)</sup> | :heavy_check_mark: | 3 | :heavy_check_mark: |
| ARM CortexM4 | Teensy 3.2 | :heavy_check_mark: | 3 | :heavy_check_mark: |

(1) 2 x UARTS allows to use ESC serial and GPS at the same time  
(2) Allows to use hardware serial for Smartport and SBUS without signal inverter. Also is half duplex capable  
(3) Arduino boards with 32kb or less and smartport, only the ESC driver defined in config.h is flashed. It can't be changed later with lua script  
(4) Teensy LC is not 5v tolerant. If using with Castle is preferred Teensy 3.2, which is 5v tolerant  
(5) Arduino Pro Mini and Pololu ATmega328PB requires a USB-TTL programmer for flashing

ATmega boards at 5v (16Mhz) may not read properly serial port when using ESC serial or GPS as they use 3.3v logic level and Arduino 5v. In this case if using 5v it is better without USB (Pro Mini) and/or use software serial. If using ATmega board, is preferred 3.3v (8Mhz) boards

### 1.2 Connections

| Board | Arduino Pro Mini | Pololu ATmega328PB | Arduino ATmega2560 | Teensy 2 | Teensy LC<br/>Teensy 3.x |
| :---: | :---: | :---: | :---: | :---: | :---: |
| MCU | ATmega328P | ATmega328PB | ATmega2560 | ATmega32U4 | ARMCortexM0+<br/>ARMCortexM4 |
| NTC 1 | A0 | A0 | A0 | F6(A1) | 14 |
| NTC 2 | A1 | A1 | A1 | F7(A0) | 15 |
| Voltage 1 | A2 | A2 | A2 | F5(A3) | 16 |
| Voltage 2 | A3 | A3 | A3 | F4(A2) | 17 |
| Current | A6 | A6 | A6 | B5(A9) | 20 |
| Airspeed | A7 | A7 | A7 | D7(A7) | 26 |
| ESC serial | RX | RX0 | 19(RX1) | D2(RX1) | 9 |
| GPS | RX | RX1 | 17(RX2) | D2(RX1) | 7 |
| PWM in | 8 | 8 | 49 | D4(A6) | 6 |
| PWM out | 10 | 10 | 7 | B6(A10) | 22 |
| Rx Castle | 8 | 8 | 49 | C7 | 16 |
| ESC Castle<sup>(1)</sup> | 2 & 10 | 2 & 22 | 45 & 48 | C8 & B6 | 2 & 6 |
| Frsky Smartport, SBUS | 7  & 12<sup>(2)</sup> | 7  & 12<sup>(2)</sup> | A15  & D10<sup>(2)</sup> | B3  & B4<sup>(2)</sup><br/>(15 & 16) | 1 |
| Frsky D | 12 | 12 | D10 | B4(16) | 1 |
| SRXL, IBUS, SB, Jeti Ex | RX & TX<sup>(2)</sup> </br>or<sup>(8)</sup> 7 & 12<sup>(2)</sup> | RX1 & TX1<sup>(2)</sup> </br>or<sup>(6)</sup> RX0 & TX0<sup>(2)</sup> </br>or<sup>(7)</sup> 7 & 12<sup>(2)</sup> | 15(RX3) & 14(TX3)<sup>(2)</sup> | D2(RX1) & D3(TX1)<sup>(2)</sup> </br>or<sup>(8)</sup> B3 & B4<sup>(2)</sup>(15 & 16) | 1 |
| XBUS or sensor SDA | A4 | A4 | 20 | D1(2) | 18<sup>(3)</sup> |
| XBUS or sensor SCL | A5 | A5 | 21 | D0(3) | 19<sup>(3)</sup> |
| XBUS NPN clock stretch<sup>(4)</sup> | 13 | 13 | 13 | PB5(9) | 13 |

(1) with 3.3k pull up resistor  
(2) diode 1N4148. Note the inverted orientation of the diode for Frsky and SBUS. Alternatively 1k to 3.3k resistor can be used  
(3) If using i2c_t3 library connect I2C sensor to pins 23 (SDA1) and 22 (SCL1)  
(4) optional. Only if needed. If connected, enable XBUS_CLOCK_STRECH_SWITCH in config.h  
(5) If using serial ESC  
(6) If using GPS  
(7) If using serial ESC and GPS. If using a diode, the diode line towards the pin  
(8) If using serial ESC or GPS. If using a diode, the diode line towards the pin  


## 2. Receiver protocol

The following Rx protocols are supported:

- Frsky Smartport : inverted serial, 57600 bps
- Frsky D : inverted serial, 9200 bps
- Spektrum SRXL V5: serial, 115200 bps
- Spektrum XBUS: I2C
- Flysky IBUS: serial, 115200 bps
- Futaba SBUS2: inverted serial, 100000 bps
- Multiplex Sensor Bus: serial, 38400 bps
- Jeti Ex Bus: serial 125000, 250000 bps

Depending on the receiver protocol connect to the Rx as follows

### ATmega boards

1N4148 diode or 1k to 3.3k resistor

<p align="center"><img src="./images/frsky_smartport_sbus.png" width="400"><br>
<i>Frsky Smartport, SBUS</br>or SRXL, IBUS, SB, Jeti Ex Bus and boards with no available UARTS, as being used for ESC serial or GPS (if using a diode, then invert orientation) </i><br><br></p>

<p align="center"><img src="./images/srxl_ibus_sb.png" width="460"><br>
<i>SRXL, IBUS, SB, Jeti Ex Bus</i><br><br></p>

<p align="center"><img src="./images/frsky_d.png" width="310"><br>
<i>Frsky D</i><br><br></p>

### ARM Cortex boards 

<p align="center"><img src="./images/frsk_smartport_srxl_ibus_sbus_teensy.png" width="350"><br>

### SBUS 2

Sensor mapping to slots for Futaba radios:

|Slot |Sensor |
| :---: | :---: |
|1	  | RPM |
|2-3  |	Voltage (voltage 1, voltage 2) |
|4-5  | Vario (speed, altitude) |
|6	  | Temperature 1 |
|7	  | Temperature 2 |
|8-10 |	Power 1 (current, voltage, consumption) |
|11-13|	Power 2 (current, voltage, consumption)	|
|16-23|	GPS (speed, altitude, time, vario, lat1, lat2, lon1, lon2) |

### XBUS

Auto-config may be used to detect the new sensors.

If using Teensy LC/3.1/3.5/3.6, it is feasible to use i2c_t3 library by enabling I2C_T3_TEENSY in config.h. This allows to use a second I2C port to use I2C sensors together with XBUS. Then I2C sensor to be connected to pins 23 (SDA1) and 22 (SCL1)

<p align="center"><img src="./images/xbus.png" width="300"><br>
  <i>XBUS</i><br><br></p>

If no telemetry is shown, may be MSRC is booting too slow and the first poll from the receiver is not answered. There are several ways to fix this:

1. Power on the receiver after MSRC has started
2. Flash MSRC without bootloader
3. I2C clock stretch. Pull down the SCL line until MSRC has started, then open the switch. You can use a manual swith or a NPN transistor (e.g. PN2222ABU). If using a transistor you need to enable XBUS_CLOCK_STRECH_SWITCH in config.h to open the switch after boot. If using manual switch, open the switch after boot, to finish the clock stretch

<p align="center"><img src="./images/xbus-switch.png" width="300"><br>
  <i>Clock stretch XBUS with manual switch</i><br><br></p>

<p align="center"><img src="./images/xbus-npn-switch.png" width="300"><br>
  <i>Clock stretch XBUS with transistor</i><br><br></p>

### Jeti Ex Bus

Configure receiver pin (E1 or E2) as Ex Bus

## 3. Sensors

All sensors are available with Smartport protocol

Other protocols may not have all of the sensors:

- Frsky D. Not available second voltage

### 3.1. ESC

#### Serial telemetry

Compatible ESC serial protocols:

 - Hobbywing V3/V4/V5. Serial 19200
 - Kontronik Kosmic/Kolibri/JivePro. Serial 115200, even parity
 - APD F/HV/UHV. Serial 115200

Optionally, for Hobbywing Flyfun (V5) and APD F-series, a PWM signal can be generated from the RPM telemetry value

<p align="center"><img src="./images/serial.png" width="360"><br>
  <i>ESC serial</i><br><br></p>

<ins>Hobbywing Platinum V3</ins>
 
 Telemetry port is the program port. It is a servo male plug on the ESC

<ins>Hobbywing Platinum V4 / FlyFun V5</ins>

- The telemetry port is a wired servo female plug. Don't confuse with RPM PWM signal connector or the program port
- Check firmware installed is NOT VBAR version
- Values for current when throttle is low (up to 25%, depending on model) may not be reliable. If getting high or noisy current values when throttle is low, adjust CURRENT_THRESHOLD in config.h. Below this throttle percentage (0-100%), current values will be 0A
- Set ESCHW4_CURRENT_MAX to the peak current of the ESC (eg: 80A: ESCHW4_CURRENT_MAX 100)
- Adjust ESCHW4_DIVISOR and ESCHW4_AMPGAIN in escHW4.h, depending on model:
 
| Cells range | ESCHW4_DIVISOR |
| :---: | :---: |
| 3-6S (LV) | 11 |
| 3-8S (LV v2) | 15.75 |
| 5-12s (HV) | 21 |
| 6-14s (HV v2) | unknown |

| Amperage | ESCHW4_AMPGAIN |
| :---: | :---: |
| FlyFun 60A | 6
| FlyFun 80A | 12.4 |
| 100A | 9<sup>(1)</sup> |
| Platinum V4 120A | 10 |
| 130A | 11.3<sup>(1)</sup> |
| 150A | 12.9<sup>(1)</sup> |
| 160A | 13.7<sup>(1)</sup> |
| Platinum V4 200A | 16.9 |

(1) Extrapolated from confirmed models. If you find discrepancy adjust gain parameter and send gain and ESC model to update the table

#### PWM signal

If the ESC have a PWM signal or a phase sensor is installed, RPMs can be measured. If ESC have both serial and PWM signal, like Hobbywing V4/V5, then PWM signal is not needed for telemetry

<p align="center"><img src="./images/pwm_in.png" width="350"><br>
  <i>PWM signal/phase sensor circuit</i><br><br></p>

#### Castle link

The telemetry is send over the ESC signal. *Castle Link* has to be enabled in the ESC config

If using ATmega328P telemetry values are not accurate all the time. Some readings are increased by 5-10%. This is a hardware limitation

<p align="center"><img src="./images/castle_teensy2.png" width="420"><br>
  <i>Castle Link with Teensy 2</i><br><br></p>

<p align="center"><img src="./images/castle_teensy3.png" width="500"><br>
<i>Castle Link with Teensy LC/3.x</i><br><br></p>

#### Available ESC telemetry

| Model              | RPM         | Throttle    | Motor PWM   | Voltage     | Current   | Voltage BEC | Current BEC | Temperature 1 | Temperature 2 | Ripple Volt |
| ------------------ | :---------: | :---------: | :---------: | :---------: | :-------: | :---------: | :---------: | :-----------: | :-----------: | :---------: |
| Hobbywing V3       | :heavy_check_mark:         | :heavy_check_mark:<sup>(1)</sup>      | :heavy_check_mark:<sup>(1)</sup>      |             |           |             |             |               |               |             |
| Hobbywing V4/V5<sup>(4)</sup><sup>(5)</sup> | :heavy_check_mark:         | :heavy_check_mark:<sup>(1)</sup>      | :heavy_check_mark:<sup>(1)</sup>      | :heavy_check_mark:         | :heavy_check_mark:<sup>(2)</sup>    |             |             | :heavy_check_mark: FET       | :heavy_check_mark: BEC       |             |
| Castle Link        | :heavy_check_mark:         | :heavy_check_mark:<sup>(1)</sup>      | :heavy_check_mark:<sup>(1)</sup>      | :heavy_check_mark:         | :heavy_check_mark:       | :heavy_check_mark:<sup>(3)</sup>      | :heavy_check_mark:<sup>(3)</sup>      | :heavy_check_mark:           |               | :heavy_check_mark:         |
| Kontronik        | :heavy_check_mark:         | :heavy_check_mark:<sup>(1)</sup>      | :heavy_check_mark:<sup>(1)</sup>      | :heavy_check_mark:         | :heavy_check_mark:       | :heavy_check_mark:      | :heavy_check_mark: | :heavy_check_mark: Power amp | :heavy_check_mark: BEC     |         |
| APD F | :heavy_check_mark:         | :heavy_check_mark:<sup>(1)</sup>      | :heavy_check_mark:<sup>(1)</sup>      | :heavy_check_mark:         | :heavy_check_mark:    |             |             | :heavy_check_mark:       |        |             |
| APD HV/UHV | :heavy_check_mark:         | :heavy_check_mark:<sup>(1)</sup>      | :heavy_check_mark:<sup>(1)</sup>      | :heavy_check_mark:         | :heavy_check_mark:    |             |             | :heavy_check_mark:      |       |             |


(1) Available but not forwarded to smartport  
(2) For 80A models and higher  
(3) Not available in all models  
(4) Sensors varies depending on model and firmware. See [annex](#Hobbywing-V4-V5)  
(5) Upgrade to latest firmware version. But note VBAR versions of firmware are not compatible. Install a non VBAR version of the ESC firmware

If voltage is available the cell voltage average is calculated for 3S,4S,5S,6S,7S,8S,10S and 12S batteries. 10 seconds after power on the number of cells is autodetected. Average cell voltage to be >3.8v for correct cell count

If current is available, battery consumption is calculated in mAh

### RPM multipliers

Adjust RPMs in *config.h*:

- Set the number of pair of poles of the motor, RPM_PAIR_OF_POLES
- For helis also set the pinion and main gear teeth: RPM_PINION_TEETH, RPM_MAIN_TEETH

Alternatively this can be done in the transmitter

### 3.2. Serial GPS

Serial GPS (NMEA protocol) is supported

### 3.3. Analog sensors

The following analog sensors are supported:

- 2 x voltage divider. Calibrate voltage analog sensors with VOLTAGE1_MULTIPLIER and VOLTAGE2_MULTIPLIER in config.h. Or from opentx, but it is recommended by code to increase sensor resolution
- 2 x temperature sensors (thermistors)
- Current sensor (Hall effect: ACS758). Calibrate current analog sensor with CURRENT_MULTIPLIER in config.h. Or from opentx, but it is recommended by code to increase sensor resolution
- Airspeed sensor (MPXV7002)

### 3.4. I2C sensors

The following I2C sensors are suported:

- Barometer: BMP280

Remarks:

- For BMP280 module there are two versions: 3.3V (6 pins, I2C/SPI) and 5V (4 pins, I2C). Note that  3.3V version is not 5V tolerant. Use module version accordingly to your board Vcc
- If using XBUS, use Teensy LC/3.x to be able to use I2C sensors

<p align="center"><img src="./images/full.png" width="600"><br>
  <i>I2C and analog sensors with Smartport</i><br><br></p>

## 4. Configuration

### From PC - MSRC gui

Use MSRC to generate _config.h_. Copy and paste the text to config.h and then flash

<p align="center"><img src="./images/msrc_gui.png" width="600"><br>

Find the Binaries [here](https://github.com/dgatf/msrc/tree/master/msrc_gui/binaries)

Or build msrc_gui:

- Install QT
- cd to msrc_gui
- _qmake_
- _make_

### From PC - config.h

Manually change _config.h_

### From transmitter (only Smartport)

The configuration is modified with a lua script (X7, X9, X-lite and Horus with openTx 2.2 or higher)

<p align="center"><img src="./images/lua_x7.png" height="128">   <img src="./images/lua_x9.png" height="128">   <img src="./images/lua_x10.png" height="200"></p>

Copy the file msrc.lua to the SCRIPTS/TOOLS folder. (if using older openTx 2.2 copy to SCRIPTS folder and execute by long press)

If not using lua script comment *#define CONFIG_LUA* and assign config values in msrc.h

Options:

- ESC protocol. HobbyWing Platinum V3, HobbyWing Platinum V4/Hobbywing Flyfun V5 or PWM signal
- Voltage1. Enable/disable analog voltage divider 1
- Voltage2. Enable/disable analog voltage divider 2
- Ntc1. Enable/disable analog thermistor 1
- Ntc2. Enable/disable analog thermistor 2
- Current. Enable/disable analog current
- Airspeed. Enable/disable analog airspeed sensor
- PWM out. Enable/disable analog a PWM signal from RPM values from ESC serial
- GPS.  Enable/disable serial GPS
- Averaging queue size: 1 to 16
- Refresh rate (ms): 0 to 1600
- I2C (x2). Sensor type and address


## 5. Flash to Arduino

Using Arduino IDE copy folder *msrc* and open *msrc.ino*. Select the board and flash

## 6. OpenTx sensors (Smartport)

The default sensor id is 10. This can be changed with [change_id_frsky](https://github.com/dgatf/change_id_frsky)

Depending on your configuration some the following sensors will be available in Opentx. After configuration go to sensors screen and update with *Search new sensors*
 
ESC:

- ESC RPM: Erpm (0x0b60)
- ESC voltage: EscV (0x0b50)
- ESC cell average: VFAS (0x0210)
- ESC current: EscA (0x0b50)
- ESC consumption: EscC (0x0b60)
- ESC temp FET (HW) or ESC temp linear (Castle): EscT (0x0b70)
- ESC temp BEC (HW) or ESC temp NTC (Castle): EscT (0x0b71)
- ESC ripple voltage: EscV (0x0b51)
- ESC BEC voltage: BecV (0x0e50)
- ESC BEC current: BecC (0x0e50)

Only HW V4: Sensors 5100, 5101 and 5102 contains the ESC signature and raw current. To disable, comment ESC_SIGNATURE in config.h

GPS:

- GPS Lat/Lon: GPS (0x800)
- GPS altitude: GAlt (0x820)
- GPS speed: GSpd (0x820)
- GPS compass: Hdg (0x840)
- GPS date/time: Date (0x850)

Analog:

- Voltage 1: A3 (0x0900)
- Voltage 2: A4 (0x0910)
- Thermistor 1: Tmp1 (0x0400)
- Thermistor 2: Tmp2 (0x0410)
- Current: Curr (0x020f)
- AirSpeed: ASpd (0x0a00)

I2C:

- Altitude: Alt (0x0820)
- Temperature: T1 (0x0401, 0x0402)

Some of the sensors have to be adusted in openTx

### 6.1. Adjust RPM sensor (Erpm)

- Blades/poles: number of pair of poles * main gear teeth  
- Multiplier: pinion gear teeth

### 6.2. Adjust voltage sensors (A3, A4)

Remark: Instead of adjusting the sensor in opentx, it is recommended to use VOLTAGE1_MULTIPLIER and VOLTAGE2_MULTIPLIER in config.h to increase the sensor resolution. Though voltage resolution in opentx is 10 times the current resolution, so less critical

Measure the voltage of the battery with a voltmeter and adjust *Ratio* in A3, A4 sensor

### 6.3. Adjust analog current sensor (Curr)

Remark: Instead of adjusting the sensor in opentx, it is recommended to use CURRENT_MULTIPLIER in config.h to increase the sensor resolution

If using a hall effect sensor, adjust the ratio: *25.5 x 1000 / output sensitivity (mV/A)*

To calculate the battery consumption add a new sensor:

- Type: Calculated
- Formula: Consumption
- Sensor: Curr

### 6.4 Calculate current consumption

Battery consumption is calculatd since MSRC v0.9

Alternatively can be calculated by adding a calculated sensor in openTx:

- Type: _Calculated_
- Formula: _Consumption_
- Sensor: _EscA_ or _Curr_

## 7. Images

<p align="center"><img src="./images/top.jpg" width="300">  <img src="./images/bottom.jpg" width="300"></p>

<p align="center"><img src="./images/450_1.jpg" width="300">  <img src="./images/450_2.jpg" width="300"></p>

<p align="center"><img src="./images/450_3.jpg" width="300">  <img src="./images/450_x7.bmp" width="300"><br><i>MSRC on Align 450 connected to Hobbywing V3 Platinum and two thermistors for ESC and motor</i><br></p>


## 8. Video

[Video](https://youtu.be/Mby2rlmAMlU)


## 9. Annex

### 9.1. ESC protocol specifications Hobbywing

Serial parameters:

- 19200 bps
- 1 packet every 20 ms
- Big endian


#### Hobbywing V3

| Byte  | 1                   | 2                | 3                | 4                | 5             | 6              | 7            | 8            | 9           | 10          |
| ----- | :-----------------: | :--------------: | :--------------: | :--------------: | :-----------: | :------------: | :----------: | :----------: | :---------: | :---------: |
| Value | Package Head (0x9B) | Package Number 1 | Package Number 2 | Package Number 3 | Rx Throttle 1 | Rx Throttle  2 | Output PWM 1 | Output PWM 2 | RPM Cycle 1 | RPM Cycle 2 |

*RPM = 60000000 / RPM Cycle*

thr, pwm: 0-255 (8bits)

#### Hobbywing V4 V5

| Byte  | 1     | 2     | 3     | 4     | 5     | 6     | 7     | 8     | 9     | 10    | 11    | 12    | 13    | 14    | 15    | 16    | 17    | 18    | 19    |
| ---   | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| Value | Package Head (0x9B) | Package Number 1 |	Package Number 2 | Package Number 3	| Rx Throttle 1	| Rx Throttle  2 | Output PWM 1 | Output PWM 2	| RPM 1 | RPM 2	| RPM 3	| Voltage 1 |	Voltage 2	| Current 1	| Current 2	| TempFET 1	| TempFET 2	| Temp 1 |	Temp 2

thr, pwm: 0-1024 (10bits)

Voltage, current and temperature are raw sensor data. Actual values requires transformation. Depending on the model, sensors are different

Before throttle is raised from 0, signature packets are sent between telemetry packets. This is used to identify the hardware and firmware of the ESC

Examples:

| Model\Byte    | 1     | 2     | 3     | 4     | 5     | 6     | 7     | 8     | 9     | 10    | 11    | 12    | 13    |
| ----------    | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| V4LV25/60/80A | 0x9B  | 0x9B  | 0x03  | 0xE8  | 0x01  | 0x08  | 0x5B  | 0x00  | 0x01  | 0x00  | 0x21  | 0x21  | 0xB9  |
| V4HV200A OPTO | 0x9B  | 0x9B  | 0x03  | 0xE8  | 0x01  | 0x02  | 0x0D  | 0x0A  | 0x3D  | 0x05  | 0x1E  | 0x21  | 0xB9  |
| V5HV130A OPTO | 0x9B  | 0x9B  | 0x03  | 0xE8  | 0x01  | 0x0B  | 0x41  | 0x21  | 0x44  | 0xB9  | 0x21  | 0x21  | 0xB9  |


### 9.2. ESC protocol specifications Castle Link

For best accuracy RX pulse input is captured with a timer interrupt and ESC pulse output is produced by hardware PWM. Maximum latency is 20ms

See [Castle Link Live](https://dzf8vqv24eqhg.cloudfront.net/userfiles/4671/6540/ckfinder/files/Product%20Manuals/Accessories%20and%20replacement%20parts/castle_link_live_2_0.pdf?dc=201606221536-537)

### 9.2. ESC protocol specifications Kontronik

Supported models: Kosmic, Kolibri, JivePro. Protocol [specs](https://www.kontronik.com/fileadmin/kontronik-sobek/Public/Content/Images/Content/Downloads/Software/Kontronik_TelMe_V4.12_1.12_EN.pdf)

Note that there is another protocol used by Kontronik Jive at 9600 that it is not supported. Info [here](https://www.helifreak.com/showthread.php?t=721180)

### 9.3. Output PWM signal for FBL

For best accuracy PWM signal output for FBL is produced by hardware PWM from serial RPM values. Maximum latency is 40ms

### 9.4. ADC voltage

To obtain the voltage at the analog pin it is required the ADC bits (b) and the Vref:

<img src="https://latex.codecogs.com/svg.latex?V_o=V_{ref}*\frac{Raw}{2^b}" title="Vo = Vref * Raw / 2^bits" /><br>

### 9.5. Analog voltage sensors. Voltage divider circuit

Two battery voltages can be measured through the analog pins A2 and A3
Metal resistors are recommended as gives more accurate readings (0.1W or higher)
Arduino can read up to 3.3V/5V and is optimized for signal inputs with 10K impedance

<p align="center"><img src="./images/Resistive_divider.png" width="200"></p>

To select R values apply formulas:

<img src="https://latex.codecogs.com/svg.latex?V_o=V_{in}\frac{R_2}{R_1+R_2}<3.3Vor5V" title="Vo = Vi\*R2/(R1+R2) < 3.3V or 5V" /><br>

<img src="https://latex.codecogs.com/svg.latex?Z=\frac{1}{\frac{1}{R_1}+\frac{1}{R_2}}<10K" title="Z = 1/((1/R1)+(1/R2)) < 10K" />


For 6S battery (or lower) and Pro Mini 3.3v:

 - R1 68k
 - R2 10k

If more than 6S change R values or you may burn the Arduino!

### 9.6. Temperature sensors. Thermistors

Two temperature sensors can be installed through the analog pins A0 and A1
Temperature is measured with NTC thermistors (100k). Adjust thermistor Beta in ntc.h if needed (NTC_BETA, default is 4190). Sensor output in Celsius

<p align="center"><img src="./images/ntc.gif" width="200"></p>

To obtain the thermistor resistance:

<img src="https://latex.codecogs.com/svg.latex?R_t=\frac{V_o*R_s}{(V_{in}-V_o)}" title="Rt = Vo * Rs / (Vin - voltage))}" />

And temperature with Beta formula:

<img src="https://latex.codecogs.com/svg.latex?T=\frac{1}{\frac{ln\frac{R_t}{R_{ref}}}{\beta}+\frac{1}{T_{ref}}}" title="T = 1/[ln(Rt/Rref)/B+1/Tref]" />

Or with Steinhart and Hart Equation if data is available:

<img src="https://latex.codecogs.com/svg.latex?T=\frac{1}{A+B*ln\frac{R_t}{R_{ref}}+C*ln(\frac{R_t}{R_{ref}})^2+D*ln(\frac{R_t}{R_{ref}})^3}" title="T = 1/[A+Bln(Rt/Rref)+Cln(Rt/Rref)²+Dln(Rt/Rref)³]" />

### 9.7. Current

#### Hall effect

Hall effect sensors. Induced magnetic field is transformed into voltage. They are caracterized by their sensitivity

<img src="https://latex.codecogs.com/svg.latex?I=\frac{V_o}{S}" title="I=Vo/S" />

#### Shunt resistor

The voltage drop in the shunt resistor is amplified by a differential amplifier to obtain Vo

<img src="https://latex.codecogs.com/svg.latex?I=\frac{V_o}{A_d*R_s}" title="I=Vo/Ad*Rs" />

<p align="center"><img src="./images/High-Side-Current-Sensing.png" width="200"></p>


### 9.8. Air Speed

Air speed is measured with a differential pressure sensor 

Presssure is calculated with the sensor's transfer formula:

<img src="https://latex.codecogs.com/svg.latex?V_o=V_{cc}*(B*P+A)" title="Vo=Vcc(MP+A)" />


And the air speed using the Bernouilli formula:

<img src="https://latex.codecogs.com/svg.latex?V=\sqrt{2*P/\rho}" title="V=(2P/d)^1/2" />

After 2 seconds the sensor is calibrated to zero

Adjust constants in *pressure.h*:

- TRANSFER_SLOPE (B) as per sensor datasheet
- For fine tuning measure the Vcc on the sensor and adjust TRANSFER_VCC

## 10. Change log

v0.9

- HW V4/V5. Added throttle threshold and maximum current filters for current sensor. Values for current when throttle is low may not be reliable, depending on model
- Added consumption calculation. Thanks MJ666
- Fixed Jeti Ex bug
- HW V4/V5. Only one HW V4/V5 protocol. Voltage and current sensor parameters to be set manually
- Added RPM multipliers. Motor poles and gears (for helis) 
- Changed serial drivers timeout to microseconds for fast protocols (e.g. Jeti Ex)
- Allow additional Rx protocols & boards to be used with serial ESC and GPS. Use software serial only for Rx protocols, not sensors
- GPS. Added HDOP, sats and vario
- IBUS. Fixed S32 type bug (GPS values)
- XBUS. Added analog voltage2/ntc2 sensor
- BMP280. Added vertical speed calculation (vario)
- Current analog. Added consumption
- Added ESC support for APD F/HV/UHV

[v0.8](https://github.com/dgatf/msrc/tree/v0.8)

- Added specific drivers for hardware serial and software serial
- Added msrc_gui to create config.h (not all options are avaiable from gui)
- Added Rx protocols: Frsky D, Spektrum XBUS, Spektrum SRXL V5, Flysky Ibus, Futaba SBUS2, Multiplex Sensor Bus, Jeti Ex Bus
- Added support for ATmega328PB, ATmega2560, ATmega32U4, ARM Cortex M0+ and ARM Cortex M4
- Improved accuracy for PWM input (rpm) measurement
- Added ESCs support for: Kontronik Kosmic/Kolibri/JivePro
- Improved current calculation for HW V4/V5

[v0.7](https://github.com/dgatf/msrc/tree/v0.7)

- Added analog airspeed sensor (MPXV7002)
- Fixed Castle Link bug
- Removed BMP180
- Fixed flickering in color lcd displays

[v0.6](https://github.com/dgatf/msrc/tree/v0.6)

- Added GPS serial (BN220, BN880, NEO-M8N,...)

[v0.5](https://github.com/dgatf/msrc/tree/v0.5)

- Added Castle Link Live protocol
- Hobbywing V4/V5. Improved transformations for voltage and current depending on the model (thanks to Commodore8888)

[v0.4.1](https://github.com/dgatf/msrc/tree/v0.4.1)

- Fix Horus display

[v0.4](https://github.com/dgatf/msrc/tree/v0.4)

- Changed R3 resistor to 3.3k
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


## 11. Support

For questions, issues or new protocol request (use this [sketch](./sniffer/sniffer.ino)) please post in the forums:

[Helifreak](https://www.helifreak.com/showthread.php?t=835243)

[Openrcforums](https://www.openrcforums.com/forum/viewtopic.php?f=84&t=11911)

Or open an [Issue](https://github.com/dgatf/msrc/issues) in Github


## 12. Acknowledgements

- Commodore8888 (Helifreak)
- MikeJ (Helifreak)