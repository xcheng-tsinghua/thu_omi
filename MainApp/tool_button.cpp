#include "tool_button.h"

ToolButton::ToolButton(QString pic_name, QWidget *parent)
	:QPushButton(parent)
{
	m_iconName = pic_name;
	//设置文本颜色
	QPalette text_palette = palette();
	text_palette.setColor(QPalette::ButtonText, QColor(230, 230, 230));
	setPalette(text_palette);

	//设置文本粗体
	QFont &text_font = const_cast<QFont &>(font());
	text_font.setWeight(QFont::Bold);

	//设置图标
	setIcon(QIcon("../icons/" + m_iconName + "Out.png"));
	setIconSize(this->size());

	setStyleSheet(
		"QPushButton\
		{\
			border: none;\
			width: 58px;\
			height: 41px;\
		}"
	);

	mouse_over = false;
	mouse_press = false;
}

ToolButton::~ToolButton()
{

}

void ToolButton::enterEvent(QEvent *)
{
	mouse_over = true;
	setIcon(QIcon("../icons/" + m_iconName + "In.png"));
}

void ToolButton::leaveEvent(QEvent *)
{
	mouse_over = false;
	setIcon(QIcon("../icons/" + m_iconName + "Out.png"));
}

void ToolButton::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		emit clicked();
	}
}

void ToolButton::setMousePress(bool mouse_press)
{
	this->mouse_press = mouse_press;
	update();
}