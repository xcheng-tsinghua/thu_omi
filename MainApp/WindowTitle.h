#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "windows.h"
#include "tool_button.h"
#include "qlayout.h"
#include "qlineedit.h"
/*
定义一个新的QWidget类,将标题栏和UI界面都放到这个类里面,定义这个类是为了方便改变主题
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
自定义的标题栏类
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
    //右上角三个按钮共用一个槽函数
    void on_TiBar_Clicked();

protected:
    //重写鼠标事件
    virtual void mouseDoubleClickEvent(QMouseEvent* event);// 双击标题栏进行界面的最大化/还原
    virtual void mouseMoveEvent(QMouseEvent* event)override;// 进行鼠界面的拖动
    virtual bool eventFilter(QObject* obj, QEvent* event);// 设置界面标题与图标

private:
    //void updateMaximize();// 最大化/还原
    void paintEvent(QPaintEvent* event) override;
    //bool winEvent(MSG* message, long* result) override;

    //自定义标题栏控件
    QLabel* TiBar_pIconLabel;               //左上角图标label
    QLabel* TiBar_pTitleLabel;              //中间标题栏label
    QLabel* m_pTitleLabel2;                 //新标题栏label

    ToolButton* TiBar_pMinimizeBtn;        //右上角缩小到底部任务栏按钮,最小化
    ToolButton* TiBar_pMaximizeBtn;        //右上角放大缩小按钮
    ToolButton* TiBar_pCloseBtn;           //右上角关闭按钮

    QPushButton* m_pSearchBtn;

    QLineEdit* m_pSearchLine;//搜索框

    bool IsMaximaze;
    void ShowNormal2(QWidget* AmWindow, bool IsBnt = true);
};


class FramelessWidget : public QWidget
{
    Q_OBJECT

public:
    FramelessWidget(QWidget* parent = 0);
    ~FramelessWidget();
    void setMarginWidth(const int&);           //设置鼠标可以在界面边缘多大范围内拖动改变界面大小
    void serResizable(bool);                    //设置是否可以拖动改变大小

protected:
    bool winEvent(MSG* message, long* result)override;

private:
    int m_iMarginWidth;
    bool m_bCanResize;
};