#ifndef CHATBUBBLEWIDGET_H
#define CHATBUBBLEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QDateTime>
#include <QScrollBar>
#include <QScrollArea>

class ChatBubbleItem : public QWidget
{
    Q_OBJECT
public:
    explicit ChatBubbleItem(const QString& message, const QDateTime& time, bool isFromMe, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_message;
    QDateTime m_time;
    bool m_isFromMe;
    QColor m_bubbleColor;
    QColor m_textColor;
};

class ChatBubbleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChatBubbleWidget(QWidget* parent = nullptr);
    
    void addMessage(const QString& message, bool isFromMe);
    void addSystemMessage(const QString& message);
    void clear();
    void setAutoScroll(bool enabled) { m_autoScroll = enabled; }
    bool autoScroll() const { return m_autoScroll; }

private:
    QVBoxLayout* m_layout;
    QWidget* m_container;
    QVBoxLayout* m_containerLayout;
    QScrollBar* m_scrollBar;
    bool m_autoScroll = true;
};

#endif // CHATBUBBLEWIDGET_H 