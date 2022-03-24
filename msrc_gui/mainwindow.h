#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QClipboard>
#include <QPainter>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void generateCircuit(QLabel *label);

private:
    Ui::MainWindow *ui;

private slots:
    void generateConfig();
    void exitApp();

    void on_cbReceiver_currentIndexChanged(const QString &arg1);
    void on_cbEsc_currentIndexChanged(const QString &arg1);
    void on_gbEsc_toggled(bool arg1);
    void on_btCopy_clicked();
    void on_cbEscModel_currentIndexChanged(const QString &arg1);
    void on_cbBarometerType_currentIndexChanged(const QString &arg1);
    void on_gbVoltage1_toggled(bool arg1);
    void on_gbVoltage2_toggled(bool arg1);
    void on_cbTemperature1_toggled(bool checked);
    void on_cbTemperature2_toggled(bool checked);
    void on_gbAltitude_toggled(bool arg1);
    void on_cbAirspeed_toggled(bool checked);
    void on_gbCurrent_toggled(bool arg1);
    void on_cbBarometerType_currentTextChanged(const QString &arg1);
    void on_cbEsc_currentTextChanged(const QString &arg1);
    void on_cbBoard_currentTextChanged(const QString &arg1);
    void on_cbReceiver_currentTextChanged(const QString &arg1);
    void on_btCircuit_clicked();
    void resizeEvent(QResizeEvent* event);

    void on_gbGps_toggled(bool arg1);
};
#endif // MAINWINDOW_H
