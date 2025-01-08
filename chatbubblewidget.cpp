#include "chatbubblewidget.h"
#include <QScrollBar>
#include <QPainterPath>

ChatBubbleItem::ChatBubbleItem(const QString& message, const QDateTime& time, bool isFromMe, QWidget* parent)
    : QWidget(parent)
    , m_message(message)
    , m_time(time)
    , m_isFromMe(isFromMe)
    , m_bubbleColor(isFromMe ? QColor("#95de64") : QColor("#69c0ff"))
    , m_textColor(isFromMe ? QColor("#135200") : QColor("#003a8c"))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    setMinimumHeight(50);
}

void ChatBubbleItem::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 计算文本区域
    QFontMetrics fm(font());
    QRect textRect = fm.boundingRect(0, 0, width() * 0.7, INT_MAX, 
                                   Qt::TextWordWrap, m_message);
    
    // 计算气泡位置
    int bubblePadding = 12;
    int bubbleMargin = 20;
    int timeHeight = 20;
    QRect bubbleRect = textRect.adjusted(-bubblePadding, -bubblePadding,
                                       bubblePadding, bubblePadding);
    
    if(m_isFromMe) {
        bubbleRect.moveRight(width() - bubbleMargin);
    } else {
        bubbleRect.moveLeft(bubbleMargin);
    }
    bubbleRect.moveTop(timeHeight);

    // 绘制时间
    painter.setPen(QColor("#8c8c8c"));
    QString timeStr = m_time.toString("hh:mm:ss");
    QRect timeRect(0, 0, width(), timeHeight);
    painter.drawText(timeRect, m_isFromMe ? Qt::AlignRight : Qt::AlignLeft, 
                    QString("[%1]").arg(timeStr));

    // 绘制气泡
    QPainterPath path;
    int radius = 16;
    int triangleSize = 8;
    
    if(m_isFromMe) {
        // 右侧气泡
        path.addRoundedRect(bubbleRect, radius, radius);
        QPolygonF triangle;
        triangle << bubbleRect.topRight() + QPoint(triangleSize, triangleSize)
                << bubbleRect.topRight() + QPoint(-triangleSize, triangleSize * 2)
                << bubbleRect.topRight() + QPoint(-triangleSize, 0);
        path.addPolygon(triangle);
    } else {
        // 左侧气泡
        path.addRoundedRect(bubbleRect, radius, radius);
        QPolygonF triangle;
        triangle << bubbleRect.topLeft() + QPoint(-triangleSize, triangleSize)
                << bubbleRect.topLeft() + QPoint(triangleSize, triangleSize * 2)
                << bubbleRect.topLeft() + QPoint(triangleSize, 0);
        path.addPolygon(triangle);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(m_bubbleColor);
    painter.drawPath(path);

    // 绘制文本
    painter.setPen(m_textColor);
    painter.drawText(bubbleRect, Qt::AlignLeft | Qt::TextWordWrap, m_message);
}

ChatBubbleWidget::ChatBubbleWidget(QWidget* parent) : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    // 创建一个容器来放置气泡
    m_container = new QWidget;
    m_containerLayout = new QVBoxLayout(m_container);
    m_containerLayout->setSpacing(10);
    m_containerLayout->setContentsMargins(10, 10, 10, 10);
    m_containerLayout->addStretch();

    // 创建滚动区域
    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidget(m_container);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // 设置滚动区域样式，隐藏滚动条
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            border: none;
            background: white;
        }
        QScrollBar:vertical {
            width: 0px;
            background: transparent;
        }
        QScrollBar::handle:vertical {
            background: transparent;
        }
        QScrollBar::add-line:vertical {
            height: 0px;
        }
        QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )");

    m_layout->addWidget(scrollArea);
    m_scrollBar = scrollArea->verticalScrollBar();
}

void ChatBubbleWidget::addMessage(const QString& message, bool isFromMe)
{
    ChatBubbleItem* bubble = new ChatBubbleItem(message, QDateTime::currentDateTime(), 
                                               isFromMe, m_container);
    m_containerLayout->insertWidget(m_containerLayout->count() - 1, bubble);
    
    if(m_autoScroll) {
        m_scrollBar->setValue(m_scrollBar->maximum());
    }
}

void ChatBubbleWidget::addSystemMessage(const QString& message)
{
    QLabel* systemMsg = new QLabel(m_container);
    systemMsg->setAlignment(Qt::AlignCenter);
    systemMsg->setText(QString("<div style='background: rgba(0,0,0,0.1); padding: 6px 12px; "
                              "border-radius: 12px; color: #666666;'>[%1] %2</div>")
                      .arg(QTime::currentTime().toString("hh:mm:ss"))
                      .arg(message));
    m_containerLayout->insertWidget(m_containerLayout->count() - 1, systemMsg);
    
    if(m_autoScroll) {
        m_scrollBar->setValue(m_scrollBar->maximum());
    }
}

void ChatBubbleWidget::clear()
{
    QLayoutItem* item;
    while((item = m_containerLayout->takeAt(0)) != nullptr) {
        if(item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    m_containerLayout->addStretch();
} 