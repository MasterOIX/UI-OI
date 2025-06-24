#include "cancontroller.h"
#include <QDebug>
#include <cstring>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>

CanController::CanController(QObject *parent)
    : QObject(parent) {}

bool CanController::initialize(const QString &interfaceName) {
    iface = interfaceName;
    struct ifreq ifr{};
    struct sockaddr_can addr{};

    socketFd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socketFd < 0) {
        qWarning() << "Failed to open CAN socket.";
        return false;
    }

    strncpy(ifr.ifr_name, iface.toStdString().c_str(), IFNAMSIZ - 1);
    if (ioctl(socketFd, SIOCGIFINDEX, &ifr) < 0) {
        qWarning() << "Failed to get interface index.";
        return false;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(socketFd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        qWarning() << "Failed to bind CAN socket.";
        return false;
    }

    qDebug() << "CAN interface" << iface << "initialized.";
    return true;
}

void CanController::canSend(const QString &message) {
    if (socketFd < 0) {
        qWarning() << "CAN interface not initialized.";
        return;
    }

    struct can_frame frame{};
    QByteArray data = encrypt(message.toUtf8());

    frame.can_id = 0x123;
    frame.can_dlc = qMin(data.size(), 8);
    memcpy(frame.data, data.constData(), frame.can_dlc);

    if (write(socketFd, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        qWarning() << "CAN send failed.";
    } else {
        qDebug() << "Sent CAN message:" << message;
    }
}

void CanController::canDump() {
    if (socketFd < 0) {
        qWarning() << "CAN interface not initialized.";
        return;
    }

    struct can_frame frame{};
    while (true) {
        ssize_t nbytes = read(socketFd, &frame, sizeof(struct can_frame));
        if (nbytes < 0) {
            qWarning() << "CAN read error.";
            break;
        }

        QByteArray raw((const char *)frame.data, frame.can_dlc);
        QByteArray plain = decrypt(raw);
        qDebug() << "Received CAN msg:" << QString::fromUtf8(plain);
    }
}

// 🔐 Dummy encryption — replace with your crypto lib
QByteArray CanController::encrypt(const QByteArray &data) {
    return data; // TODO: replace with AES or ChaCha20
}

QByteArray CanController::decrypt(const QByteArray &data) {
    return data; // TODO: replace with decryption
}
