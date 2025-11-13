#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QListWidget>
#include <QTextEdit>
#include <QTimer>




class ChatWidget : public QListWidget
{
    Q_OBJECT
public:
    ChatWidget(QWidget *parent = nullptr);

    void addMessage(const QString &text, bool isMe);

    // 专门用于处理流式消息的追加
    void appendToLastMessage(const QString &textFragment, bool isMe);

    void clear(); //清理

protected:

    void resizeEvent(QResizeEvent *event) override;

private:
    bool isAtBottom() const;
    void delayedScrollToBottom();

private slots:
    void updateItemHeight(QListWidgetItem* item);

private:
    // 跟踪最后一条AI消息，用于流式追加
    QString m_lastAiMessage;      // 存储最后一条AI消息的 *完整* Markdown 文本
    QListWidgetItem* m_lastAiItem; // 指向最后一条AI消息的 *item*
};

#endif // CHATWIDGET_H
