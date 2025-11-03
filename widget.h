#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include "ChatWidget.h"
#include <QClipboard>
#include <QMimeData>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QGuiApplication>
#include "Chat.h"
#include "welcomepage.h"
#include <QStackedWidget>
#include <QProcess>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QListWidget>

#include <QUuid>


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE


struct ChatSession {
    QString id;
    QString title;
    QVector<QString> messages;
};


class Widget : public QWidget
{
    Q_OBJECT

public:
    // Widget(QWidget *parent = nullptr);
    explicit Widget(Chat *chatInstance, QWidget *parent = nullptr);
    ~Widget();

signals:
    void sessionSelected(const QString &id);  // 切换会话
    void newSessionCreated(const QString &id);

public slots:
    void receiveMessageFromWelcome(const QString &msg);

    void onNewChatClicked();   //新建新会话
    void onSessionSelected(QListWidgetItem *item); // 切换历史会话

private:
    bool eventFilter(QObject *obj, QEvent *ev) override;
    void resizeEvent(QResizeEvent *event) override;



private slots:
    void pushButton_clicked();
 //   void onMessageReceived(const QString &message);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_toolButton_8_clicked();

    void on_toolButton_9_clicked();

private:
    Ui::Widget *ui;
    Chat *chat;

    QVector<ChatSession> sessions; // 所有会话
    QString currentSessionId;      // 当前会话 id
};
#endif // WIDGET_H
