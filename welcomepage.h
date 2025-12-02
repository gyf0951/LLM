#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include <QWidget>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QKeyEvent>
#include <QTimer>
#include <QClipboard>
#include <QMimeData>
#include <QProcess>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QLabel>
#include <QListWidgetItem>
#include "historyuploader.h"


namespace Ui {
class welcomepage;
}

class welcomepage : public QWidget
{
    Q_OBJECT

public:
    explicit welcomepage(QWidget *parent = nullptr);
    ~welcomepage();

    // 加载历史记录
    void loadHistoryRecords(const QString &userId);

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;



signals:
    void sendMessageToChatPage(const QString &msg);  // 发送消息到聊天页面
    void historyItemClicked(const QVariantMap &record); // 点击历史记录信号  参数: record - 包含完整记录信息的Map (id, question, answer, time等)


private slots:
    void welcome_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_toggleBtn_clicked();

    void on_toggleBtn_2_clicked();

    void on_toolButton_2_clicked();

   // void on_toolButton_3_clicked();

    // 历史记录相关槽函数
    void onRecordsFetched(const QList<QVariantMap> &records); // 当从服务器成功获取到历史记录时调用
    void onFetchFailed(const QString &error);    // 当从服务器获取历史记录失败时调用
    void onHistoryItemClicked(QListWidgetItem *item);  // 当用户点击历史记录列表中的某一项时调用

private:
    Ui::welcomepage *ui;

    HistoryUploader *historyUploader;
    QString currentUserId;           // 当前用户ID

    void setupHistoryListWidget();   // 初始化历史记录控件样式
    void addHistoryItem(const QVariantMap &record); // 添加单条记录


};

#endif // WELCOMEPAGE_H
