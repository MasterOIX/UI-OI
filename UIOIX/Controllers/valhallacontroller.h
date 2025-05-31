#ifndef VALHALLACONTROLLER_H
#define VALHALLACONTROLLER_H

#include "qjsonobject.h"
#include <QObject>
#include <QGeoCoordinate>
#include <QNetworkAccessManager>
#include <QTimer>
//#include <QTextToSpeech>

class ValhallaController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QGeoCoordinate currentLocation READ currentLocation WRITE setCurrentLocation NOTIFY currentLocationChanged)
    Q_PROPERTY(QGeoCoordinate destination READ destination WRITE setDestination NOTIFY destinationChanged)
    Q_PROPERTY(bool navigating READ navigating WRITE setNavigating NOTIFY navigatingChanged)
    Q_PROPERTY(QVariantList polylinePoints READ polylinePoints NOTIFY polylineChanged)
    Q_PROPERTY(double bearing READ bearing NOTIFY bearingChanged)
    Q_PROPERTY(int shapeIndex READ shapeIndex NOTIFY shapeIndexChanged)
    Q_PROPERTY(int etaTime READ etaTime NOTIFY navigationUpdated)
    Q_PROPERTY(double remainingDistance READ remainingDistance NOTIFY navigationUpdated)
    Q_PROPERTY(double distanceToNextManeuver READ distanceToNextManeuver NOTIFY nextManeuverDistanceChanged)
    Q_PROPERTY(QString navigationUpdatedInstruction READ navigationUpdatedInstruction NOTIFY navigationUpdated)
    Q_PROPERTY(QString verbalInstruction READ verbalInstruction NOTIFY navigationUpdated)

public:
    explicit ValhallaController(QObject *parent = nullptr);

    // === Properties ===
    QGeoCoordinate currentLocation() const;
    QGeoCoordinate destination() const;
    void setDestination(const QGeoCoordinate &destination);
    bool navigating() const;

    // === Methods ===
    Q_INVOKABLE void getRoute(double fromLat, double fromLon, double toLat, double toLon);
    Q_INVOKABLE void calculateBearing(QGeoCoordinate from, QGeoCoordinate to);
    Q_INVOKABLE void stopSimulation();
    Q_INVOKABLE void startNavigation();
    Q_INVOKABLE void simulateRoute();

    void setNavigating(bool newNavigating);
    QVariantList polylinePoints() const;

    double bearing() const;
    int shapeIndex() const;
    int etaTime() const;
    double remainingDistance() const;

    double distanceToNextManeuver() const;

    QString navigationUpdatedInstruction() const;

    void setCurrentLocation(const QGeoCoordinate &newCurrentLocation);

    QString verbalInstruction() const;

signals:
    void routeReady(const QString &polyline, double distanceKm, int durationSec);
    void navigationUpdated(const QString &instruction, double remainingKm, int remainingSec);
    void errorOccurred(const QString &errorMessage);

    void currentLocationChanged();
    void destinationChanged();
    void navigatingChanged();
    void polylineChanged();
    void bearingChanged();
    void shapeIndexChanged();
    void nextManeuverDistanceChanged();

private:
    void onReplyFinished(QNetworkReply *reply);
    void updatePosition(double lat, double lon);

    QVector<QGeoCoordinate> decodePolyline(const QString &polyline);
    int findClosestPointIndex(const QGeoCoordinate &position) const;
    double computeRemainingDistance(int currentIndex) const;
    int computeRemainingTime(int currentIndex) const;
    void rerouteFrom(const QGeoCoordinate &currentPos);

    // === Internal State ===
    QNetworkAccessManager m_networkManager;
    QJsonObject m_routeJson;
    QByteArray m_rawJson;
    QVector<QGeoCoordinate> m_polylinePoints;
    QList<QJsonObject> m_maneuvers;

    QGeoCoordinate m_currentLocation;
    QGeoCoordinate m_destination;
    QGeoCoordinate m_lastStart;

    QTimer *m_simulationTimer = nullptr;
    int m_simulationIndex = 0;
    bool m_navigating = false;
    double m_bearing;
    int m_shapeIndex = 0;
    int m_etaTime;
    double m_remainingDistance;
    double m_distanceToNextManeuver;
    QString m_navigationUpdatedInstruction;

    //QTextToSpeech *m_tts;
    QString m_verbalInstruction;
};

#endif // VALHALLACONTROLLER_H
