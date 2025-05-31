#include "valhallacontroller.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QDebug>
#include <QtMath>
#include <limits>

ValhallaController::ValhallaController(QObject *parent)
    : QObject(parent),
    m_currentLocation(44.354618, 26.154010),
    m_simulationTimer(nullptr),
    m_simulationIndex(0),
    m_navigating(false),
    m_shapeIndex(-1),
    m_etaTime(0),
    m_remainingDistance(0.0)
    //m_verbalInstruction("")
{
    connect(&m_networkManager, &QNetworkAccessManager::finished,
            this, &ValhallaController::onReplyFinished);
    //m_tts = new QTextToSpeech(this);
    //qDebug() << "Current voice:" << m_tts->voice().name();
    //m_tts->setLocale(QLocale::English);
    //m_tts->setVolume(1.0);
}

QGeoCoordinate ValhallaController::currentLocation() const {
    return m_currentLocation;
}

QGeoCoordinate ValhallaController::destination() const {
    return m_destination;
}

void ValhallaController::setDestination(const QGeoCoordinate &destination) {
    if (destination != m_destination) {
        m_destination = destination;
        emit destinationChanged();
    }
}

bool ValhallaController::navigating() const {
    return m_navigating;
}


void ValhallaController::startNavigation()
{
    getRoute(m_currentLocation.latitude(), m_currentLocation.longitude(),
             m_destination.latitude(), m_destination.longitude());
}

