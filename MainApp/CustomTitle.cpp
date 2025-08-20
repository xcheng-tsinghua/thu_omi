#include "CustomTitle.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QFile>
#include <QMouseEvent>

#define BUTTON_HEIGHT 20        // ��ť�߶�;
#define BUTTON_WIDTH 20         // ��ť���;
#define TITLE_HEIGHT 24         // �������߶�;

CustomTitle::CustomTitle(QWidget* parent)
    : QWidget(parent)
    , m_colorR(64)
    , m_colorG(86)
    , m_colorB(141)
    , m_isPressed(false)
    , m_buttonType(MIN_MAX_BUTTON)
    //, m_windowBorderWidth(0)
    , m_isTransparent(false)
{
    // ��ʼ��;
    initControl();
    initConnections();
}

CustomTitle::~CustomTitle()
{

}

// ��ʼ���ؼ�;
void CustomTitle::initControl()
{
    m_pIcon = new QLabel;
    m_pTitleContent = new QLabel;

    m_pButtonMin = new QPushButton;
    m_pButtonRestore = new QPushButton;
    m_pButtonMax = new QPushButton;
    m_pButtonClose = new QPushButton;

    m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

    m_pTitleContent->setObjectName("TitleContent");
    m_pButtonMin->setObjectName("ButtonMin");
    m_pButtonRestore->setObjectName("ButtonRestore");
    m_pButtonMax->setObjectName("ButtonMax");
    m_pButtonClose->setObjectName("ButtonClose");

    m_pButtonMin->setToolTip(QString::fromLocal8Bit("��С��"));
    m_pButtonRestore->setToolTip(QString::fromLocal8Bit("��ԭ"));
    m_pButtonMax->setToolTip(QString::fromLocal8Bit("���"));
    m_pButtonClose->setToolTip(QString::fromLocal8Bit("�ر�"));

    QHBoxLayout* mylayout = new QHBoxLayout(this);
    mylayout->addWidget(m_pIcon);
    mylayout->addWidget(m_pTitleContent);

    mylayout->addWidget(m_pButtonMin);
    mylayout->addWidget(m_pButtonRestore);
    mylayout->addWidget(m_pButtonMax);
    mylayout->addWidget(m_pButtonClose);

    mylayout->setContentsMargins(0, 0, 5, 0);
    //mylayout->setSpacing(0);
    mylayout->setMargin(0);

    m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->setFixedHeight(TITLE_HEIGHT);
    this->setWindowFlags(Qt::FramelessWindowHint);

    //QPainter painter(this);// ����QPainterһ������
    //QPen pen;
    //painter.setPen(QPen(QColor(79, 79, 79)));
    //QPointF points1[5] = { QPointF(2, 2), QPointF(0, 9999), QPointF(9999, 24), QPointF(0, 24),QPointF(0, 0) };
    //painter.drawPolyline(points1, 5);
}

// �źŲ۵İ�;
void CustomTitle::initConnections()
{
    connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));
    connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked()));
    connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));
    connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}

// ���ñ���������ɫ,��paintEvent�¼��н��л��Ʊ���������ɫ;
// �ڹ��캯���и���Ĭ��ֵ�������ⲿ������ɫֵ�ı����������ɫ;
void CustomTitle::setBackgroundColor(int r, int g, int b, bool isTransparent)
{
    m_colorR = r;
    m_colorG = g;
    m_colorB = b;
    m_isTransparent = isTransparent;
    // ���»��ƣ�����paintEvent�¼���;
    update();
}

// ���ñ�����ͼ��;
void CustomTitle::setTitleIcon(QString filePath, QSize IconSize)
{
    QPixmap titleIcon(filePath);
    m_pIcon->setPixmap(titleIcon.scaled(IconSize));
}

// ���ñ�������;
void CustomTitle::setTitleContent(QString titleContent, int titleFontSize)
{
    // ���ñ��������С;
    QFont font = m_pTitleContent->font();
    font.setPointSize(titleFontSize);
    m_pTitleContent->setFont(font);
    // ���ñ�������;
    m_pTitleContent->setText(titleContent);
    m_titleContent = titleContent;
}

// ���ñ���������;
void CustomTitle::setTitleWidth(int width)
{
    this->setFixedWidth(width);
}

// ���ñ������ϰ�ť����;
// ���ڲ�ͬ���ڱ������ϵİ�ť����һ�������Կ����Զ���������еİ�ť;
// �����ṩ���ĸ���ť���ֱ�Ϊ��С������ԭ����󻯡��رհ�ť�������Ҫ������ť�������������;
void CustomTitle::setButtonType(ButtonType buttonType)
{
    m_buttonType = buttonType;

    switch (buttonType)
    {
    case MIN_BUTTON:
    {
        m_pButtonRestore->setVisible(false);
        m_pButtonMax->setVisible(false);
    }
    break;
    case MIN_MAX_BUTTON:
    {
        m_pButtonRestore->setVisible(false);
    }
    break;
    case ONLY_CLOSE_BUTTON:
    {
        m_pButtonMin->setVisible(false);
        m_pButtonRestore->setVisible(false);
        m_pButtonMax->setVisible(false);
    }
    break;
    default:
        break;
    }
}

