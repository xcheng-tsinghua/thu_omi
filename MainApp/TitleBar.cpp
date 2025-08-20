#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include "TitleBar.h"
#include <QPainter>
#include "FMainWindow.h"
#include "FTreeWnd.h"

extern QString gDockTitleColor;
extern FMainWindow* gWindow;

TitleBar::TitleBar(QWidget* parent)
    : QWidget(parent)
{
    m_buttonType = MIN_MAX_BUTTON;

    setFixedHeight(24);
    m_pIconLabel = new QLabel;
    m_pTitleLabel = new QLabel;

    m_pButtonMax = new QPushButton;
    m_pButtonMin = new QPushButton;
    m_pButtonRestore = new QPushButton;
    m_pButtonClose = new QPushButton;

    m_pButtonMax->setStyleSheet(
    "border:none"
    );
    m_pButtonMax->setFlat(true);

    m_pButtonRestore->setStyleSheet(
        "border:none"
    );
    m_pButtonRestore->setFlat(true);

    m_pIconLabel->setFixedSize(20, 20);
    m_pIconLabel->setScaledContents(true);

    m_pButtonMax->setFixedSize(20, 20);
    m_pButtonMin->setFixedSize(20, 20);
    m_pButtonRestore->setFixedSize(20, 20);
    m_pButtonClose->setFixedSize(20, 20);

    m_pTitleLabel->setObjectName("whiteLabel");
    m_pButtonMax->setObjectName("maximizeButton");
    m_pButtonMin->setObjectName("minimumButton");
    m_pButtonRestore->setObjectName("resetButton");
    m_pButtonClose->setObjectName("closeButton");

    m_pButtonMax->setToolTip("Maximize");
    m_pButtonMin->setToolTip("Minimize");
    m_pButtonRestore->setToolTip("Reset");
    m_pButtonClose->setToolTip("Close");

    m_pTitleLabel->setContentsMargins(5, 0, 0, 0);
    m_pTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_pTitleLabel->setText(QString::fromLocal8Bit("三维模型"));
    //m_pTitleLabel->setFont(QFont("Microsoft YaHei"));
    QPalette tmpPa;
    tmpPa.setColor(QPalette::WindowText, Qt::white);
    m_pTitleLabel->setPalette(tmpPa);

    m_pButtonMax->setIcon(QIcon("../icons/CenterWndMax.png"));
    m_pButtonRestore->setIcon(QIcon("../icons/CenterWndReset.png"));

    QHBoxLayout* mainWidgetLayout = new QHBoxLayout(this);
    QWidget* mainWidget = new QWidget;
    QHBoxLayout* pLayout = new QHBoxLayout;

    mainWidgetLayout->addWidget(mainWidget);
    mainWidget->setLayout(pLayout);
    mainWidgetLayout->setMargin(0);
    pLayout->setContentsMargins(7, 0, 0, 0);
    pLayout->setSpacing(0);
    mainWidget->setStyleSheet("QWidget{background-color:" + gDockTitleColor + "}");
    pLayout->addWidget(m_pIconLabel);
    pLayout->addWidget(m_pTitleLabel);
    pLayout->addStretch();

    //pLayout->addWidget(m_pButtonMin);
    pLayout->addWidget(m_pButtonMax);
    pLayout->addWidget(m_pButtonRestore);
    //pLayout->addWidget(m_pButtonClose);

    m_pButtonMax->setVisible(false);

    connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));
    connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked()));
    connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));
    connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}

TitleBar::~TitleBar()
{

}

// 绘制标题栏背景色;
void TitleBar::paintEvent(QPaintEvent* event)
{
    //设置背景色;
    QPainter painter(this);
    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 0, 0);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(pathBack, QBrush(QColor(64, 86, 141)));

    // 当窗口最大化或者还原后，窗口长度变了，标题栏的长度应当一起改变;
    // 这里减去m_windowBorderWidth ，是因为窗口可能设置了不同宽度的边框;
    // 如果窗口有边框则需要设置m_windowBorderWidth的值，否则m_windowBorderWidth默认为0;
    if (this->width() != (this->parentWidget()->width() - 0))
    {
        this->setFixedWidth(this->parentWidget()->width() - 0);
    }
    QWidget::paintEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    // 只有存在最大化、还原按钮时双击才有效;
    if (m_buttonType == MIN_MAX_BUTTON)
    {
        // 通过最大化按钮的状态判断当前窗口是处于最大化还是原始大小状态;
        // 或者通过单独设置变量来表示当前窗口状态;
        if (m_pButtonMax->isVisible())
        {
            onButtonMaxClicked();
        }
        else
        {
            onButtonRestoreClicked();
        }
    }

    return QWidget::mouseDoubleClickEvent(event);
}

// 以下为按钮操作响应的槽;
void TitleBar::onButtonMinClicked()
{
    emit signalButtonMinClicked();
}

void TitleBar::onButtonRestoreClicked()
{
    m_pButtonRestore->setVisible(false);
    m_pButtonMax->setVisible(true);
    emit signalButtonRestoreClicked();
}

void TitleBar::onButtonMaxClicked()
{
    m_pButtonMax->setVisible(false);
    m_pButtonRestore->setVisible(true);
    emit signalButtonMaxClicked();
}

void TitleBar::onButtonCloseClicked()
{
    emit signalButtonCloseClicked();
}

// 以下通过mousePressEvent、mouseMoveEvent、mouseReleaseEvent三个事件实现了鼠标拖动标题栏移动窗口的效果;
void TitleBar::mousePressEvent(QMouseEvent* event)
{
    //只要移动了窗口，就将窗口置为自由窗口状态
    m_pButtonRestore->setVisible(false);
    m_pButtonMax->setVisible(true);

    if (m_buttonType == MIN_MAX_BUTTON)
    {
        m_isPressed = true;
        m_startMovePos = event->globalPos();
    }
    else
    {
        m_isPressed = true;
        m_startMovePos = event->globalPos();
    }

    return QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isPressed)
    {
        QPoint movePoint = event->globalPos() - m_startMovePos;
        QPoint widgetPos = this->parentWidget()->pos();
        m_startMovePos = event->globalPos();
        this->parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
    }
    return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event)
{
    m_isPressed = false;
    return QWidget::mouseReleaseEvent(event);
}