#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include "ch34x_qt.h"
#include "serialsettingsdialog.h"

class SerialManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialManager(QObject *parent = nullptr);
    ~SerialManager();

    bool openPort(const QString& portName);
    void closePort();
    bool isOpen() const;
    bool sendData(const QString& message);
    QStringList getAvailablePorts() const;

    void applySettings(const SerialSettingsDialog::Settings& settings);

signals:
    void messageReceived(const QString& message);
    void errorOccurred(const QString& error);
    void portsChanged();
    void connectionStatusChanged(bool connected);

private slots:
    void handleSerialData(const QByteArray& data);
    void handleSerialError(const QString& error);
    void handlePortsChanged();

private:
    CH341Qt* m_serialDevice;
    SerialSettingsDialog::Settings m_currentSettings;
};

#endif // SERIALMANAGER_H 
