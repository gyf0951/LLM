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

struct ModelConfig {
    QString id;
    QString name;
    QString displayName;
    QString description;
    QString themeColor;
    QString imagePath;

    QString apiKey;  //api key
};

namespace Ui {
class welcomepage;
}

class welcomepage : public QWidget
{
    Q_OBJECT

public:
    explicit welcomepage(QWidget *parent = nullptr);
    ~welcomepage();

    // 设置模型配置
    void setModelConfig(const ModelConfig& config);

    // 清除模型图片（当点击首页按钮时调用）
    void clearModelImage();

    //返回模型配置
    ModelConfig getCurrentModelConfig();

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;



signals:
    void sendMessageToChatPage(const QString &msg);

    void modelConfigChanged(const ModelConfig& config);  // 配置变化信号

private slots:
    void welcome_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_toggleBtn_clicked();

    void on_toggleBtn_2_clicked();

    void on_toolButton_2_clicked();

    void on_toolButton_3_clicked();

private:
    Ui::welcomepage *ui;


    // 添加模型配置相关成员
    QString m_currentModelId;
    QString m_currentModelName;
    QLabel* m_modelImageLabel;  // 动态添加的模型图片标签

    ModelConfig m_currentModelConfig;

    void addModelImageToHeader(const QString& imagePath); // 在header中添加模型图片
    void removeModelImageFromHeader(); // 从header中移除模型图片

    void updateWelcomeUI(); // 根据当前模型更新UI

};

#endif // WELCOMEPAGE_H
