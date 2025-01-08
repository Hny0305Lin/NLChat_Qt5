#include "serialsettingsdialog.h"
#include <QVBoxLayout>
#include "uilayoutmanager.h"

SerialSettingsDialog::SerialSettingsDialog(QWidget *parent) : QDialog(parent)
{
    setupUi();
    loadDefaultSettings();
    applyStyle();
}

void SerialSettingsDialog::setupUi()
{
    // 创建控件
    m_baudRateBox = new QComboBox(this);
    m_dataBitsBox = new QComboBox(this);
    m_stopBitsBox = new QComboBox(this);
    m_parityBox = new QComboBox(this);
    m_flowControlBox = new QComboBox(this);
    m_bufferSizeBox = new QSpinBox(this);
    m_packageDelayBox = new QSpinBox(this);
    m_autoScrollBox = new QCheckBox(tr("自动滚动到最新消息"), this);
    m_autoScrollBox->setChecked(true);
    m_autoScrollBox->setEnabled(false);
    
    m_defaultButton = new QPushButton(tr("恢复默认"), this);
    m_okButton = new QPushButton(tr("确定"), this);
    m_cancelButton = new QPushButton(tr("取消"), this);
    
    // 设置控件属性
    setupComboBoxes();
    setupSpinBoxes();
    
    // 使用布局管理器设置界面
    UILayoutManager::setupSerialSettingsDialog(
        this, m_baudRateBox, m_dataBitsBox, m_stopBitsBox,
        m_parityBox, m_flowControlBox, m_bufferSizeBox,
        m_packageDelayBox, m_autoScrollBox, m_defaultButton,
        m_okButton, m_cancelButton
    );
    
    // 连接信号槽
    connect(m_defaultButton, &QPushButton::clicked, this, &SerialSettingsDialog::loadDefaultSettings);
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_packageDelayBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SerialSettingsDialog::handlePackageDelayChanged);
    connect(m_bufferSizeBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SerialSettingsDialog::handleBufferSizeChanged);
}

// 添加新的辅助函数来设置下拉框和数值框的选项
void SerialSettingsDialog::setupComboBoxes()
{
    // 波特率选项
    QList<qint32> baudRates = {9600, 19200, 38400, 57600, 115200, 921600};
    for(qint32 rate : baudRates) {
        m_baudRateBox->addItem(QString::number(rate), rate);
    }
    
    // 数据位选项
    QList<QPair<QString, QSerialPort::DataBits>> dataBits = {
        {"5", QSerialPort::Data5},
        {"6", QSerialPort::Data6},
        {"7", QSerialPort::Data7},
        {"8", QSerialPort::Data8}
    };
    for(const auto& pair : dataBits) {
        m_dataBitsBox->addItem(pair.first, pair.second);
    }
    
    // 停止位选项
    QList<QPair<QString, QSerialPort::StopBits>> stopBits = {
        {"1", QSerialPort::OneStop},
        {"1.5", QSerialPort::OneAndHalfStop},
        {"2", QSerialPort::TwoStop}
    };
    for(const auto& pair : stopBits) {
        m_stopBitsBox->addItem(pair.first, pair.second);
    }
    
    // 校验位选项
    QList<QPair<QString, QSerialPort::Parity>> parityBits = {
        {tr("无"), QSerialPort::NoParity},
        {tr("奇校验"), QSerialPort::OddParity},
        {tr("偶校验"), QSerialPort::EvenParity},
        {tr("标记"), QSerialPort::MarkParity},
        {tr("空格"), QSerialPort::SpaceParity}
    };
    for(const auto& pair : parityBits) {
        m_parityBox->addItem(pair.first, pair.second);
    }
    
    // 流控制选项
    QList<QPair<QString, QSerialPort::FlowControl>> flowControls = {
        {tr("无"), QSerialPort::NoFlowControl},
        {tr("硬件"), QSerialPort::HardwareControl},
        {tr("软件"), QSerialPort::SoftwareControl}
    };
    for(const auto& pair : flowControls) {
        m_flowControlBox->addItem(pair.first, pair.second);
    }
}

void SerialSettingsDialog::setupSpinBoxes()
{
    // 缓冲区大小设置
    m_bufferSizeBox->setRange(1024, 8192);
    m_bufferSizeBox->setSingleStep(1024);
    m_bufferSizeBox->setSuffix(" bytes");
    m_bufferSizeBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    
    // 合包延迟设置
    m_packageDelayBox->setRange(1, 120);
    m_packageDelayBox->setSuffix(" ms");
    m_packageDelayBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
}

void SerialSettingsDialog::loadDefaultSettings()
{
    int index;
    
    // 设置波特率 115200
    index = m_baudRateBox->findData(115200);
    if(index >= 0) m_baudRateBox->setCurrentIndex(index);
    
    // 设置数据位 8
    index = m_dataBitsBox->findData(QSerialPort::Data8);
    if(index >= 0) m_dataBitsBox->setCurrentIndex(index);
    
    // 设置停止位 1
    index = m_stopBitsBox->findData(QSerialPort::OneStop);
    if(index >= 0) m_stopBitsBox->setCurrentIndex(index);
    
    // 设置校验位 无
    index = m_parityBox->findData(QSerialPort::NoParity);
    if(index >= 0) m_parityBox->setCurrentIndex(index);
    
    // 设置流控制 无
    index = m_flowControlBox->findData(QSerialPort::NoFlowControl);
    if(index >= 0) m_flowControlBox->setCurrentIndex(index);
    
    // 设置缓冲区大小和合包延迟
    m_bufferSizeBox->setValue(4096);  // 4KB 缓冲区
    m_packageDelayBox->setValue(50);   // 50ms 延迟
    m_autoScrollBox->setChecked(true); // 自动滚动开启
}

