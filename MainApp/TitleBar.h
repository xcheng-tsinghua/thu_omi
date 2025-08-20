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
    // ��ť�������ź�;
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

    QPushButton* m_pButtonMin;          // ��С����ť;
    QPushButton* m_pButtonRestore;      // ��󻯻�ԭ��ť;
    QPushButton* m_pButtonMax;          // ��󻯰�ť;
    QPushButton* m_pButtonClose;        // �رհ�ť;

    ButtonType m_buttonType;

    // �ƶ����ڵı���;
    bool m_isPressed;
    QPoint m_startMovePos;
};