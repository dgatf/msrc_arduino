# ESC telemetry to FrSky Smartport

Send ESC telemetry to Frsky Smartport using an Arduino Pro Mini 168 or 328P (3.3v or 5v)

## ESC telemetry

The input is either ESC serial data or PWM signal

ESC serial protocols implemented:

- Hobbywing Platinum V3: RPM
- Hobbywing Platinum V4, Hobbywing Flyfun V5: RPM, temperature and battery voltage
- PWM signal: RPM

## PWM output

PWM signal generation from ESC serial (for HW V5 which does not have PWM output) for FLB systems

PWM signal properties: logic level 3.3V and duty cycle 17%

## Analog sensors (optional)

### Voltage divider circuit

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

## Thermistors

Two temperature sensors can be installed through the analog pins A0 and A1
Temperature is measured with NTC thermistors (100k). Adjust thermistor Beta in esc_smartport.h if needed (NTC_BETA, default is 4190). Sensor output in Celsius

## Current

Current is measured through analog pin A4

## OpenTx sensors (sensor Id 10)

- RPM: EscR (0x0b60)
- ESC voltage: EscV (0x0b50)
- ESC temp1: EscT (0x0b70)
- ESC temp2: EscT (0x0b71)
- Voltage 1: A3 (0x0900)
- Voltage 2: A4 (0x0910)
- Thermistor 1: Tmp1 (0x0400)
- Thermistor 2 : Tmp2 (0x0410)
- Current: Curr (0x020f)

## Minumum wiring:

 - SmartPort Vcc to Arduino RAW
 - SmartPort Gnd to Arduino Gnd
 - Smartport Signal to Arduino PIN_SMARTPORT_RX (7)
 - Smartport Signal to R3 (4.7k)
 - R3 (4.7k) to Arduino PIN_SMARTPORT_TX (12)
 - If using ESC serial: ESC serial signal to Arduino Rx
 - If using ESC PWM: ESC PWM signal to Arduino PIN_PWM_ESC (8)

<p align="center"><img src="./images/esc_smartport_min.png" width="600"><br>
  *Minimum circuit*<br><br>
  <img src="./images/esc_smartport_full.png" width="600"><br>
  *Full circuit*<br><br>
<img src="./images/top.jpg" width="400"><br>
<img src="./images/bottom.jpg" width="400"></p>

## Adjust RPM sensor (EscR)

- Blades/poles: number of pair of poles * main gear teeth  
- Multiplier: pinion gear teeth

## Adjust voltage sensors (A3)

Measure the voltage of the battery with a voltmeter and adjust *Ratio* in A3 sensor

## Adjust current sensor (Curr)

Adjust sensor ratio: *1000 / output sensitivity (mV/A)*

To get battery consumption add a new sensor:

- Type: Calculated
- Formula: Consumption
- Sensor: Curr

## Configuration

The configuration is modified with a lua script (X7, X9, X-lite and Horus with openTx 2.2 or higher)

<p align="center"><img src="./images/escSp.bmp" width="300"></p>

Options:

- ESC protocol. HobbyWing Platinum V3, HobbyWing Platinum V4/Hobbywing Flyfun V5 or PWM signal
- Voltage1. For voltage divider 1
- Voltage2. For voltage divider 2
- Ntc1. Thermistor 1
- Ntc2. Thermistor 2
- Current
- PWM out. To generate PWM output from ESC serial  (for obbywing Flyfun V5)

Copy the file escSp.lua to the SCRIPTS folder in the sdcard of the Tx and execute as one-time script from SD-HD-CARD screen (long press and Execute). It can be executed also as telemetry script if copied to TELEMETRY folder and assigned to a model telemetry screen

## Flash to Arduino

Using Arduino IDE copy folder *esc_smartport* and open *esc_smartport.ino*. Select board *Arduino Pro or Pro Mini*, processor *ATMega168 or ATMega328P (3.3V 8MHz or 5V 16MHz)* and flash

## [Video](https://youtu.be/Mby2rlmAMlU)

Change log:

v0.3

- Averaging telemetry added
- Voltage2 sensor changed from A3 to A4
- Ntc2 sensor changed from Tmp1 to Tmp2
