#ifndef TEXTSREAM_H
#define TEXTSREAM_H

#include <QWidget>
#include <QTextEdit>
#include <QTimer>

class TextSream : public QWidget
{
    Q_OBJECT
public:
    explicit TextSream(QWidget *parent = nullptr);
    void startStreaming(); //开始定时器
    void appendStreamingContent(const QString &chunk); //把文字先存起来，等待定时器节拍输出
    void finishStreaming();

private:
    QTextEdit *m_pTextEdit;
    // 新增流式显示相关成员
    QString m_streamBuffer;
    QTimer m_streamTimer;

signals:
};

#endif // TEXTSREAM_H
