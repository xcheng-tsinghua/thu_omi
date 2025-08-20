#include "WindowTitle.h"
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <qt_windows.h>
#include <QPainter>
#include <QDebug>
#include "qdesktopwidget.h"
#include "FMainWindow.h"
#include "windowsx.h"

extern FMainWindow* gWindow;

WindowTitle::WindowTitle(QWidget* parent) :
    QWidget(parent)
{
    //this->setStyleSheet("background-color: rgb(204, 213, 240);");

    IsMaximaze = true;
    m_pMenuBarContent = new QWidget;
    p_MenuLayout = new QHBoxLayout;
    p_MenuLayout->setSpacing(0);
    p_MenuLayout->setContentsMargins(0, 0, 0, 0);
    m_pMenuBarContent->setLayout(p_MenuLayout);

    QWidget* p_TitleLabelContent = new QWidget;
    QHBoxLayout* p_TitleLabelLayout = new QHBoxLayout;
    p_TitleLabelLayout->setContentsMargins(13, 0, 0, 0);
    p_TitleLabelLayout->setSpacing(20);
    p_TitleLabelContent->setLayout(p_TitleLabelLayout);
    
    m_pSearchLine = new QLineEdit;
    m_pSearchLine->setPlaceholderText(QString::fromLocal8Bit("����(Ctrl+Q)"));
    m_pSearchLine->setMaximumSize(275, 27);
    m_pSearchLine->setFixedSize(275, 27);
    //m_pSearchLine->setGeometry(0, 0, 275, 27);
    m_pSearchLine->setFont(QFont("Microsoft YaHei", 9, QFont::Normal));
    m_pSearchBtn = new QPushButton;
    m_pSearchBtn->setIcon(QIcon("../icons/Search.png"));
    m_pSearchBtn->setFlat(true);
    m_pSearchBtn->setStyleSheet("QPushButton{background: transparent;}");
    QHBoxLayout* lay = new QHBoxLayout;
    lay->addWidget(m_pSearchBtn);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->setAlignment(Qt::AlignRight);
    m_pSearchLine->setLayout(lay);
    m_pSearchLine->setFrame(false);
    m_pSearchLine->setStyleSheet("QLineEdit{ background-color: rgb(252,252,252); border:1px solid rgb(189,196,209);}QLineEdit:focus{background-color: rgb(255,255,255);border:1px solid rgb(64,80,141);}"); // ������ʽ
    m_pSearchLine->setTextMargins(0, 0, 0, 0);

    p_TitleLabelLayout->addWidget(m_pSearchLine);

    TiBar_pIconLabel = new QLabel;
    TiBar_pTitleLabel = new QLabel;

    m_pTitleLabel2 = new QLabel;

    //ʹ������
    //m_pTitleLabel2->setStyleSheet("color:rgb(64,86,141);background-color:rgb(217,224,248);");
    //m_pTitleLabel2->setText(QString::fromLocal8Bit("   Thu-OMI   "));
    //m_pTitleLabel2->setFont(QFont("Microsoft YaHei", -1, QFont::Bold));
    //m_pTitleLabel2->setFixedHeight(31);

    //ʹ��ͼƬ
    QPixmap iconForName("../icons/NameLabel.png");
    m_pTitleLabel2->setPixmap(iconForName);
    m_pTitleLabel2->setFixedSize(88, 31);

    p_TitleLabelLayout->addWidget(m_pTitleLabel2);
    //TiBar_pTitleLabel->setStyleSheet("color:rgb(138,27,255);");//������ɫ

    QLabel* MarginLabel = new QLabel;

    TiBar_pMinimizeBtn = new ToolButton("WinMin");
    TiBar_pMaximizeBtn = new ToolButton("WinNormal");
    TiBar_pCloseBtn = new ToolButton("WinClose");

    TiBar_pIconLabel->setScaledContents(true);  //����ͼƬ��ʾ���ʴ�С

    TiBar_pTitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);//�����Ա��洰��С������Ĭ�ϲ�����Ϊ��
    MarginLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);//�����Ա��洰��С������Ĭ�ϲ�����Ϊ��

    TiBar_pTitleLabel->setFixedHeight(41);

    QPixmap icon("../icons/WindowIcon.png");
    TiBar_pIconLabel->setPixmap(icon);
    TiBar_pIconLabel->setFixedSize(39, 26);

    TiBar_pTitleLabel->setFont(QFont("Times New Roman", 14, QFont::Normal));
    MarginLabel->setFont(QFont("Times New Roman", 10, QFont::Normal));
    MarginLabel->setText(" ");
    p_MenuLayout->addWidget(MarginLabel);

    TiBar_pTitleLabel->setObjectName("whiteLabel"); //�����Ա���˶�������ơ�
    TiBar_pMinimizeBtn->setObjectName("minimizeButton");
    TiBar_pMaximizeBtn->setObjectName("maximizeButton");
    TiBar_pCloseBtn->setObjectName("closeButton");

    TiBar_pMinimizeBtn->setToolTip(QString::fromLocal8Bit("��С��")); //���ͣ�����������ʾ
    TiBar_pMaximizeBtn->setToolTip(QString::fromLocal8Bit("���/��ԭ"));
    TiBar_pCloseBtn->setToolTip(QString::fromLocal8Bit("�ر�"));

    QHBoxLayout* pLayout = new QHBoxLayout(this);
    pLayout->addWidget(TiBar_pIconLabel);
    //pLayout->addWidget(TiBar_pTitleLabel);
    pLayout->addWidget(m_pMenuBarContent, 0, Qt::AlignLeft | Qt::AlignVCenter);
    pLayout->addWidget(p_TitleLabelContent, 1, Qt::AlignLeft | Qt::AlignVCenter);

    pLayout->addWidget(TiBar_pMinimizeBtn);
    pLayout->addWidget(TiBar_pMaximizeBtn);
    pLayout->addWidget(TiBar_pCloseBtn);
    pLayout->setSpacing(0);//���ÿؼ�֮��ļ��
    pLayout->setContentsMargins(0, 0, 0, 0);//�����������µı߾�
    pLayout->setMargin(0);
    this->setLayout(pLayout);
    //this->setWindowState(Qt::WindowMaximized);

    connect(TiBar_pMinimizeBtn, SIGNAL(clicked()), this, SLOT(on_TiBar_Clicked()));
    connect(TiBar_pMaximizeBtn, SIGNAL(clicked()), this, SLOT(on_TiBar_Clicked()));
    connect(TiBar_pCloseBtn, SIGNAL(clicked()), this, SLOT(on_TiBar_Clicked()));
}
/*  ���������Ͻ�������ť�Ĳۺ���*/
void WindowTitle::on_TiBar_Clicked()
{
    QPushButton* pBtn = qobject_cast<QPushButton*>(sender());//��������źż���Ĳ��е��ã��򷵻�ָ�����źŵĶ����ָ��; ����������0.
    /*  ���ش˴���С�����Ĵ��ڣ������У�����ܾ��У�����ϵͳ��ܵ���һ�����ȴ���С������ �������С�����Ǵ��ڣ��򷵻ش���С��������*/
    QWidget* pWindow = this->window();
    if (pWindow->isWindow())//�������С�����Ƕ������ڣ��򷵻�true�����򷵻�false��
    {
        if (pBtn == TiBar_pMinimizeBtn)
        {
            pWindow->showMinimized();//��С������С��������Ϊͼ�ꡣ���ô˺�����Ӱ�촰�ڡ�
            IsMaximaze = false;

            gWindow->MinimizeAll();
        }
        else if (pBtn == TiBar_pMaximizeBtn)
        {
            //pWindow->isMaximized() ? pWindow->showNormal() : pWindow->showMaximized();//�Ŵ���С
            //pWindow->isMaximized() ? pWindow->showNormal() : pWindow->setGeometry(QApplication::desktop()->availableGeometry());//�Ŵ���С
            IsMaximaze ? ShowNormal2(pWindow) : pWindow->setGeometry(QApplication::desktop()->availableGeometry());//�Ŵ���С
            if (IsMaximaze)
                TiBar_pMaximizeBtn->m_iconName = "WinMax";
            
            else
                TiBar_pMaximizeBtn->m_iconName = "WinNormal";
            TiBar_pMaximizeBtn->setIcon(QIcon("../icons/" + TiBar_pMaximizeBtn->m_iconName + "Out.png"));
            IsMaximaze ? IsMaximaze = false : IsMaximaze = true;
        }
        else if (pBtn == TiBar_pCloseBtn)
        {
            gWindow->CloseAll();
            pWindow->close();//�رմ���
        }
    }
}

