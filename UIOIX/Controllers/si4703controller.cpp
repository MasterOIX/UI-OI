#include "si4703controller.h"
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <gpiod.h>
#include <QThread>
#include <QDebug>
#include <sys/ioctl.h>

Si4703Controller::Si4703Controller(QObject *parent) : QObject(parent), i2cFd(-1) {}

bool Si4703Controller::reset() {
    struct gpiod_chip *chip = gpiod_chip_open_by_name("gpiochip0");
    if (!chip) {
        qDebug() << "Failed to open GPIO chip.";
        return false;
    }

    struct gpiod_line *sdio = gpiod_chip_get_line(chip, SDIO_GPIO);
    struct gpiod_line *reset = gpiod_chip_get_line(chip, RESET_GPIO);
    if (!sdio || !reset) {
        qDebug() << "Failed to get GPIO lines.";
        gpiod_chip_close(chip);
        return false;
    }

    if (gpiod_line_request_output(sdio, "si4703", 0) < 0 ||
        gpiod_line_request_output(reset, "si4703", 0) < 0) {
        qDebug() << "Failed to set GPIO directions.";
        gpiod_chip_close(chip);
        return false;
    }

    gpiod_line_set_value(sdio, 0);
    gpiod_line_set_value(reset, 0);
    QThread::msleep(100);
    gpiod_line_set_value(reset, 1);
    QThread::msleep(100);

    gpiod_line_release(sdio);
    gpiod_line_release(reset);
    gpiod_chip_close(chip);

    i2cFd = open("/dev/i2c-1", O_RDWR);
    if (i2cFd < 0) {
        qDebug() << "Failed to open I2C device.";
        return false;
    }

    if (ioctl(i2cFd, I2C_SLAVE, I2C_ADDR) < 0) {
        qDebug() << "Failed to set I2C address.";
        return false;
    }

    qDebug() << "SI4703 reset and ready.";
    return true;
}

bool Si4703Controller::readRegisters(QVector<uint16_t> &regs) {
    uint8_t buffer[32];
    if (read(i2cFd, buffer, 32) != 32) {
        qDebug() << "I2C read failed.";
        return false;
    }

    regs.resize(16);
    int idx = 0x0A;
    for (int i = 0; i < 32; i += 2) {
        regs[idx] = (buffer[i] << 8) | buffer[i + 1];
        idx = (idx + 1) % 16;
    }
    return true;
}

bool Si4703Controller::writeRegisters(const QVector<uint16_t> &regs) {
    uint8_t out[12];
    for (int i = 0; i < 6; ++i) {
        out[2 * i]     = (regs[2 + i] >> 8) & 0xFF;
        out[2 * i + 1] = regs[2 + i] & 0xFF;
    }
    return write(i2cFd, out, 12) == 12;
}

bool Si4703Controller::initialize() {
    QVector<uint16_t> regs;
    if (!readRegisters(regs)) return false;

    regs[TEST1] = 0x8100;
    if (!writeRegisters(regs)) return false;
    QThread::msleep(500);

    if (!readRegisters(regs)) return false;

    regs[POWERCFG] = ENABLE | DMUTE;
    regs[SYSCONFIG1] = RDS | DE;
    regs[SYSCONFIG2] = (0x10 << 8) | SPACE | 0x000F;
    regs[SYSCONFIG3] = (0x04 << 4) | 0x08;

    if (!writeRegisters(regs)) return false;

    qDebug() << "SI4703 initialized.";
    return true;
}

bool Si4703Controller::tuneFrequency(double freqMHz) {
    QVector<uint16_t> regs;
    if (!readRegisters(regs)) return false;

    int channel = static_cast<int>((freqMHz - 87.5) / 0.1);
    regs[CHANNEL] = (channel & 0x03FF) | TUNE_BIT;
    if (!writeRegisters(regs)) return false;

    for (int i = 0; i < 30; ++i) {
        QThread::msleep(100);
        if (!readRegisters(regs)) return false;
        if (regs[STATUSRSSI] & STC_BIT) break;
    }

    regs[CHANNEL] &= ~TUNE_BIT;
    if (!writeRegisters(regs)) return false;
    QThread::msleep(100);

    if (!readRegisters(regs)) return false;
    if (!isMuted)
        regs[POWERCFG] |= DMUTE;
    else
        regs[POWERCFG] &= ~DMUTE;

    if (!writeRegisters(regs)) return false;

    qDebug() << "Tuned to" << freqMHz << "MHz.";
    emit tuned(freqMHz);
    return true;
}

void Si4703Controller::tuneNextStation() {
    currentStationIndex = (currentStationIndex + 1) % preferredStations.size();
    tuneFrequency(preferredStations[currentStationIndex]);
}

void Si4703Controller::tunePreviousStation() {
    currentStationIndex = (currentStationIndex - 1 + preferredStations.size()) % preferredStations.size();
    tuneFrequency(preferredStations[currentStationIndex]);
}

void Si4703Controller::mute(bool on) {
    isMuted = on;
    QVector<uint16_t> regs;
    if (!readRegisters(regs)) return;

    if (on)
        regs[POWERCFG] &= ~DMUTE;
    else
        regs[POWERCFG] |= DMUTE;

    if (writeRegisters(regs)) {
        qDebug() << (on ? "Muted" : "Unmuted");
    } else {
        qDebug() << "Failed to change mute state";
    }
}
