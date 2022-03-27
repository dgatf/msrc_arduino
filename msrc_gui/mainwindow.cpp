#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "circuitdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->cbBoard->addItems({"Arduino Pro Mini", "Arduino Pro Micro", "Arduino Micro", /*"Pololu ATmega328PB", "ATmega2560 Pro Mini",*/ "Teensy 2", "Teensy LC/3.x"});
    ui->cbEsc->addItems({"Hobbywing V3", "Hobbywing V4", "PWM", "Castle link", "Kontronic", "APD F", "APD HV"});
    ui->cbGpsBaudrate->addItems({"115200", "57600", "38400", "19200", "14400", "9600","4800"});
    ui->cbGpsBaudrate->setCurrentIndex(5);
    ui->cbReceiver->addItems({"Frsky Smartport", "Frsky D", "Spektrum XBUS", "Spektrum SRXL", "Flysky IBUS", "Futaba SBUS2", "Multiplex Sensor Bus", "Jeti Ex Bus", "Hitec"});
    ui->cbEscModel->addItems({"Platinum PRO v4 25/40/60", "Platinum PRO v4 80A", "Platinum PRO v4 100A", "Platinum PRO v4 120A", "Platinum PRO v4 130A-HV", "Platinum PRO v4 150A", "Platinum PRO v4 200A-HV",
                              "FlyFun 30/40A", "FlyFun 60A", "FlyFun 80A", "FlyFun 120A", "FlyFun 110A-HV", "FlyFun 130A-HV",  "FlyFun 160A-HV"});
    ui->cbCurrentSensorType->addItems({"Hall effect", "Open loop hall effect"});
    ui->cbBarometerType->addItems({"BMP280", "MS5611"});
    ui->cbAltitudeFilter->addItems({"Low", "Medium", "High"});
    ui->cbAltitudeFilter->setCurrentIndex(2);
    ui->cbSpeedUnitsGps->addItems({"km/h", "kts"});
    for (uint8_t i = 0; i < 127; i++) {
        QString hex;
        hex.setNum(i,16);
        ui->cbAddress->addItem("0x" + hex);
    }
    ui->cbAddress->setCurrentIndex(0x77);

    connect(ui->btGenerate, SIGNAL (released()),this, SLOT (generateConfig()));
    connect(ui->actionGenerate_config, SIGNAL(triggered()), this, SLOT(generateConfig()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exitApp()));

    ui->lbCircuit->resize(621, 400); //(ui->lbCircuit->parentWidget()->width(), ui->lbCircuit->parentWidget()->height());
    generateCircuit(ui->lbCircuit);
}