void WindowTitle::mouseDoubleClickEvent(QMouseEvent* event)
{
    QWidget* pWindow = this->window();
    pWindow->setGeometry(QApplication::desktop()->availableGeometry());
    TiBar_pMaximizeBtn->m_iconName = "WinNormal";
    TiBar_pMaximizeBtn->setIcon(QIcon("../icons/" + TiBar_pMaximizeBtn->m_iconName + "Out.png"));
    IsMaximaze = true;
}

void WindowTitle::mouseMoveEvent(QMouseEvent* event)
{
    if (ReleaseCapture())
    {
        QWidget* pWindow = this->window();
        if (pWindow->isWindow())
        {
            if (IsMaximaze)
            {
                ShowNormal2(pWindow, false);
            }

            IsMaximaze = false;
            TiBar_pMaximizeBtn->m_iconName = "WinMax";
            TiBar_pMaximizeBtn->setIcon(QIcon("../icons/" + TiBar_pMaximizeBtn->m_iconName + "Out.png"));

            SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
        }
    }
    event->ignore();
}

bool WindowTitle::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::WindowTitleChange:
    {
        QWidget* pWidget = qobject_cast<QWidget*>(obj);
        if (pWidget)
        {
            TiBar_pTitleLabel->setText(pWidget->windowTitle());
            return true;
        }
    }
    case QEvent::WindowIconChange:
    {
        QWidget* pWidget = qobject_cast<QWidget*>(obj);
        if (pWidget)
        {
            QIcon icon = pWidget->windowIcon();
            TiBar_pIconLabel->setPixmap(icon.pixmap(TiBar_pIconLabel->size()));
            return true;
        }
    }
    case QEvent::WindowStateChange:
    case QEvent::Resize:
        //updateMaximize();
        return true;
    default:
        return false;
    }
    return QWidget::eventFilter(obj, event);
}

