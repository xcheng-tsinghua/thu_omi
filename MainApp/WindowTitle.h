#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "windows.h"
#include "tool_button.h"
#include "qlayout.h"
#include "qlineedit.h"
/*
����һ���µ�QWidget��,����������UI���涼�ŵ����������,�����������Ϊ�˷���ı�����
*/
//class WidgetPar : public QWidget
//{
//    Q_OBJECT
//public:
//    explicit WidgetPar(QWidget* parent = 0);
//
//signals:
//
//public slots:
//    //void paintEvent(QPaintEvent *event);
//private:
//    void paintEvent(QPaintEvent* event) override;
////protected:
////    virtual bool winEvent(MSG* message, long* result)override;
//};

/*
�Զ���ı�������
*/
class WindowTitle : public QWidget
{
    Q_OBJECT
public:
    explicit WindowTitle(QWidget* parent = 0);
    QWidget* m_pMenuBarContent;
    QHBoxLayout* p_MenuLayout;
signals:

public slots:
    //���Ͻ�������ť����һ���ۺ���
    void on_TiBar_Clicked();

protected:
    //��д����¼�
    virtual void mouseDoubleClickEvent(QMouseEvent* event);// ˫�����������н�������/��ԭ
    virtual void mouseMoveEvent(QMouseEvent* event)override;// �����������϶�
    virtual bool eventFilter(QObject* obj, QEvent* event);// ���ý��������ͼ��

private:
    //void updateMaximize();// ���/��ԭ
    void paintEvent(QPaintEvent* event) override;
    //bool winEvent(MSG* message, long* result) override;

    //�Զ���������ؼ�
    QLabel* TiBar_pIconLabel;               //���Ͻ�ͼ��label
    QLabel* TiBar_pTitleLabel;              //�м������label
    QLabel* m_pTitleLabel2;                 //�±�����label

    ToolButton* TiBar_pMinimizeBtn;        //���Ͻ���С���ײ���������ť,��С��
    ToolButton* TiBar_pMaximizeBtn;        //���ϽǷŴ���С��ť
    ToolButton* TiBar_pCloseBtn;           //���Ͻǹرհ�ť

    QPushButton* m_pSearchBtn;

    QLineEdit* m_pSearchLine;//������

    bool IsMaximaze;
    void ShowNormal2(QWidget* AmWindow, bool IsBnt = true);
};


class FramelessWidget : public QWidget
{
    Q_OBJECT

public:
    FramelessWidget(QWidget* parent = 0);
    ~FramelessWidget();
    void setMarginWidth(const int&);           //�����������ڽ����Ե���Χ���϶��ı�����С
    void serResizable(bool);                    //�����Ƿ�����϶��ı��С

protected:
    bool winEvent(MSG* message, long* result)override;

private:
    int m_iMarginWidth;
    bool m_bCanResize;
};