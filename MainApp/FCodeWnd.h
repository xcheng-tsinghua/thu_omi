/*	
\brief	A window class to define the inspection code window.
\author: Jacky
\date    2017-03
*/
#ifndef F_INSPECTCODE_WND_H
#define F_INSPECTCODE_WND_H
#pragma once
#include "../External/fkernel/Include/FKernelDefines.h"
#include <Qtgui>
#include <QDockWidget>

class FHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	FHighlighter(QTextDocument *parent = 0);
	void	setPosAndLength( int pos, int length );
	void	startHighlight() {m_bHighlight = true;}
	void	stopHightlight() {m_bHighlight = false;}

protected:
	void highlightBlock(const QString &text);

private:
	int	m_startPos;
	int m_length;
	bool	m_bHighlight;
	//QVector<FHighlightingRule> m_highlightingRules;
	QTextCharFormat m_keywordFormat;		///e.g. Inputs, Priority
	QTextCharFormat m_commandFormat;		///e.g. Rail, Create, etc.
	QTextCharFormat m_testFormat;
	// QTextCharFormat m_conditionFormat;	///condition part of a rule
	// QTextCharFormat m_semanticFormat;		///semantic part of a rule
};

class FMainWindow;
class FTestDockWnd: public QDockWidget
{
public:
	FTestDockWnd(QWidget* parent=NULL);
};

class FCodeWnd : public QDockWidget
{
	Q_OBJECT
public:
	FCodeWnd(QWidget* parent=NULL);
	~FCodeWnd(void);
	////////////////////////// Accessing methods ///////////////////////////
	void	setTextEdit( QString text );
	void	clearTextEdit();
	QTextEdit*	getTextEdit() {return m_pTextEdit;}
	QTextDocument* getTextDom() {return m_pTextEdit->document();}
	void	setHighlightSections(const QList<QTextEdit::ExtraSelection>& sections, 
		QTextCursor	cursor);
	void	setHighlightRegion(QVector<int> region);
	void	clearHighlight();


protected:
	void	LoadFile(const QString &fileName);	//load NC code file

public slots:
	void	onPauseBtn();		///triggered when the Pause button is clicked
	void	onHighlight();

signals:
	void	highlightSignal();


protected:
	QTextEdit*		m_pTextEdit;		///show the NC code
	FHighlighter*	m_pHighlighter;		///highlight rules in m_pRuleEdit
	QList<QTextEdit::ExtraSelection> m_highlightSections;
	QVector<int>	m_highlightRegion;
};

#endif