//���ڴ�С�����ı�
//void WindowTitle::updateMaximize()
//{
//    QWidget* pWindow = this->window();
//    if (pWindow->isTopLevel())
//    {
//        bool bMaximize = pWindow->isMaximized();
//        if (bMaximize)
//        {
//            TiBar_pMaximizeBtn->setToolTip(tr("Restore"));
//            TiBar_pMaximizeBtn->setProperty("maximizeProperty", "restore");
//        }
//        else
//        {
//            TiBar_pMaximizeBtn->setProperty("maximizeProperty", "maximize");
//            TiBar_pMaximizeBtn->setToolTip(tr("Maximize"));
//        }
//        TiBar_pMaximizeBtn->setStyle(QApplication::style());
//    }
//}

void WindowTitle::paintEvent(QPaintEvent* event)
{
    //���ñ���ɫ;
    QPainter painter(this);
    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 0, 0);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(pathBack, QBrush(QColor(204, 213, 240)));

    // ��������󻯻��߻�ԭ�󣬴��ڳ��ȱ��ˣ��������ĳ���Ӧ��һ��ı�;
    // �����ȥm_windowBorderWidth ������Ϊ���ڿ��������˲�ͬ��ȵı߿�;
    // ��������б߿�����Ҫ����m_windowBorderWidth��ֵ������m_windowBorderWidthĬ��Ϊ0;
    if (this->width() != (this->parentWidget()->width() - 0))
    {
        this->setFixedWidth(this->parentWidget()->width() - 0);
    }
    QWidget::paintEvent(event);
}

void WindowTitle::ShowNormal2(QWidget* AmWindow, bool IsBnt)
{
    if (IsBnt)
    {
        AmWindow->setGeometry(200, 70, 0.7 * QApplication::desktop()->availableGeometry().width(), 0.7 * QApplication::desktop()->availableGeometry().height());
    }
    else
    {
        QPoint GloPt = cursor().pos();
        QPoint LocPt = this->mapFromGlobal(GloPt);

        AmWindow->setGeometry(QRect(GloPt.x() - 0.7 * LocPt.x(), GloPt.y() - LocPt.y(), 0.7 * QApplication::desktop()->availableGeometry().width(), 0.7 * QApplication::desktop()->availableGeometry().height()));
    }
}

//bool WindowTitle::winEvent(MSG* message, long* result)
//{
//    //Q_UNUSED(eventType);
//    MSG* msg = message;
//    switch (msg->message) {
//    case WM_NCHITTEST://����ƶ���Ϣ
//        int xPos = GET_X_LPARAM(msg->lParam) - this->frameGeometry().x();//��������������
//        int yPos = GET_Y_LPARAM(msg->lParam) - this->frameGeometry().y();
//        if (this->geometry().contains(xPos, yPos)) //������(α)
//            *result = HTCAPTION;//POINT�ص���䣬ͨ��resultָ�뷵��--------------------------------<-
//        else //�������ֲ�����������false�����������¼�����������
//            return false;
//        return true;
//    }
//    return false; //�˴�����false�����������¼�����������
//}

//WidgetPar::WidgetPar(QWidget* parent) :
//    QWidget(parent)
//{
//    //setStyleSheet("border:1px solid rgb(32, 61, 119);");
//}
//
//void WidgetPar::paintEvent(QPaintEvent* event)
//{
//    //���ñ���ɫ;
//    QPainter painter(this);
//    QPainterPath pathBack;
//    pathBack.setFillRule(Qt::WindingFill);
//    pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 0, 0);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.fillPath(pathBack, QBrush(QColor(247, 249, 254)));
//
//    // ��������󻯻��߻�ԭ�󣬴��ڳ��ȱ��ˣ��������ĳ���Ӧ��һ��ı�;
//    // �����ȥm_windowBorderWidth ������Ϊ���ڿ��������˲�ͬ��ȵı߿�;
//    // ��������б߿�����Ҫ����m_windowBorderWidth��ֵ������m_windowBorderWidthĬ��Ϊ0;
//    //if (this->width() != (this->parentWidget()->width() - 0))
//    //{
//    //    this->setFixedWidth(this->parentWidget()->width() - 0);
//    //}
//    QWidget::paintEvent(event);
//}

