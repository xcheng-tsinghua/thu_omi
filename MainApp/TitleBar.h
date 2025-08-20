#pragma once
#include <QWidget>
#include "CustomTitle.h"
#include "qdockwidget.h"

class QLabel;
class QPushButton;

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget* parent = 0);
    ~TitleBar();

signals:
    // 按钮触发的信号;
    void signalButtonMinClicked();
    void signalButtonRestoreClicked();
    void signalButtonMaxClicked();
    void signalButtonCloseClicked();

private slots:
    void onButtonMinClicked();
    void onButtonRestoreClicked();
    void onButtonMaxClicked();
    void onButtonCloseClicked();

private:
    void paintEvent(QPaintEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    QLabel* m_pIconLabel;
    QLabel* m_pTitleLabel;

    QPushButton* m_pButtonMin;          // 最小化按钮;
    QPushButton* m_pButtonRestore;      // 最大化还原按钮;
    QPushButton* m_pButtonMax;          // 最大化按钮;
    QPushButton* m_pButtonClose;        // 关闭按钮;

    ButtonType m_buttonType;

    // 移动窗口的变量;
    bool m_isPressed;
    QPoint m_startMovePos;
};