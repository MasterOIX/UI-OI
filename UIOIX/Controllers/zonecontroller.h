#ifndef ZONECONTROLLER_H
#define ZONECONTROLLER_H

#include <QObject>

class ZoneController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoEnabled READ autoEnabled WRITE setAutoEnabled NOTIFY autoEnabledChanged)
    Q_PROPERTY(int temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(int seatHeating READ seatHeating WRITE setSeatHeating NOTIFY seatHeatingChanged)
public:
    explicit ZoneController(QObject *parent = nullptr);

    bool autoEnabled() const;
    void setAutoEnabled(bool newAutoEnabled);

    Q_INVOKABLE void increaseTemperature(const int &value);

    int temperature() const;
    void setTemperature(int newTemperature);

    int seatHeating() const;
    void setSeatHeating(int newSeatHeating);

signals:
    void autoEnabledChanged();
    void temperatureChanged();

    void seatHeatingChanged();

private:
    bool m_autoEnabled;
    int m_temperature;
    bool m_seatHeating;
};

#endif // ZONECONTROLLER_H