//bool WidgetPar::winEvent(MSG* message, long* result)
//{
//    int m_iMarginWidth = 5;
//    bool m_bCanResize = true;
//
//    switch (message->message)
//    {
//    case WM_NCHITTEST:
//        int iPosX = GET_X_LPARAM(message->lParam) - this->frameGeometry().x();//���λ��x
//        int iPosY = GET_Y_LPARAM(message->lParam) - this->frameGeometry().y();//���λ��y
//
//        *result = HTCAPTION;
//
//        if (!m_bCanResize)
//        {
//            return true;//����Ϊ�棬��ʱ�Կ��϶������޷��ı��С
//        }
//
//        if (iPosX >= 0 && iPosX <= m_iMarginWidth)
//        {
//            *result = HTLEFT;//��
//            int iHeight = this->height();
//            if (iPosY >= 0 && iPosY <= m_iMarginWidth)
//            {
//                *result = HTTOPLEFT;//����
//            }
//            if (iPosY >= iHeight - m_iMarginWidth && iPosY <= iHeight)
//            {
//                *result = HTBOTTOMLEFT;//����
//            }
//            return true;
//        }
//
//        int iWidth = this->width();
//        int iHeight = this->height();
//        if (iPosX >= iWidth - m_iMarginWidth && iPosX <= iWidth)
//        {
//            *result = HTRIGHT;//��
//            if (iPosY >= 0 && iPosY <= m_iMarginWidth)
//            {
//                *result = HTTOPRIGHT;//����
//                return true;
//            }
//            if (iPosY >= iHeight - m_iMarginWidth && iPosY <= iHeight)
//            {
//                *result = HTBOTTOMRIGHT;//����
//                return true;
//            }
//            return true;
//        }
//        if (iPosY >= 0 && iPosY <= m_iMarginWidth)
//        {
//            *result = HTTOP;//��
//            return true;
//        }
//        if (iPosY >= iHeight - m_iMarginWidth && iPosY <= iHeight)
//        {
//            *result = HTBOTTOM;//��
//            return true;
//        }
//        return true;
//    }
//    return false;
//}

FramelessWidget::FramelessWidget(QWidget* parent)
    : QWidget(parent)
{
    m_iMarginWidth = 3;
    m_bCanResize = true;
    //setGeometry(50, 50, 200, 200);
    setWindowFlags(Qt::FramelessWindowHint);
}

FramelessWidget::~FramelessWidget()
{

}

void FramelessWidget::setMarginWidth(const int& iWidth)
{
    m_iMarginWidth = iWidth;
}


void FramelessWidget::serResizable(bool bCanResize)
{
    m_bCanResize = bCanResize;
}

bool FramelessWidget::winEvent(MSG* message, long* result)
{
    switch (message->message)
    {
    case WM_NCHITTEST://����ƶ���Ϣ
        int iPosX = GET_X_LPARAM(message->lParam) - this->frameGeometry().x();//���λ��x
        int iPosY = GET_Y_LPARAM(message->lParam) - this->frameGeometry().y();//���λ��y

        *result = HTCAPTION;

        if (!m_bCanResize)
        {
            return true;//����Ϊ�棬��ʱ�Կ��϶������޷��ı��С
        }

        if (iPosX >= 0 && iPosX <= m_iMarginWidth)
        {
            *result = HTLEFT;//��
            int iHeight = this->height();
            if (iPosY >= 0 && iPosY <= m_iMarginWidth)
            {
                *result = HTTOPLEFT;//����
            }
            if (iPosY >= iHeight - m_iMarginWidth && iPosY <= iHeight)
            {
                *result = HTBOTTOMLEFT;//����
            }
            return true;
        }

        int iWidth = this->width();
        int iHeight = this->height();
        if (iPosX >= iWidth - m_iMarginWidth && iPosX <= iWidth)
        {
            *result = HTRIGHT;//��
            if (iPosY >= 0 && iPosY <= m_iMarginWidth)
            {
                *result = HTTOPRIGHT;//����
                return true;
            }
            if (iPosY >= iHeight - m_iMarginWidth && iPosY <= iHeight)
            {
                *result = HTBOTTOMRIGHT;//����
                return true;
            }
            return true;
        }
        if (iPosY >= 0 && iPosY <= m_iMarginWidth)
        {
            *result = HTTOP;//��
            return true;
        }
        if (iPosY >= iHeight - m_iMarginWidth && iPosY <= iHeight)
        {
            *result = HTBOTTOM;//��
            return true;
        }
        return true;
    }
    return false;
}