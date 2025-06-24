#ifndef CANCONTROLLER_H
#define CANCONTROLLER_H

#include <QObject>
#include <QString>

class CanController : public QObject {
    Q_OBJECT

public:
    explicit CanController(QObject *parent = nullptr);
    bool initialize(const QString &interfaceName = "can0");

    Q_INVOKABLE void canSend(const QString &message);
    Q_INVOKABLE void canDump();

private:
    int socketFd = -1;
    QString iface;

    QByteArray encrypt(const QByteArray &data);
    QByteArray decrypt(const QByteArray &data);
};

#endif // CANCONTROLLER_H
