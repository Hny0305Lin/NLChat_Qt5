#include "messagelistwindow.h"
#include <QHBoxLayout>

MessageListItem::MessageListItem(const QString& name, const QString& message, 
                               const QDateTime& time, QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(4);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    
    QHBoxLayout* topLayout = new QHBoxLayout();
    
    m_nameLabel = new QLabel(name, this);
    m_timeLabel = new QLabel(time.toString("hh:mm:ss"), this);
    
    m_nameLabel->setStyleSheet("font-weight: bold; color: #1890ff;");
    m_timeLabel->setStyleSheet("color: #8c8c8c; font-size: 9pt;");
    
    topLayout->addWidget(m_nameLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_timeLabel);
    
    m_messageLabel = new QLabel(message, this);
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setStyleSheet("color: #333333;");
    
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_messageLabel);
}

MessageListWindow::MessageListWindow(QWidget* parent) : QWidget(parent)
{
    setupUi();
    applyStyle();
}

void MessageListWindow::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // 标题栏
    m_titleLabel = new QLabel(tr("消息列表"), this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setFixedHeight(40);
    
    // 消息列表
    m_listWidget = new QListWidget(this);
    m_listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setFrameShape(QFrame::NoFrame);
    
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addWidget(m_listWidget);
    
    setFixedWidth(300);
}

void MessageListWindow::applyStyle()
{
    setStyleSheet(R"(
        MessageListWindow {
            background-color: white;
            border-left: 1px solid #e8e8e8;
        }
        QLabel#m_titleLabel {
            background-color: #001529;
            color: white;
            font-size: 12pt;
            font-weight: bold;
        }
        QListWidget {
            background-color: white;
            border: none;
        }
        QListWidget::item {
            background-color: white;
            border-bottom: 1px solid #f0f0f0;
            padding: 0px;
        }
        QListWidget::item:hover {
            background-color: #f5f5f5;
        }
    )");
}

void MessageListWindow::addMessage(const QString& portName, const QString& message, bool isFromMe)
{
    QString name = isFromMe ? tr("我") : portName;
    QListWidgetItem* item = new QListWidgetItem(m_listWidget);
    MessageListItem* widget = new MessageListItem(name, message, QDateTime::currentDateTime());
    
    item->setSizeHint(widget->sizeHint());
    m_listWidget->addItem(item);
    m_listWidget->setItemWidget(item, widget);
    m_listWidget->scrollToBottom();
}

void MessageListWindow::setCurrentPort(const QString& portName)
{
    m_currentPort = portName;
    m_titleLabel->setText(tr("消息列表 - %1").arg(portName));
} 