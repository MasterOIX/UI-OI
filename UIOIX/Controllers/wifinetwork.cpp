#include "wifinetwork.h"

WiFiNetwork::WiFiNetwork(QObject *parent)
    : QObject(parent), m_signal(0)
{
}

QString WiFiNetwork::name() const {
    return m_name;
}

void WiFiNetwork::setName(const QString &newName) {
    if (m_name != newName) {
        m_name = newName;
        emit nameChanged();
    }
}

int WiFiNetwork::signal() const {
    return m_signal;
}

void WiFiNetwork::setSignal(int newSignal) {
    if (m_signal != newSignal) {
        m_signal = newSignal;
        emit signalChanged();
    }
}

QString WiFiNetwork::security() const {
    return m_security;
}

void WiFiNetwork::setSecurity(const QString &newSecurity) {
    if (m_security != newSecurity) {
        m_security = newSecurity;
        emit securityChanged();
    }
}

QString WiFiNetwork::password() const {
    return m_password;
}

void WiFiNetwork::setPassword(const QString &newPassword) {
    if (m_password != newPassword) {
        m_password = newPassword;
        emit passwordChanged();
    }
}

