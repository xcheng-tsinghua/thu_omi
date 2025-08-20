#pragma once

#include <qpushbutton.h>
#include <QMouseEvent>
#include <QPainter>

class ToolButton : public QPushButton
{

public:

	explicit ToolButton(QString pic_name, QWidget *parent = 0);
	~ToolButton();
	void setMousePress(bool mouse_press);

protected:

	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	void mousePressEvent(QMouseEvent *event);

public:

	bool mouse_over; //鼠标是否移过
	bool mouse_press; //鼠标是否按下
	QString m_iconName;
};