SerialSettingsDialog::Settings SerialSettingsDialog::getSettings() const
{
    Settings settings;
    settings.baudRate = m_baudRateBox->currentData().toInt();
    settings.dataBits = static_cast<QSerialPort::DataBits>(m_dataBitsBox->currentData().toInt());
    settings.stopBits = static_cast<QSerialPort::StopBits>(m_stopBitsBox->currentData().toInt());
    settings.parity = static_cast<QSerialPort::Parity>(m_parityBox->currentData().toInt());
    settings.flowControl = static_cast<QSerialPort::FlowControl>(m_flowControlBox->currentData().toInt());
    settings.bufferSize = m_bufferSizeBox->value();
    settings.packageDelay = m_packageDelayBox->value();
    settings.autoScroll = true;  // 强制设置为true
    return settings;
}

void SerialSettingsDialog::setSettings(const Settings& settings)
{
    // 设置波特率
    int index = m_baudRateBox->findData(settings.baudRate);
    if(index >= 0) m_baudRateBox->setCurrentIndex(index);
    
    // 设置数据位
    index = m_dataBitsBox->findData(settings.dataBits);
    if(index >= 0) m_dataBitsBox->setCurrentIndex(index);
    
    // 设置停止位
    index = m_stopBitsBox->findData(settings.stopBits);
    if(index >= 0) m_stopBitsBox->setCurrentIndex(index);
    
    // 设置校验位
    index = m_parityBox->findData(settings.parity);
    if(index >= 0) m_parityBox->setCurrentIndex(index);
    
    // 设置流控制
    index = m_flowControlBox->findData(settings.flowControl);
    if(index >= 0) m_flowControlBox->setCurrentIndex(index);
    
    m_bufferSizeBox->setValue(settings.bufferSize);
    m_packageDelayBox->setValue(settings.packageDelay);
    m_autoScrollBox->setChecked(true);  // 忽略传入的设置，总是保持选中
}

void SerialSettingsDialog::applyStyle()
{
    setStyleSheet(R"(
        QDialog {
            background-color: #f0f2f5;
        }
        QLabel {
            color: white;
            font-size: 10pt;
        }
        QComboBox {
            color: black;
            padding: 6px 12px;
            border: 1px solid #d9d9d9;
            border-radius: 4px;
            background-color: white;
            min-width: 150px;
            min-height: 20px;
        }
        QComboBox QAbstractItemView {
            color: black;
            background-color: white;
            selection-background-color: #e6f7ff;
            selection-color: black;
        }
        QSpinBox {
            color: black;
            padding: 6px 12px;
            border: 1px solid #d9d9d9;
            border-radius: 4px;
            background-color: white;
            min-width: 150px;
            min-height: 20px;
        }
        QPushButton {
            padding: 8px 16px;
            border-radius: 4px;
            border: none;
            color: white;
            min-width: 80px;
        }
        QPushButton#m_okButton {
            background-color: #1890ff;
        }
        QPushButton#m_okButton:hover {
            background-color: #40a9ff;
        }
        QPushButton#m_cancelButton {
            background-color: #ff4d4f;
        }
        QPushButton#m_cancelButton:hover {
            background-color: #ff7875;
        }
        QPushButton#m_defaultButton {
            background-color: #52c41a;
        }
        QPushButton#m_defaultButton:hover {
            background-color: #73d13d;
        }
    )");
}

void SerialSettingsDialog::handlePackageDelayChanged(int value)
{
    if(value < 10) {
        QMessageBox::warning(this, tr("警告"),
            tr("当前合包延迟设置为 %1ms，小于10ms可能会影响通信效果。\n"
               "建议设置在10-120ms之间。").arg(value));
    }
}

void SerialSettingsDialog::handleBufferSizeChanged(int value)
{
    if(value < 1024) {
        QMessageBox::warning(this, tr("警告"),
            tr("缓冲区大小设置过小（%1 bytes）！\n"
               "过小的缓冲区可能导致数据丢失，建议设置不小于1024 bytes。").arg(value));
    }
    else if(value > 8192) {
        QMessageBox::warning(this, tr("警告"),
            tr("缓冲区大小设置过大（%1 bytes）！\n"
               "过大的缓冲区可能导致内存占用过高，建议设置不超过8192 bytes。").arg(value));
    }
}

bool SerialSettingsDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            m_dragPosition = mouseEvent->globalPos() - frameGeometry().topLeft();
            event->accept();
            return true;
        }
    } else if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->buttons() & Qt::LeftButton) {
            move(mouseEvent->globalPos() - m_dragPosition);
            event->accept();
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
} 
