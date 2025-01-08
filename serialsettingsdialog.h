#ifndef SERIALSETTINGSDIALOG_H
#define SERIALSETTINGSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QSerialPort>
#include <QMessageBox>
#include <QCheckBox>
#include <QMouseEvent>

class SerialSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialSettingsDialog(QWidget *parent = nullptr);
    
    struct Settings {
        int baudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::StopBits stopBits;
        QSerialPort::Parity parity;
        QSerialPort::FlowControl flowControl;
        int bufferSize;
        int packageDelay;
        bool autoScroll;
    };
    
    Settings getSettings() const;
    void setSettings(const Settings& settings);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void handlePackageDelayChanged(int value);
    void handleBufferSizeChanged(int value);

private:
    QComboBox* m_baudRateBox;
    QComboBox* m_dataBitsBox;
    QComboBox* m_stopBitsBox;
    QComboBox* m_parityBox;
    QComboBox* m_flowControlBox;
    QSpinBox* m_bufferSizeBox;
    QSpinBox* m_packageDelayBox;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    QPushButton* m_defaultButton;
    QCheckBox* m_autoScrollBox;

    void setupUi();
    void loadDefaultSettings();
    void applyStyle();
    void setupComboBoxes();
    void setupSpinBoxes();

    QPoint m_dragPosition;
};

#endif // SERIALSETTINGSDIALOG_H 