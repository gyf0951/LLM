#ifndef HISTORYUPLOADER_H
#define HISTORYUPLOADER_H

#include <QObject>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

class HistoryUploader : public QObject
{
    Q_OBJECT
public:
    explicit HistoryUploader(QObject *parent = nullptr);

    //上传
    void uploadRecord(const QString &userId,
                      const QString &modelType,
                      const QString &question,
                      const QString &answer,
                      const QString &questionTime,
                      const QString &answerTime);

    // 获取历史记录
    void fetchRecords(const QString &userId);

signals:
    void uploadSuccess();
    void uploadFailed(const QString &error);

    //获取成功后把数据传出去
    void recordsFetched(const QList<QVariantMap> &records);
    void fetchFailed(const QString &error);

private:
    QString generateUUID();  // 生成 32 位 uuid
    QNetworkAccessManager *manager;
};

#endif // HISTORYUPLOADER_H