void ValhallaController::getRoute(double fromLat, double fromLon, double toLat, double toLon) {
    qDebug() << "[Valhalla] 🚀 Sending route request...";
    QJsonObject requestJson{
        {"locations", QJsonArray{
                                 QJsonObject{{"lat", fromLat}, {"lon", fromLon}},
                                 QJsonObject{{"lat", toLat}, {"lon", toLon}}}},
        {"costing", "auto"},
        {"directions_options", QJsonObject{{"units", "kilometers"}}}
    };

    QNetworkRequest request(QUrl("http://localhost:8002/route"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_networkManager.post(request, QJsonDocument(requestJson).toJson());
    m_lastStart = QGeoCoordinate(fromLat, fromLon);
    setDestination(QGeoCoordinate(toLat, toLon));

    qDebug() << "[Valhalla] From:" << fromLat << fromLon << "To:" << toLat << toLon;
}

void ValhallaController::calculateBearing(QGeoCoordinate from, QGeoCoordinate to)
{
    double lat1 = qDegreesToRadians(from.latitude());
    double lon1 = qDegreesToRadians(from.longitude());
    double lat2 = qDegreesToRadians(to.latitude());
    double lon2 = qDegreesToRadians(to.longitude());

    double deltaLon = lon2 - lon1;

    double y = qSin(deltaLon) * qCos(lat2);
    double x = qCos(lat1) * qSin(lat2) -
               qSin(lat1) * qCos(lat2) * qCos(deltaLon);

    double bearingRad = qAtan2(y, x);
    double bearingDeg = qRadiansToDegrees(bearingRad);

    // Normalize to 0–360
    if (bearingDeg < 0)
        bearingDeg += 360;

    m_bearing = bearingDeg;
    emit bearingChanged();
}

void ValhallaController::onReplyFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qWarning() << "[Valhalla] ❌ Network error:" << reply->errorString();
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject root = doc.object();
    if (!root.contains("trip")) {
        emit errorOccurred("No 'trip' in response");
        return;
    }

    m_routeJson = root["trip"].toObject();
    m_rawJson = doc.toJson();
    QJsonObject firstLeg = m_routeJson["legs"].toArray().first().toObject();

    m_maneuvers.clear();
    for (const QJsonValue &val : firstLeg["maneuvers"].toArray()) {
        QJsonObject maneuver = val.toObject();
        m_maneuvers.append(maneuver);
        qDebug() << "[Valhalla] 📍 Maneuver:" << maneuver["instruction"].toString()
                 << "from" << maneuver["begin_shape_index"].toInt()
                 << "to" << maneuver["end_shape_index"].toInt();
    }

    QString shape = firstLeg["shape"].toString();
    m_polylinePoints = decodePolyline(shape);
    emit polylineChanged();

    double distance = firstLeg["summary"].toObject()["length"].toDouble();
    int duration = static_cast<int>(firstLeg["summary"].toObject()["time"].toDouble());

    qDebug() << "[Valhalla] ✅ Route received. Distance:" << distance << "km, Duration:" << duration << "sec";

    emit routeReady(shape, distance, duration);
    m_simulationIndex = 0;
    simulateRoute();

    reply->deleteLater();
}

QVector<QGeoCoordinate> ValhallaController::decodePolyline(const QString &polyline) {
    QVector<QGeoCoordinate> coords;
    int index = 0, lat = 0, lon = 0;

    while (index < polyline.length()) {
        int b, shift = 0, result = 0;
        do {
            b = polyline[index++].unicode() - 63;
            result |= (b & 0x1F) << shift;
            shift += 5;
        } while (b >= 0x20);
        lat += (result & 1) ? ~(result >> 1) : (result >> 1);

        shift = 0;
        result = 0;
        do {
            b = polyline[index++].unicode() - 63;
            result |= (b & 0x1F) << shift;
            shift += 5;
        } while (b >= 0x20);
        lon += (result & 1) ? ~(result >> 1) : (result >> 1);

        QGeoCoordinate coord(lat / 1e6, lon / 1e6);
        coords.append(coord);
    }

    qDebug() << "[Valhalla] 📦 Decoded polyline with" << coords.size() << "points.";
    return coords;
}

void ValhallaController::simulateRoute() {
    if (m_polylinePoints.size() < 2)
        return;

    if(m_navigating){
        stopSimulation();
    }
    m_simulationIndex = 0;
    m_navigating = true;
    emit navigatingChanged();

    m_simulationTimer = new QTimer(this);
    double totalDistance = m_routeJson["summary"].toObject()["length"].toDouble() * 1000;
    double totalTime = m_routeJson["summary"].toObject()["time"].toDouble();
    double avgSpeed = totalTime > 0 ? totalDistance / totalTime : 10.0;

    connect(m_simulationTimer, &QTimer::timeout, this, [this, avgSpeed]() mutable {
        if (m_simulationIndex >= m_polylinePoints.size() - 1) {
            stopSimulation();
            return;
        }

        // 🧠 Simulate off-route at a specific index for testing
        if (m_simulationIndex == m_polylinePoints.size() / 2) {
            QGeoCoordinate offRoute(44.4381, 26.1018);
            qDebug() << "[Valhalla] 💥 Simulating off-route jump!";

            // Prevent repeating off-route indefinitely
            m_simulationIndex++;  // move forward so we don't re-trigger this
            updatePosition(offRoute.latitude(), offRoute.longitude());
            return;
        }

        QGeoCoordinate coord = m_polylinePoints[m_simulationIndex++];
        setCurrentLocation(coord);
        updatePosition(coord.latitude(), coord.longitude());

        QGeoCoordinate next = m_polylinePoints[qMin(m_simulationIndex, m_polylinePoints.size() - 1)];
        int delay = static_cast<int>((coord.distanceTo(next) / avgSpeed) * 1000);
        m_simulationTimer->setInterval(qMax(50, delay));
    });

    m_simulationTimer->start(0);
}


void ValhallaController::updatePosition(double lat, double lon) {
    if (m_polylinePoints.isEmpty()) return;

    QGeoCoordinate pos(lat, lon);
    int closestIndex = findClosestPointIndex(pos);

    if (closestIndex != m_shapeIndex) {
        m_shapeIndex = closestIndex;
        emit shapeIndexChanged();
    }

    qDebug() << "[Valhalla] 📍 Updating position to:" << lat << lon
             << "Closest shape index:" << closestIndex;

    if (closestIndex == -1 || pos.distanceTo(m_polylinePoints[closestIndex]) > 50) {
        qDebug() << "[Valhalla] ⚠️ Off-route! Triggering re-route.";

        stopSimulation(); // 🛑 FIRST stop the current simulation cleanly

        qDebug() << "[Valhalla] 🔁 Rerouting from:" << lat << lon;
        getRoute(lat, lon, m_destination.latitude(), m_destination.longitude());
        return;
    }

    m_remainingDistance = computeRemainingDistance(closestIndex);
    m_etaTime = computeRemainingTime(closestIndex);

    m_navigationUpdatedInstruction = "Continue";

    double distanceToNextInstruction = 0.0;
    for (const auto &m : std::as_const(m_maneuvers)) {
        int beginIdx = m["begin_shape_index"].toInt();
        if (beginIdx > closestIndex) {
            m_navigationUpdatedInstruction = m["instruction"].toString();

            int endIdx = m["end_shape_index"].toInt();
            for (int i = closestIndex; i < qMin(endIdx, m_polylinePoints.size() - 1); ++i) {
                distanceToNextInstruction += m_polylinePoints[i].distanceTo(m_polylinePoints[i + 1]);
            }
            /*if (distanceToNextInstruction > 200) {
                if(m.contains("verbal_post_transition_instruction")) {
                    if (m_verbalInstruction != m["verbal_post_transition_instruction"].toString()){
                        m_verbalInstruction = m["verbal_post_transition_instruction"].toString();
                        qDebug() << "[Valhalla] 📣 Verbal instruction:" << m_verbalInstruction;
                        m_tts->say(m_verbalInstruction);
                    }
                }
                else {
                    if (m_verbalInstruction != m_navigationUpdatedInstruction){
                        m_verbalInstruction = m_navigationUpdatedInstruction;
                        qDebug() << "[Valhalla] 📣 Verbal instruction:" << m_verbalInstruction;
                        m_tts->say(m_verbalInstruction);
                    }
                }
            }
            else if (distanceToNextInstruction > 150 && distanceToNextInstruction < 200) {
                if (m.contains("verbal_pre_transition_instruction"))
                    if (m_verbalInstruction != m["verbal_pre_transition_instruction"].toString()) {
                        m_verbalInstruction = m["verbal_pre_transition_instruction"].toString();
                        qDebug() << "[Valhalla] 📣 Verbal instruction:" << m_verbalInstruction;
                        m_tts->say(m_verbalInstruction);
                    }
            }
            else if (distanceToNextInstruction < 30) {
                if (m.contains("verbal_succinct_transition_instruction"))
                    if (m_verbalInstruction != m["verbal_succinct_transition_instruction"].toString()){
                        m_verbalInstruction = m["verbal_succinct_transition_instruction"].toString();
                        qDebug() << "[Valhalla] 📣 Verbal instruction:" << m_verbalInstruction;
                        m_tts->say(m_verbalInstruction);
                    }
            }*/

            break;
        }
    }

    qDebug() << "[Valhalla] 📣 Next:" << m_navigationUpdatedInstruction
             << "| Remaining:" << m_remainingDistance << "km," << m_etaTime << "sec";

    emit navigationUpdated(m_navigationUpdatedInstruction, m_remainingDistance, m_etaTime);
    if (distanceToNextInstruction != m_distanceToNextManeuver) {
        m_distanceToNextManeuver = distanceToNextInstruction;
        emit nextManeuverDistanceChanged();
    }
}

int ValhallaController::findClosestPointIndex(const QGeoCoordinate &position) const {
    int closest = -1;
    double minDist = std::numeric_limits<double>::max();
    for (int i = 0; i < m_polylinePoints.size(); ++i) {
        double dist = position.distanceTo(m_polylinePoints[i]);
        if (dist < minDist) {
            minDist = dist;
            closest = i;
        }
    }
    return closest;
}

double ValhallaController::computeRemainingDistance(int fromIndex) const {
    double dist = 0;
    for (int i = fromIndex; i < m_polylinePoints.size() - 1; ++i)
        dist += m_polylinePoints[i].distanceTo(m_polylinePoints[i + 1]);
    return dist / 1000.0;
}

int ValhallaController::computeRemainingTime(int fromIndex) const {
    double totalDist = m_routeJson["summary"].toObject()["length"].toDouble() * 1000;
    double totalTime = m_routeJson["summary"].toObject()["time"].toDouble();
    double remainingDist = computeRemainingDistance(fromIndex) * 1000;
    return totalDist > 0 ? static_cast<int>((remainingDist / totalDist) * totalTime) : 0;
}

void ValhallaController::stopSimulation() {
    if (m_simulationTimer) {
        if (m_simulationTimer->isActive())
            m_simulationTimer->stop();

        m_simulationTimer->deleteLater();
        m_simulationTimer = nullptr;
    }

    if (m_navigating) {
        m_navigating = false;
        emit navigatingChanged();
    }
    m_polylinePoints.clear();
    m_shapeIndex = -1;

    emit polylineChanged();
    emit shapeIndexChanged();

    qDebug() << "[Valhalla] ⛔ Simulation stopped.";
}

void ValhallaController::rerouteFrom(const QGeoCoordinate &currentPos) {
    qDebug() << "[Valhalla] 🔁 Rerouting from:" << currentPos.latitude() << currentPos.longitude();

    // Stop the old simulation first
    stopSimulation();

    // Call getRoute with the new origin and current destination
    getRoute(currentPos.latitude(), currentPos.longitude(), m_destination.latitude(), m_destination.longitude());
}

QVariantList ValhallaController::polylinePoints() const {
    QVariantList list;
    for (const QGeoCoordinate &coord : m_polylinePoints) {
        list.append(QVariant::fromValue(coord));
    }
    return list;
}

void ValhallaController::setNavigating(bool newNavigating)
{
    if (m_navigating == newNavigating)
        return;
    m_navigating = newNavigating;
    emit navigatingChanged();
}

double ValhallaController::bearing() const
{
    return m_bearing;
}

int ValhallaController::shapeIndex() const
{
    return m_shapeIndex;
}

int ValhallaController::etaTime() const
{
    return m_etaTime;
}

double ValhallaController::remainingDistance() const
{
    return m_remainingDistance;
}

double ValhallaController::distanceToNextManeuver() const
{
    return m_distanceToNextManeuver;
}

QString ValhallaController::navigationUpdatedInstruction() const
{
    return m_navigationUpdatedInstruction;
}

void ValhallaController::setCurrentLocation(const QGeoCoordinate &newCurrentLocation)
{
    calculateBearing(m_currentLocation, newCurrentLocation);
    if (m_currentLocation == newCurrentLocation)
        return;
    m_currentLocation = newCurrentLocation;
    emit currentLocationChanged();
}

QString ValhallaController::verbalInstruction() const
{
    return m_verbalInstruction;
}
