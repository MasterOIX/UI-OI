#include "infocontroller.h"
#include <QDebug>

InfoController::InfoController(QObject *parent)
    : QObject(parent),
    m_totalDistance(123456),
    m_range(123),
    m_consumption(12.3),
    m_averageSpeed(60),
    m_startTime("12:00"),
    m_tripDistance(0),
    m_tripDuration("0:00hr"),
    m_tripConsumption(0.0),
    m_tripAverageSpeed(0),
    m_resetAutomatically(false),
    m_startTotalDistance(0),
    m_startTotalConsumption(0.0),
    m_lastReceivedDistance(0),
    m_lastReceivedConsumption(0.0),
    m_settingsManager(this)
{
    m_totalDistance = m_settingsManager.loadTotalDistance();
    m_range = m_settingsManager.loadRange();
    m_consumption = m_settingsManager.loadConsumption();
    m_averageSpeed = m_settingsManager.loadAverageSpeed();
    m_startTime = m_settingsManager.loadStartTime();
    m_tripDistance = m_settingsManager.loadTripDistance();
    m_tripDuration = m_settingsManager.loadTripDuration();
    m_tripConsumption = m_settingsManager.loadTripConsumption();
    m_tripAverageSpeed = m_settingsManager.loadTripAverageSpeed();
    m_resetAutomatically = m_settingsManager.loadResetAutomatically();

    m_startTotalDistance = m_totalDistance;
    m_startTotalConsumption = m_consumption;
    m_resetTimestamp = QDateTime::currentDateTime();
    m_lastReceivedDistance = m_totalDistance;
    m_lastReceivedConsumption = m_consumption;

    if (m_resetAutomatically) {
        resetTrip();
    }

    // Timer to update trip duration every minute
    connect(&m_tripTimer, &QTimer::timeout, this, [this]() {
        QStringList parts = m_tripDuration.split(":");
        if (parts.size() != 2) return;

        bool ok1 = false, ok2 = false;
        int hours = parts[0].toInt(&ok1);
        int mins = parts[1].remove("hr").toInt(&ok2);

        if (ok1 && ok2) {
            mins++;
            if (mins >= 60) {
                mins = 0;
                hours++;
            }
            QString updated = QString("%1:%2hr")
                                  .arg(hours)
                                  .arg(mins, 2, 10, QChar('0'));
            setTripDuration(updated);
        }
    });
    m_tripTimer.start(60000);  // every 1 minute
}


int InfoController::totalDistance() const { return m_totalDistance; }
void InfoController::setTotalDistance(int value) {
    if (m_totalDistance == value) return;
    m_totalDistance = value;
    emit totalDistanceChanged(m_totalDistance);
    m_settingsManager.saveTotalDistance(m_totalDistance);
}

int InfoController::range() const { return m_range; }
void InfoController::setRange(int value) {
    if (m_range == value) return;
    m_range = value;
    emit rangeChanged(m_range);
    m_settingsManager.saveRange(m_range);
}

double InfoController::consumption() const { return m_consumption; }
void InfoController::setConsumption(double value) {
    if (qFuzzyCompare(m_consumption, value)) return;
    m_consumption = value;
    emit consumptionChanged(m_consumption);
    m_settingsManager.saveConsumption(m_consumption);
}

int InfoController::averageSpeed() const { return m_averageSpeed; }
void InfoController::setAverageSpeed(int value) {
    if (m_averageSpeed == value) return;
    m_averageSpeed = value;
    emit averageSpeedChanged(m_averageSpeed);
    m_settingsManager.saveAverageSpeed(m_averageSpeed);
}

QString InfoController::startTime() const { return m_startTime; }
void InfoController::setStartTime(const QString &value) {
    if (m_startTime == value) return;
    m_startTime = value;
    emit startTimeChanged(m_startTime);
    m_settingsManager.saveStartTime(m_startTime);
}

int InfoController::tripDistance() const { return m_tripDistance; }
void InfoController::setTripDistance(int value) {
    if (m_tripDistance == value) return;
    m_tripDistance = value;
    emit tripDistanceChanged(m_tripDistance);
    m_settingsManager.saveTripDistance(m_tripDistance);
}

QString InfoController::tripDuration() const { return m_tripDuration; }
void InfoController::setTripDuration(const QString &value) {
    if (m_tripDuration == value) return;
    m_tripDuration = value;
    emit tripDurationChanged(m_tripDuration);
    m_settingsManager.saveTripDuration(m_tripDuration);
}

double InfoController::tripConsumption() const { return m_tripConsumption; }
void InfoController::setTripConsumption(double value) {
    if (qFuzzyCompare(m_tripConsumption, value)) return;
    m_tripConsumption = value;
    emit tripConsumptionChanged(m_tripConsumption);
    m_settingsManager.saveTripConsumption(m_tripConsumption);
}

int InfoController::tripAverageSpeed() const { return m_tripAverageSpeed; }
void InfoController::setTripAverageSpeed(int value) {
    if (m_tripAverageSpeed == value) return;
    m_tripAverageSpeed = value;
    emit tripAverageSpeedChanged(m_tripAverageSpeed);
    m_settingsManager.saveTripAverageSpeed(m_tripAverageSpeed);
}

bool InfoController::resetAutomatically() const { return m_resetAutomatically; }
void InfoController::setResetAutomatically(bool value) {
    if (m_resetAutomatically == value) return;
    m_resetAutomatically = value;
    emit resetAutomaticallyChanged(m_resetAutomatically);
    m_settingsManager.saveResetAutomatically(m_resetAutomatically);
}

void InfoController::resetTrip() {
    m_resetTimestamp = QDateTime::currentDateTime();
    m_startTotalDistance = m_totalDistance;
    m_startTotalConsumption = m_consumption;

    QString now = m_resetTimestamp.time().toString("hh:mm");
    setStartTime(now);
    setTripDistance(0);
    setTripDuration("0:00hr");
    setTripConsumption(0.0);
    setTripAverageSpeed(0);
}

void InfoController::updateFromCAN(int currentCANdistanceKm, double currentCANconsumptionKWh)
{
    int deltaDistance = currentCANdistanceKm - m_lastReceivedDistance;
    double deltaConsumption = currentCANconsumptionKWh - m_lastReceivedConsumption;

    if (deltaDistance > 0) {
        setTotalDistance(m_totalDistance + deltaDistance);
    }

    if (deltaConsumption > 0.0) {
        setConsumption(m_consumption + deltaConsumption);
    }

    m_lastReceivedDistance = currentCANdistanceKm;
    m_lastReceivedConsumption = currentCANconsumptionKWh;

    setTripDistance(m_totalDistance - m_startTotalDistance);
    setTripConsumption(m_consumption - m_startTotalConsumption);

    int durationMinutes = m_resetTimestamp.secsTo(QDateTime::currentDateTime()) / 60;
    int hours = durationMinutes / 60;
    int mins = durationMinutes % 60;
    setTripDuration(QString("%1:%2hr").arg(hours).arg(mins, 2, 10, QChar('0')));

    if (durationMinutes > 0) {
        double durationHours = durationMinutes / 60.0;
        setTripAverageSpeed(static_cast<int>(m_tripDistance / durationHours));
    }
}
