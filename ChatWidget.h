#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QListWidget>
#include <QStyledItemDelegate>
#include <QTextEdit>
#include <QTimer>


class ChatDelegate : public QStyledItemDelegate
{
public:
    ChatDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    // <-- FIX: 提取常量，方便统一修改
    const int m_marginV = 5;  // 气泡/文本 上下的外边距
    const int m_paddingH = 12; // 气泡内部的水平内边距
    const int m_paddingV = 8;  // 气泡内部的垂直内边距


};

class ChatWidget : public QListWidget
{
    Q_OBJECT
public:
    ChatWidget(QWidget *parent = nullptr);

    void addMessage(const QString &text, bool isMe);

    // 专门用于处理流式消息的追加
    void appendToLastMessage(const QString &textFragment, bool isMe);

    bool isAtBottom() const;
    void delayedScrollToBottom();
};

#endif // CHATWIDGET_H
