#ifndef LLMCHAT_H
#define LLMCHAT_H

#include <QWidget>


// 前向声明 welcomepage 和 ModelConfig
class welcomepage;
struct ModelConfig;


namespace Ui {
class LLMChat;
}

class LLMChat : public QWidget
{
    Q_OBJECT

public:
    explicit LLMChat(QWidget *parent = nullptr);
    ~LLMChat();

    // 设置欢迎页引用
    void setWelcomePage(welcomepage* page);

signals:
    void switchToWelcomePage();

private slots:
    void on_toolButton_8_clicked();

    void on_toolButton_10_clicked();

    void on_toolButton_11_clicked();

    void on_toolButton_15_clicked();

    void on_toolButton_16_clicked();

    void on_toolButton_17_clicked();

private:
    Ui::LLMChat *ui;
    welcomepage* m_welcomePage;
};

#endif // LLMCHAT_H
