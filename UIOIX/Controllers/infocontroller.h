// infocontroller.h
#ifndef INFOCONTROLLER_H
#define INFOCONTROLLER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <Controllers/settingsmanager.h>
#include <QTimer>

class InfoController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int totalDistance READ totalDistance WRITE setTotalDistance NOTIFY totalDistanceChanged)
    Q_PROPERTY(int range READ range WRITE setRange NOTIFY rangeChanged)
    Q_PROPERTY(double consumption READ consumption WRITE setConsumption NOTIFY consumptionChanged)
    Q_PROPERTY(int averageSpeed READ averageSpeed WRITE setAverageSpeed NOTIFY averageSpeedChanged)

    Q_PROPERTY(QString startTime READ startTime WRITE setStartTime NOTIFY startTimeChanged)
    Q_PROPERTY(int tripDistance READ tripDistance WRITE setTripDistance NOTIFY tripDistanceChanged)
    Q_PROPERTY(QString tripDuration READ tripDuration WRITE setTripDuration NOTIFY tripDurationChanged)
    Q_PROPERTY(double tripConsumption READ tripConsumption WRITE setTripConsumption NOTIFY tripConsumptionChanged)
    Q_PROPERTY(int tripAverageSpeed READ tripAverageSpeed WRITE setTripAverageSpeed NOTIFY tripAverageSpeedChanged)

    Q_PROPERTY(bool resetAutomatically READ resetAutomatically WRITE setResetAutomatically NOTIFY resetAutomaticallyChanged)

public:
    explicit InfoController(QObject *parent = nullptr);

    int totalDistance() const;
    void setTotalDistance(int value);

    int range() const;
    void setRange(int value);

    double consumption() const;
    void setConsumption(double value);

    int averageSpeed() const;
    void setAverageSpeed(int value);

    QString startTime() const;
    void setStartTime(const QString &value);

    int tripDistance() const;
    void setTripDistance(int value);

    QString tripDuration() const;
    void setTripDuration(const QString &value);

    double tripConsumption() const;
    void setTripConsumption(double value);

    int tripAverageSpeed() const;
    void setTripAverageSpeed(int value);

    bool resetAutomatically() const;
    void setResetAutomatically(bool value);

    Q_INVOKABLE void resetTrip();
    Q_INVOKABLE void updateFromCAN(int currentCANdistanceKm, double currentCANconsumptionKWh);

signals:
    void totalDistanceChanged(int);
    void rangeChanged(int);
    void consumptionChanged(double);
    void averageSpeedChanged(int);

    void startTimeChanged(QString);
    void tripDistanceChanged(int);
    void tripDurationChanged(QString);
    void tripConsumptionChanged(double);
    void tripAverageSpeedChanged(int);

    void resetAutomaticallyChanged(bool);

private:
    int m_totalDistance;
    int m_range;
    double m_consumption;
    int m_averageSpeed;

    QString m_startTime;
    int m_tripDistance;
    QString m_tripDuration;
    double m_tripConsumption;
    int m_tripAverageSpeed;

    bool m_resetAutomatically;

    QDateTime m_resetTimestamp;
    int m_startTotalDistance;
    double m_startTotalConsumption;

    int m_lastReceivedDistance;
    double m_lastReceivedConsumption;

    SettingsManager m_settingsManager;
    QTimer m_tripTimer;
};

#endif // INFOCONTROLLER_H
