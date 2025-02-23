#include "serialmanager.h"

SerialManager::SerialManager(QObject *parent) : QObject(parent)
{
    m_serialDevice = new CH34xQt(this);

    connect(m_serialDevice, &CH34xQt::dataReceived,
            this, &SerialManager::handleSerialData);
    connect(m_serialDevice, &CH34xQt::errorOccurred,
            this, &SerialManager::handleSerialError);
    connect(m_serialDevice, &CH34xQt::portsChanged,
            this, &SerialManager::handlePortsChanged);
}

SerialManager::~SerialManager()
{
    closePort();
}

bool SerialManager::openPort(const QString& portName)
{
    bool success = m_serialDevice->openDevice(portName);
    if(success) {
        applySettings(m_currentSettings);
        emit connectionStatusChanged(true);
    }
    return success;
}

void SerialManager::closePort()
{
    if(m_serialDevice->isOpen()) {
        m_serialDevice->closeDevice();
        emit connectionStatusChanged(false);
    }
}

bool SerialManager::isOpen() const
{
    return m_serialDevice->isOpen();
}

bool SerialManager::sendData(const QString& message)
{
    return m_serialDevice->writeData(message.toUtf8());
}

QStringList SerialManager::getAvailablePorts() const
{
    return CH34xQt::availablePorts();
}

void SerialManager::handleSerialData(const QByteArray& data)
{
    QString message = QString::fromUtf8(data);
    emit messageReceived(message);
}

void SerialManager::handleSerialError(const QString& error)
{
    emit errorOccurred(error);
}

void SerialManager::handlePortsChanged()
{
    emit portsChanged();
}

void SerialManager::applySettings(const SerialSettingsDialog::Settings& settings)
{
    if(m_serialDevice) {
        m_currentSettings = settings;
        
        m_serialDevice->setBaudRate(settings.baudRate);
        m_serialDevice->setDataBits(settings.dataBits);
        m_serialDevice->setStopBits(settings.stopBits);
        m_serialDevice->setParity(settings.parity);
        m_serialDevice->setFlowControl(settings.flowControl);
        m_serialDevice->setReadBufferSize(settings.bufferSize);
        
        qDebug() << "Applied serial settings:";
        qDebug() << "Baud rate:" << settings.baudRate;
        qDebug() << "Data bits:" << settings.dataBits;
        qDebug() << "Stop bits:" << settings.stopBits;
        qDebug() << "Parity:" << settings.parity;
        qDebug() << "Flow control:" << settings.flowControl;
        qDebug() << "Buffer size:" << settings.bufferSize;
    }
} 
