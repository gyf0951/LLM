#include "TextSream.h"

TextSream::TextSream(QWidget *parent)
    : QWidget{parent}
{
    m_pTextEdit = new QTextEdit();
    m_pTextEdit->setReadOnly(true);
    m_pTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

void TextSream::startStreaming()
{
    m_streamBuffer.clear();
    m_pTextEdit->clear();

    // 设置定时器间隔为50ms
    m_streamTimer.setInterval(50);
    connect(&m_streamTimer, &QTimer::timeout, this, [this]
    {
        if(!m_streamBuffer.isEmpty())
        {
            m_pTextEdit->setPlainText(m_pTextEdit->toPlainText() + m_streamBuffer);
            // 清空缓冲区
            m_streamBuffer.clear();

            // 滚动到底部
            // 滚动优化
            QMetaObject::invokeMethod(m_pTextEdit, [this](){
                QTextCursor c = m_pTextEdit->textCursor();
                c.movePosition(QTextCursor::End);
                m_pTextEdit->setTextCursor(c);
            }, Qt::QueuedConnection);
        }
    });
    m_streamTimer.start();
}


void TextSream::appendStreamingContent(const QString &chunk)
{
     m_streamBuffer += chunk; // 将分块内容存入缓冲区
}


void TextSream::finishStreaming()
{
    if(!m_streamBuffer.isEmpty())
    {
        m_pTextEdit->setPlainText(m_pTextEdit->toPlainText() + m_streamBuffer);
    }
}
