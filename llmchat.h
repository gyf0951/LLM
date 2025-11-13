#ifndef LLMCHAT_H
#define LLMCHAT_H

#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QToolButton>

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
class LLMChat;
}

class LLMChat : public QWidget
{
    Q_OBJECT

public:
    explicit LLMChat(QWidget *parent = nullptr);
    ~LLMChat();

    void applyCurrentModel();  // 根据当前选中按钮重新应用模型


signals:
    void switchToWelcomePage();
    void sendMessageToChatPage(const QString &msg);
    void modelConfigChanged(const ModelConfig &config);

private slots:
    void on_toolButton_8_clicked();

    void on_toolButton_10_clicked();

    void on_toolButton_11_clicked();

    void on_toolButton_15_clicked();

    void on_toolButton_16_clicked();

    void on_toolButton_17_clicked();

    void on_toolButton_12_clicked();

    void on_pushButton_clicked();

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;



private:
    Ui::LLMChat *ui;

    ModelConfig m_currentModelConfig;
    QLabel* m_modelImageLabel = nullptr;

    void setModelConfig(const ModelConfig& config);
    void addModelImageToHeader(const QString& imagePath);
    void removeModelImageFromHeader();
    void updateChatUI();


    //存储当前选中的按钮
    QToolButton* m_selectedButton = nullptr;


    //  更新按钮样式的私有辅助函数
    void setSelectedButton(QToolButton *button);
};

#endif // LLMCHAT_H