// ���ñ������еı����Ƿ���Զ�����������Ƶ�Ч��;
// һ������±������еı��������ǲ������ģ����Ǽ�Ȼ�Զ���Ϳ��Լ���Ҫ�����ô��ƾ���ô��O(��_��)O��
void CustomTitle::setTitleRoll()
{
    connect(&m_titleRollTimer, SIGNAL(timeout()), this, SLOT(onRollTitle()));
    m_titleRollTimer.start(200);
}

// ���ô��ڱ߿���;
void CustomTitle::setWindowBorderWidth(int borderWidth)
{
    //m_windowBorderWidth = borderWidth;
}

// ���洰�����ǰ���ڵ�λ���Լ���С;
void CustomTitle::saveRestoreInfo(const QPoint point, const QSize size)
{
    m_restorePos = point;
    m_restoreSize = size;
}

// ��ȡ�������ǰ���ڵ�λ���Լ���С;
void CustomTitle::getRestoreInfo(QPoint& point, QSize& size)
{
    point = m_restorePos;
    size = m_restoreSize;
}

// ���Ʊ���������ɫ;
void CustomTitle::paintEvent(QPaintEvent* event)
{
    // �Ƿ����ñ���͸��;
    if (!m_isTransparent)
    {
        //���ñ���ɫ;
        QPainter painter(this);
        QPainterPath pathBack;
        pathBack.setFillRule(Qt::WindingFill);
        pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.fillPath(pathBack, QBrush(QColor(m_colorR, m_colorG, m_colorB)));
    }

    // ��������󻯻��߻�ԭ�󣬴��ڳ��ȱ��ˣ��������ĳ���Ӧ��һ��ı�;
    // �����ȥm_windowBorderWidth ������Ϊ���ڿ��������˲�ͬ��ȵı߿�;
    // ��������б߿�����Ҫ����m_windowBorderWidth��ֵ������m_windowBorderWidthĬ��Ϊ0;
    if (this->width() != (this->parentWidget()->width() - 0/*m_windowBorderWidth*/))
    {
        this->setFixedWidth(this->parentWidget()->width() - 0/*m_windowBorderWidth*/);
    }
    QWidget::paintEvent(event);
}

// ˫����Ӧ�¼�����Ҫ��ʵ��˫��������������󻯺���С������;
void CustomTitle::mouseDoubleClickEvent(QMouseEvent* event)
{
    // ֻ�д�����󻯡���ԭ��ťʱ˫������Ч;
    if (m_buttonType == MIN_MAX_BUTTON)
    {
        // ͨ����󻯰�ť��״̬�жϵ�ǰ�����Ǵ�����󻯻���ԭʼ��С״̬;
        // ����ͨ���������ñ�������ʾ��ǰ����״̬;
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

// ����ͨ��mousePressEvent��mouseMoveEvent��mouseReleaseEvent�����¼�ʵ��������϶��������ƶ����ڵ�Ч��;
void CustomTitle::mousePressEvent(QMouseEvent* event)
{
    if (m_buttonType == MIN_MAX_BUTTON)
    {
        // �ڴ������ʱ��ֹ�϶�����;
        if (m_pButtonMax->isVisible())
        {
            m_isPressed = true;
            m_startMovePos = event->globalPos();
        }
    }
    else
    {
        m_isPressed = true;
        m_startMovePos = event->globalPos();
    }

    return QWidget::mousePressEvent(event);
}

void CustomTitle::mouseMoveEvent(QMouseEvent* event)
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

void CustomTitle::mouseReleaseEvent(QMouseEvent* event)
{
    m_isPressed = false;
    return QWidget::mouseReleaseEvent(event);
}

// ����Ϊ��ť������Ӧ�Ĳ�;
void CustomTitle::onButtonMinClicked()
{
    emit signalButtonMinClicked();
}

void CustomTitle::onButtonRestoreClicked()
{
    m_pButtonRestore->setVisible(false);
    m_pButtonMax->setVisible(true);
    emit signalButtonRestoreClicked();
}

void CustomTitle::onButtonMaxClicked()
{
    m_pButtonMax->setVisible(false);
    m_pButtonRestore->setVisible(true);
    emit signalButtonMaxClicked();
}

void CustomTitle::onButtonCloseClicked()
{
    emit signalButtonCloseClicked();
}

// �÷�����Ҫ���ñ������еı�����ʾΪ������Ч��;
void CustomTitle::onRollTitle()
{
    static int nPos = 0;
    QString titleContent = m_titleContent;
    // ����ȡ��λ�ñ��ַ�����ʱ����ͷ��ʼ;
    if (nPos > titleContent.length())
        nPos = 0;

    m_pTitleContent->setText(titleContent.mid(nPos));
    nPos++;
}