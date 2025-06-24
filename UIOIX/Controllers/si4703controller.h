#ifndef SI4703CONTROLLER_H
#define SI4703CONTROLLER_H

#include <QObject>
#include <QVector>

class Si4703Controller : public QObject
{
    Q_OBJECT
public:
    explicit Si4703Controller(QObject *parent = nullptr);

    Q_INVOKABLE bool reset();
    Q_INVOKABLE bool initialize();
    Q_INVOKABLE bool tuneFrequency(double freqMHz);
    Q_INVOKABLE void tuneNextStation();
    Q_INVOKABLE void tunePreviousStation();
    Q_INVOKABLE void mute(bool on);

signals:
    void tuned(double frequency);
    void statusMessage(const QString &message);
    void errorMessage(const QString &message);

private:
    int i2cFd;
    static constexpr int I2C_ADDR = 0x10;
    static constexpr int RESET_GPIO = 23;
    static constexpr int SDIO_GPIO = 0;

    enum Register {
        POWERCFG = 0x02,
        CHANNEL = 0x03,
        SYSCONFIG1 = 0x04,
        SYSCONFIG2 = 0x05,
        SYSCONFIG3 = 0x06,
        TEST1 = 0x07,
        STATUSRSSI = 0x0A,
        READCHAN = 0x0B
    };

    static constexpr uint16_t TUNE_BIT = 0x8000;
    static constexpr uint16_t STC_BIT = 0x4000;
    static constexpr uint16_t DMUTE = 0x4000;
    static constexpr uint16_t ENABLE = 0x0001;
    static constexpr uint16_t RDS = 0x1000;
    static constexpr uint16_t DE = 0x0800;
    static constexpr uint16_t SPACE = 0x0010;

    bool isMuted = false;
    QVector<double> preferredStations = {88.0, 90.7, 94.5, 97.8, 100.1, 102.3, 104.8};
    int currentStationIndex = 0;

    bool readRegisters(QVector<uint16_t> &regs);
    bool writeRegisters(const QVector<uint16_t> &regs);
};

#endif // SI4703CONTROLLER_H