void MainWindow::generateCircuit(QLabel *label)
{
    QSize *size = new QSize(label->width(), label->height());
    QPixmap *pix = new QPixmap(*size);
    QPainter *paint = new QPainter(pix);
    QImage image;

    image.load(":/res/background.png");
    paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));

    if (ui->cbBoard->currentText() == "Arduino Pro Mini")
        image.load(":/res/mini.png");
    else if (ui->cbBoard->currentText() == "Arduino Pro Micro")
        image.load(":/res/pro_micro.png");
    else if (ui->cbBoard->currentText() == "Arduino Micro")
        image.load(":/res/micro.png");
    else if (ui->cbBoard->currentText() == "Teensy 2")
        image.load(":/res/teensy2.png");
    paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));

    if (ui->gbCurrent->isChecked())
    {
        image.load(":/res/batt_esc.png");
        paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
    }

    if (ui->gbVoltage1->isChecked())
    {
        image.load(":/res/voltage1.png");
        paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
    }

    if (ui->gbVoltage2->isChecked())
    {
        image.load(":/res/voltage2.png");
        paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
    }

    if (ui->cbBoard->currentText() == "Arduino Pro Mini")
    {
        if (ui->gbGps->isChecked())
        {
            image.load(":/res/bn220.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbTemperature1->isChecked())
        {
            image.load(":/res/ntc1.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbTemperature2->isChecked())
        {
            image.load(":/res/ntc2.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbEsc->isChecked())
        {
            if (ui->cbEsc->currentText() == "Hobbywing V3" || ui->cbEsc->currentText() == "Hobbywing V4" || ui->cbEsc->currentText() == "Kontronic" || ui->cbEsc->currentText() == "APD F" || ui->cbEsc->currentText() == "APD HV")
                image.load(":/res/esc_serial.png");
            if (ui->cbEsc->currentText() == "PWM")
                image.load(":/res/esc_pwm.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        image.load(":/res/receiver_3v_hs.png");
        if (ui->gbEsc->isChecked() &&
             (ui->cbEsc->currentText() == "Hobbywing V3" ||
              ui->cbEsc->currentText() == "Hobbywing V4" ||
              ui->cbEsc->currentText() == "Kontronic"  ||
              ui->cbEsc->currentText() == "APD F" ||
              ui->cbEsc->currentText() == "APD HV"))
        {
            image.load(":/res/receiver_3v_ss_mini.png");
        }
        if (ui->gbGps->isChecked() || ui->cbReceiver->currentText() == "Frsky Smartport" || ui->cbReceiver->currentText() == "Futaba SBUS2")
        {
            image.load(":/res/receiver_3v_ss_mini.png");
        }
        if (ui->cbReceiver->currentText() == "Frsky D")
        {
            image.load(":/res/receiver_3v_ss_mini_frskyd.png");
        }
        if (ui->cbReceiver->currentText() == "Spektrum XBUS")
        {
            image.load(":/res/receiver_3v_i2c_xbus_mini.png");
        }
        if (ui->cbReceiver->currentText() == "Hitec")
        {
            image.load(":/res/receiver_3v_i2c_mini.png");
        }
        paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));

        if (ui->gbCurrent->isChecked())
        {
            image.load(":/res/acs758_mini.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbAirspeed->isChecked())
        {
            image.load(":/res/mpxv7002_mini.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbAltitude->isChecked() && ui->cbBarometerType->currentText() == "BMP280")
        {
            image.load(":/res/bmp280_mini.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbAltitude->isChecked() && ui->cbBarometerType->currentText() == "MS5611")
        {
            image.load(":/res/ms5611_mini.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }
    }

    else if (ui->cbBoard->currentText() == "Arduino Pro Micro")
    {
        if (ui->gbGps->isChecked())
        {
            image.load(":/res/bn220.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbTemperature1->isChecked())
        {
            image.load(":/res/ntc1.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbTemperature2->isChecked())
        {
            image.load(":/res/ntc2.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbEsc->isChecked())
        {
            if (ui->cbEsc->currentText() == "Hobbywing V3" || ui->cbEsc->currentText() == "Hobbywing V4" || ui->cbEsc->currentText() == "Kontronic"  || ui->cbEsc->currentText() == "APD F" || ui->cbEsc->currentText() == "APD HV")
                image.load(":/res/esc_serial.png");
            else if (ui->cbEsc->currentText() == "PWM")
                image.load(":/res/esc_pwm_pro_micro.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        image.load(":/res/receiver_3v_hs.png");
        if (ui->gbEsc->isChecked() &&
             (ui->cbEsc->currentText() == "Hobbywing V3" ||
              ui->cbEsc->currentText() == "Hobbywing V4" ||
              ui->cbEsc->currentText() == "Kontronic"  ||
              ui->cbEsc->currentText() == "APD F" ||
              ui->cbEsc->currentText() == "APD HV"))
        {
            image.load(":/res/receiver_3v_ss_pro_micro.png");
        }
        if (ui->gbGps->isChecked() || ui->cbReceiver->currentText() == "Frsky Smartport" || ui->cbReceiver->currentText() == "Futaba SBUS2")
        {
            image.load(":/res/receiver_3v_ss_pro_micro.png");
        }
        if (ui->cbReceiver->currentText() == "Frsky D")
        {
            image.load(":/res/receiver_3v_ss_pro_micro_frskyd.png");
        }
        if (ui->cbReceiver->currentText() == "Spektrum XBUS")
        {
            image.load(":/res/receiver_3v_i2c_xbus_pro_micro.png");
        }
        if (ui->cbReceiver->currentText() == "Hitec")
        {
            image.load(":/res/receiver_3v_i2c_pro_micro.png");
        }
        paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));

        if (ui->gbCurrent->isChecked())
        {
            image.load(":/res/acs758_pro_micro.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbAirspeed->isChecked())
        {
            image.load(":/res/mpxv7002_pro_micro.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbAltitude->isChecked() && ui->cbBarometerType->currentText() == "BMP280")
        {
            image.load(":/res/bmp280_pro_micro.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbAltitude->isChecked() && ui->cbBarometerType->currentText() == "MS5611")
        {
            image.load(":/res/ms5611_pro_micro.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }
    }

    else if (ui->cbBoard->currentText() == "Arduino Micro")
    {
        if (ui->gbGps->isChecked())
        {
            image.load(":/res/bn220.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbTemperature1->isChecked())
        {
            image.load(":/res/ntc1.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbTemperature2->isChecked())
        {
            image.load(":/res/ntc2.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbEsc->isChecked())
        {
            if (ui->cbEsc->currentText() == "Hobbywing V3" || ui->cbEsc->currentText() == "Hobbywing V4" || ui->cbEsc->currentText() == "Kontronic"  || ui->cbEsc->currentText() == "APD F" || ui->cbEsc->currentText() == "APD HV")
                image.load(":/res/esc_serial.png");
            else if (ui->cbEsc->currentText() == "PWM")
                image.load(":/res/esc_pwm_micro.png");
            else if (ui->cbEsc->currentText() == "Castle link")
                image.load(":/res/esc_castle_micro.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        image.load(":/res/receiver_3v_hs.png");
        if (ui->gbEsc->isChecked() &&
             (ui->cbEsc->currentText() == "Hobbywing V3" ||
              ui->cbEsc->currentText() == "Hobbywing V4" ||
              ui->cbEsc->currentText() == "Kontronic"  ||
              ui->cbEsc->currentText() == "APD F" ||
              ui->cbEsc->currentText() == "APD HV"))
        {
            image.load(":/res/receiver_3v_ss_micro.png");
        }
        if (ui->gbGps->isChecked() || ui->cbReceiver->currentText() == "Frsky Smartport" || ui->cbReceiver->currentText() == "Futaba SBUS2")
        {
            image.load(":/res/receiver_3v_ss_micro.png");
        }
        if (ui->cbReceiver->currentText() == "Frsky D")
        {
            image.load(":/res/receiver_3v_ss_micro_frskyd.png");
        }
        if (ui->cbReceiver->currentText() == "Spektrum XBUS")
        {
            image.load(":/res/receiver_3v_i2c_xbus_micro.png");
        }
        if (ui->cbReceiver->currentText() == "Hitec")
        {
            image.load(":/res/receiver_3v_i2c_micro.png");
        }
        paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));

        if (ui->gbCurrent->isChecked())
        {
            image.load(":/res/acs758_micro.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbAirspeed->isChecked())
        {
            image.load(":/res/mpxv7002_micro.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbAltitude->isChecked() && ui->cbBarometerType->currentText() == "BMP280")
        {
            image.load(":/res/bmp280_micro.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbAltitude->isChecked() && ui->cbBarometerType->currentText() == "MS5611")
        {
            image.load(":/res/ms5611_micro.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }
    }

    else if (ui->cbBoard->currentText() == "Teensy LC/3.x")
    {
        if (ui->gbGps->isChecked())
        {
            image.load(":/res/bn220_teensy3.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbTemperature1->isChecked())
        {
            image.load(":/res/ntc1_teensy3.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbTemperature2->isChecked())
        {
            image.load(":/res/ntc2_teensy3.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbEsc->isChecked())
        {
            if (ui->cbEsc->currentText() == "Hobbywing V3" || ui->cbEsc->currentText() == "Hobbywing V4" || ui->cbEsc->currentText() == "Kontronic"  || ui->cbEsc->currentText() == "APD F" || ui->cbEsc->currentText() == "APD HV")
                image.load(":/res/esc_serial_teensy3.png");
            else if (ui->cbEsc->currentText() == "PWM")
                image.load(":/res/esc_pwm_teensy3.png");
            else if (ui->cbEsc->currentText() == "Castle link")
                image.load(":/res/esc_castle_teensylc.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        image.load(":/res/receiver_teensy3.png");
        paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));

        if (ui->gbCurrent->isChecked())
        {
            image.load(":/res/acs758_teensy3.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbAirspeed->isChecked())
        {
            image.load(":/res/mpxv7002_teensy3.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }
        if (ui->cbReceiver->currentText() == "Hitec" || ui->cbReceiver->currentText() == "Spektrum XBUS")
        {
            if (ui->gbAltitude->isChecked() && ui->cbBarometerType->currentText() == "BMP280")
            {
                image.load(":/res/bmp280_teensy3_i2c.png");
                paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
            }

            if (ui->gbAltitude->isChecked() && ui->cbBarometerType->currentText() == "MS5611")
            {
                image.load(":/res/ms5611_teensy3_i2c.png");
                paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
            }
        }
        else
        {
            if (ui->gbAltitude->isChecked() && ui->cbBarometerType->currentText() == "BMP280")
            {
                image.load(":/res/bmp280_teensy3.png");
                paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
            }

            if (ui->gbAltitude->isChecked() && ui->cbBarometerType->currentText() == "MS5611")
            {
                image.load(":/res/ms5611_teensy3.png");
                paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
            }
        }
    }

    else if (ui->cbBoard->currentText() == "Teensy 2")
    {
        if (ui->gbGps->isChecked())
        {
            image.load(":/res/bn220_teensy2.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbTemperature1->isChecked())
        {
            image.load(":/res/ntc1_teensy2.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbTemperature2->isChecked())
        {
            image.load(":/res/ntc2_teensy2.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbEsc->isChecked())
        {
            if (ui->cbEsc->currentText() == "Hobbywing V3" || ui->cbEsc->currentText() == "Hobbywing V4" || ui->cbEsc->currentText() == "Kontronic"  || ui->cbEsc->currentText() == "APD F" || ui->cbEsc->currentText() == "APD HV")
                image.load(":/res/esc_serial_teensy2.png");
            else if (ui->cbEsc->currentText() == "PWM")
                image.load(":/res/esc_pwm_teensy2.png");
            else if (ui->cbEsc->currentText() == "Castle link")
                image.load(":/res/esc_castle_teensy2.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        image.load(":/res/receiver_3v_hs_teensy2.png");
        if (ui->gbEsc->isChecked() &&
             (ui->cbEsc->currentText() == "Hobbywing V3" ||
              ui->cbEsc->currentText() == "Hobbywing V4" ||
              ui->cbEsc->currentText() == "Kontronic"  ||
              ui->cbEsc->currentText() == "APD F" ||
              ui->cbEsc->currentText() == "APD HV"))
        {
            image.load(":/res/receiver_3v_ss_teensy2.png");
        }
        if (ui->gbGps->isChecked() || ui->cbReceiver->currentText() == "Frsky Smartport" || ui->cbReceiver->currentText() == "Futaba SBUS2")
        {
            image.load(":/res/receiver_3v_ss_teensy2.png");
        }
        if (ui->cbReceiver->currentText() == "Frsky D")
        {
            image.load(":/res/receiver_3v_ss_teensy2_frskyd.png");
        }
        if (ui->cbReceiver->currentText() == "Spektrum XBUS")
        {
            image.load(":/res/receiver_3v_i2c_teensy2_xbus.png");
        }
        if (ui->cbReceiver->currentText() == "Hitec")
        {
            image.load(":/res/receiver_3v_i2c_teensy2.png");
        }
        paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));

        if (ui->gbCurrent->isChecked())
        {
            image.load(":/res/acs758_teensy2.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->cbAirspeed->isChecked())
        {
            image.load(":/res/mpxv7002_teensy2.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbAltitude->isChecked() && ui->cbBarometerType->currentText() == "BMP280")
        {
            image.load(":/res/bmp280_teensy2.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }

        if (ui->gbAltitude->isChecked() && ui->cbBarometerType->currentText() == "MS5611")
        {
            image.load(":/res/ms5611_teensy2.png");
            paint->drawImage(QPoint(0, 0), image.scaled(*size, Qt::IgnoreAspectRatio));
        }
    }

    label->setPixmap(*pix);
}
void MainWindow::exitApp()
{
    QApplication::quit();
}

void MainWindow::generateConfig()
{
    QString configString = "";

    configString +=""
                   "#ifndef CONFIG_H"
                   "\n#define CONFIG_H"
                   "\n#include \"constants.h\""
                   "\n";

    // Receiver
    configString += ""
                    "\n/* Receiver protocol */"
                    "\n#define RX_PROTOCOL ";
    switch (ui->cbReceiver->currentIndex()) {
    case 0:
        configString += "RX_SMARTPORT";
        break;
    case 1:
        configString += "RX_FRSKY";
        break;
    case 2:
        configString += "RX_XBUS";
        break;
    case 3:
        configString += "RX_SRXL";
        break;
    case 4:
        configString += "RX_IBUS";
        break;
    case 5:
        configString += "RX_SBUS";
        break;
    case 6:
        configString += "RX_MULTIPLEX";
        break;
    case 7:
        configString += "RX_JETIEX";
        break;
    case 8:
        configString += "RX_HITEC";
        break;
    }

    /* Sensors */

    // ESC
    configString += ""
                    "\n"
                    "\n/* Sensors */"
                    "\n#define CONFIG_ESC_PROTOCOL ";
    if (ui->gbEsc->isChecked()) {
        switch (ui->cbEsc->currentIndex()) {
        case 0:
            configString += "PROTOCOL_HW_V3";
            break;
        case 1:
            configString += "PROTOCOL_HW_V4";
            break;
        case 2:
            configString += "PROTOCOL_PWM";
            break;
        case 3:
            configString += "PROTOCOL_CASTLE";
            break;
        case 4:
            configString += "PROTOCOL_KONTRONIK";
            break;
        case 5:
            configString += "PROTOCOL_APD_F";
            break;
        case 6:
            configString += "PROTOCOL_APD_HV";
            break;
        }
    } else
        configString +="PROTOCOL_NONE";

    // GPS
    configString += "\n#define CONFIG_GPS ";
    if (ui->gbGps->isChecked())
        configString += "true";
    else
        configString += "false";
    configString += "\n#define GPS_BAUD_RATE ";
    configString += ui->cbGpsBaudrate->currentText();

    // Voltage 1
    configString += " \n#define CONFIG_VOLTAGE1 ";
    if (ui->gbVoltage1->isChecked()) configString += "true";
    else configString += "false";

    // Voltage 2
    configString += " \n#define CONFIG_VOLTAGE2 ";
    if (ui->gbVoltage2->isChecked()) configString += "true";
    else configString += "false";

    // Temperature 1
    configString += " \n#define CONFIG_NTC1 ";
    if (ui->cbTemperature1->isChecked()) configString += "true";
    else configString += "false";

    // Temperature 2
    configString += " \n#define CONFIG_NTC2 ";
    if (ui->cbTemperature2->isChecked()) configString += "true";
    else configString += "false";

    // Current
    configString += " \n#define CONFIG_CURRENT ";
    if (ui->gbCurrent->isChecked()) configString += "true";
    else configString += "false";

    // Airspeed
    configString += " \n#define CONFIG_AIRSPEED ";
    if (ui->cbAirspeed->isChecked()) configString += "true";
    else configString += "false";

    // Altitude
    configString += " \n#define CONFIG_I2C1_TYPE ";
    if (ui->gbAltitude->isChecked()) {
        if (ui->cbBarometerType->currentText() == "BMP280")
            configString += "I2C_BMP280";
        else if (ui->cbBarometerType->currentText() == "MS5611")
            configString += "I2C_MS5611";
    }
    else configString += "I2C_NONE";
    configString += " \n#define CONFIG_I2C1_ADDRESS ";
    configString += ui->cbAddress->currentText();

    // Refresh rate
    configString += ""
                    "\n"
                    "\n/* Refresh rate in 0.1s (1 = 100ms) */";
    configString += "\n#define CONFIG_REFRESH_RPM " + QString::number(ui->sbRpmRate->value() / 100);
    configString += "\n#define CONFIG_REFRESH_VOLT " + QString::number(ui->sbVoltageRate->value() / 100);
    configString += "\n#define CONFIG_REFRESH_CURR " + QString::number(ui->sbCurrentRate->value() / 100);
    configString += "\n#define CONFIG_REFRESH_TEMP " + QString::number(ui->sbTemperatureRate->value() / 100);
    configString += "\n#define CONFIG_REFRESH_GPS " + QString::number(ui->sbGpsRate->value() / 100);
    configString += "\n#define CONFIG_REFRESH_DEF 1";

    // Averaging
    configString += ""
                    "\n"
                    "\n/* Averaging elements (1 = no averaging) */";
    configString += "\n#define CONFIG_AVERAGING_ELEMENTS_RPM " + QString::number(ui->sbRpmAvg->value());
    configString += "\n#define CONFIG_AVERAGING_ELEMENTS_VOLT " + QString::number(ui->sbVoltageAvg->value());
    configString += "\n#define CONFIG_AVERAGING_ELEMENTS_CURR " + QString::number(ui->sbCurrentAvg->value());
    configString += "\n#define CONFIG_AVERAGING_ELEMENTS_TEMP " + QString::number(ui->sbTemperatureAvg->value());
    configString += "\n#define CONFIG_AVERAGING_ELEMENTS_VARIO " + QString::number(ui->sbVarioAvg->value());
    configString += "\n#define CONFIG_AVERAGING_ELEMENTS_DEF 1";

    // Analog voltage multipliers
    configString += ""
                    "\n"
                    "\n/* Analog voltage sensors */";
    configString += "\n#define VOLTAGE1_MULTIPLIER " + QString::number(ui->sbVoltage1Mult->value());
    configString += "\n#define VOLTAGE2_MULTIPLIER " + QString::number(ui->sbVoltage2Mult->value());

    // Analog current sensor
    configString += ""
                    "\n"
                    "\n/* Analog current sensor */";
    configString += "\n#define CURRENT_MULTIPLIER " + QString::number(1000 / ui->sbCurrentSens->value());
    if (ui->cbCurrentSensorType->currentText() == "Hall effect")
        configString += "\n#define CURRENT_OFFSET 0";
    if (ui->cbCurrentSensorType->currentText() == "Open loop hall effect")
        configString += "\n#define CURRENT_OFFSET 512";

    // RPM Multipliers
    configString += ""
                    "\n"
                    "\n/* RPM multipliers (optional, this may be done in transmitter*/";
    configString += "\n#define RPM_PAIR_OF_POLES " + QString::number(ui->sbPairOfPoles->value());
    configString += "\n#define RPM_PINION_TEETH " + QString::number(ui->sbMainTeeth->value());
    configString += "\n#define RPM_MAIN_TEETH " + QString::number(ui->sbPinionTeeth->value());

    // Altitude filter
    configString += ""
                    "\n"
                    "\n/* BMP filter. Higher filter = lower noise: 1 - low, 2 - medium, 3 - high */"
                    "\n#define BMP280_FILTER " + QString::number(ui->cbAltitudeFilter->currentIndex() + 1);

    //
    configString += ""
                    "\n"
                    "\n/* Pwm out */"
                    "\n#define CONFIG_PWMOUT false"
                    "\n#define PWMOUT_DUTY 0.5 // 0.5 = 50%"
                    "\n"
                    "\n/* Only smartport and opentx */"
                    "\n#define SENSOR_ID 10 // Sensor Id"
                    "\n#define DATA_ID 0x5000 // DataId (sensor type)"
                    "\n//#define ESC_SIGNATURE // HW V4 signature (only smartport). This outputs esc signature and raw current to sensors 5100, 5101 and 5102";

    // Lua Config
    if (ui->cbLuaConfig->isChecked()) configString += "\n#define CONFIG_LUA";
    else configString += "\n//#define CONFIG_LUA";

    // XBUS Clock stretch
    configString += ""
                    "\n"
                    "\n/* XBus */";
    if (ui->cbClockStretch->isChecked()) configString += "\n#define XBUS_CLOCK_STRECH_SWITCH";
    else configString += "\n//#define XBUS_CLOCK_STRECH_SWITCH";

    configString += ""
                    "\n"
                    "\n/* Jeti Ex */";
    if (ui->cbSpeedUnitsGps->currentText() == "km/h") configString += "\n#define JETI_GPS_SPEED_UNITS_KMH";
    else configString += "\n//#define JETI_GPS_SPEED_UNITS_KMH";

    configString += ""
                    "\n"
                    "\n/* Use library I2C_T3 for Teensy LC/3.X */"
                    "\n#define I2C_T3_TEENSY";

    // HW V4/V5 parameters
    configString += ""
                    "\n"
                    "\n/* Add init delay for FlyFun ESC. Uncomment if the ESC doesn't arm */";
    if (ui->cbInitDelay->isChecked()) configString += "\n#define ESC_INIT_DELAY 10000";
    else configString += "\n//#define ESC_INIT_DELAY 10000";
    configString += ""
                    "\n"
                    "\n/* HW V4/V5 parameters */";
    configString += "\n#define CURRENT_THRESHOLD " + QString::number(ui->sbCurrentThresold->value());
    configString += "\n#define ESCHW4_DIVISOR " + QString::number(ui->sbVoltageDivisor->value());
    configString += "\n#define ESCHW4_AMPGAIN " + QString::number(ui->sbCurrentMultiplier->value());
    configString += "\n#define ESCHW4_CURRENT_MAX " + QString::number(ui->sbCurrentMax->value());

    configString += ""
                    "\n"
                    "\n/* Force eeprom write */"
                    "\n//#define FORCE_EEPROM_WRITE // Uncomment to force write eeprom as defined in config.h. Useful when using lua and eeprom is messed up. Reflash againg with line commented or config will be reset at power up"
                    "\n"
                    "\n/* Debug"
                    "\n   Disconnect Vcc from the RC model to the Arduino"
                    "\n   Do not connect at the same time Vcc from the model and usb (TTL)"
                    "\n   Telemetry may not work properly in debug mode"
                    "\n   Connect arduino Rx to TTL Tx for flashing, then if applicabe connect arduino Rx to esc or gps"
                    "\n*/"
                    "\n"
                    "\n//#define DEBUG"
                    "\n//#define DEBUG_PACKET"
                    "\n//#define DEBUG_SBUS_MS"
                    "\n//#define DEBUG_EEPROM_WRITE"
                    "\n//#define DEBUG_EEPROM_READ"
                    "\n"
                    "\n//#define DEBUG_HW3"
                    "\n//#define DEBUG_HW4"
                    "\n//#define DEBUG_ESC_KONTRONIK"
                    "\n//#define DEBUG_APDF"
                    "\n//#define DEBUG_APDHV"
                    "\n//#define DEBUG_GPS"
                    "\n//#define DEBUG_PWM"
                    "\n//#define DEBUG_CASTLE"
                    "\n//#define DEBUG_CASTLE_RX"
                    "\n"
                    "\n//#define SIM_RX"
                    "\n//#define SIM_SENSORS"
                    "\n//#define SIM_LUA_SEND"
                    "\n//#define SIM_LUA_RECEIVE"
                    "\n"
                    "\n#endif";


    ui->txConfig->setText(configString);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_cbReceiver_currentIndexChanged(const QString &arg1)
{
    QGroupBox *gbRate = ui->gbReceiver->findChild<QGroupBox *>("gbRate");
    if (arg1 == "Spektrum XBUS") {
        ui->cbClockStretch->setVisible(true);
        gbRate->setVisible(false);
    } else {
        ui->cbClockStretch->setVisible(false);
        gbRate->setVisible(true);
    }

    if (arg1 == "Frsky D" || arg1 == "Frsky Smartport") {
        ui->cbLuaConfig->setVisible(true);
        gbRate->setVisible(true);
    } else {
        ui->cbLuaConfig->setVisible(false);
        gbRate->setVisible(false);
    }

    QLabel *lbSpeedUnitsGps = ui->gbGps->findChild<QLabel *>("lbSpeedUnitsGps");
    if (arg1 == "Jeti Ex Bus") {
        ui->cbSpeedUnitsGps->setVisible(true);
        lbSpeedUnitsGps->setVisible(true);
    } else {
        ui->cbSpeedUnitsGps->setVisible(false);
        lbSpeedUnitsGps->setVisible(false);
    }

}

void MainWindow::on_cbEsc_currentIndexChanged(const QString &arg1)
{
    QGroupBox *gbEscParameters = ui->gbEsc->findChild<QGroupBox *>("gbEscParameters");
    if (arg1 == "Hobbywing V4")
        gbEscParameters->setVisible(true);
    else
        gbEscParameters->setVisible(false);

}

void MainWindow::on_btCopy_clicked()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(ui->txConfig->toPlainText());
}

void MainWindow::on_cbEscModel_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "Platinum PRO v4 25/40/60") {
        ui->sbVoltageDivisor->setValue(11);
        ui->sbCurrentMultiplier->setValue(0);
        ui->sbCurrentMax->setValue(0);
        ui->cbInitDelay->setChecked(false);
    }
    else if (arg1 == "Platinum PRO v4 80A") {
        ui->sbVoltageDivisor->setValue(11);
        ui->sbCurrentMultiplier->setValue(8);
        ui->sbCurrentMax->setValue(100);
        ui->cbInitDelay->setChecked(false);
    }
    else if (arg1 == "Platinum PRO v4 100A") {
        ui->sbVoltageDivisor->setValue(11);
        ui->sbCurrentMultiplier->setValue(9);
        ui->sbCurrentMax->setValue(120);
        ui->cbInitDelay->setChecked(false);
    }
    else if (arg1 == "Platinum PRO v4 120A") {
        ui->sbVoltageDivisor->setValue(11);
        ui->sbCurrentMultiplier->setValue(10);
        ui->sbCurrentMax->setValue(140);
        ui->cbInitDelay->setChecked(false);
    }
    else if (arg1 == "Platinum PRO v4 150A") {
        ui->sbVoltageDivisor->setValue(15.75);
        ui->sbCurrentMultiplier->setValue(10);
        ui->sbCurrentMax->setValue(170);
        ui->cbInitDelay->setChecked(false);
    }
    else if (arg1 == "Platinum PRO v4 130A-HV") {
        ui->sbVoltageDivisor->setValue(21);
        ui->sbCurrentMultiplier->setValue(11.3);
        ui->sbCurrentMax->setValue(150);
        ui->cbInitDelay->setChecked(false);
    }
    else if (arg1 == "Platinum PRO v4 200A-HV") {
        ui->sbVoltageDivisor->setValue(21);
        ui->sbCurrentMultiplier->setValue(16.9);
        ui->sbCurrentMax->setValue(220);
        ui->cbInitDelay->setChecked(false);
    }
    else if (arg1 == "FlyFun 30/40A") {
        ui->sbVoltageDivisor->setValue(11);
        ui->sbCurrentMultiplier->setValue(0);
        ui->sbCurrentMax->setValue(0);
        ui->cbInitDelay->setChecked(true);
    }
    else if (arg1 == "FlyFun 60A") {
        ui->sbVoltageDivisor->setValue(11);
        ui->sbCurrentMultiplier->setValue(6);
        ui->sbCurrentMax->setValue(80);
        ui->cbInitDelay->setChecked(true);
    }
    else if (arg1 == "FlyFun 80A") {
        ui->sbVoltageDivisor->setValue(15.75);
        ui->sbCurrentMultiplier->setValue(12.4);
        ui->sbCurrentMax->setValue(100);
        ui->cbInitDelay->setChecked(true);
    }
    else if (arg1 == "FlyFun 120A") {
        ui->sbVoltageDivisor->setValue(21);
        ui->sbCurrentMultiplier->setValue(15);
        ui->sbCurrentMax->setValue(140);
        ui->cbInitDelay->setChecked(true);
    }
    else if (arg1 == "FlyFun 110A-HV") {
        ui->sbVoltageDivisor->setValue(21);
        ui->sbCurrentMultiplier->setValue(15);
        ui->sbCurrentMax->setValue(130);
        ui->cbInitDelay->setChecked(true);
    }
    else if (arg1 == "FlyFun 130A-HV") {
        ui->sbVoltageDivisor->setValue(21);
        ui->sbCurrentMultiplier->setValue(15);
        ui->sbCurrentMax->setValue(150);
        ui->cbInitDelay->setChecked(true);
    }
    else if (arg1 == "FlyFun 160A-HV") {
        ui->sbVoltageDivisor->setValue(21);
        ui->sbCurrentMultiplier->setValue(15);
        ui->sbCurrentMax->setValue(180);
        ui->cbInitDelay->setChecked(true);
    }
}

void MainWindow::on_cbBarometerType_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "BMP280")
    {
        ui->cbAltitudeFilter->setVisible(true);
        ui->lbAltitudeFilter->setVisible(true);
    }
    else
    {
        ui->cbAltitudeFilter->setVisible(false);
        ui->lbAltitudeFilter->setVisible(false);
    }
}

void MainWindow::on_gbEsc_toggled(bool arg1)
{
    Q_UNUSED(arg1);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_gbVoltage1_toggled(bool arg1)
{
    Q_UNUSED(arg1);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_gbVoltage2_toggled(bool arg1)
{
    Q_UNUSED(arg1);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_cbTemperature1_toggled(bool checked)
{
    Q_UNUSED(checked);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_cbTemperature2_toggled(bool checked)
{
    Q_UNUSED(checked);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_gbAltitude_toggled(bool arg1)
{
    Q_UNUSED(arg1);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_cbAirspeed_toggled(bool checked)
{
    Q_UNUSED(checked);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_gbCurrent_toggled(bool arg1)
{
    Q_UNUSED(arg1);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_cbBarometerType_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_cbEsc_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_cbBoard_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_cbReceiver_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_btCircuit_clicked()
{
    CircuitDialog circuitDialog;
    circuitDialog.setModal(true);
    circuitDialog.mainWindow = this;
    generateCircuit(circuitDialog.lbCircuit);
    circuitDialog.exec();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    generateCircuit(ui->lbCircuit);
}

void MainWindow::on_gbGps_toggled(bool arg1)
{
    Q_UNUSED(arg1);
    generateCircuit(ui->lbCircuit);
}

