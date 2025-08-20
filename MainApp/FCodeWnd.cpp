#include "FMainWindow.h"
#include "FCodeWnd.h"
#include <iostream>

using namespace std;
extern QString gDockTitleColor;

FHighlighter::FHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	//FHighlightingRule rule;

	m_startPos	=	0;
	m_length	=	0;
	m_bHighlight	=	false;
	m_testFormat.setForeground(Qt::darkBlue);
	m_testFormat.setFontWeight(QFont::Bold);
	m_testFormat.setFontPointSize(12);
	//m_keywordFormat.setForeground(Qt::darkBlue);
	//m_keywordFormat.setFontWeight(QFont::Bold);
	//m_keywordFormat.setFontPointSize(12);
	//QStringList keywordPatterns;
	//keywordPatterns << "\\bInputs\\b" << "\\bPriority\\b";
	//foreach (const QString &pattern, keywordPatterns) {
	//	rule.pattern = QRegExp(pattern);
	//	rule.format = m_keywordFormat;
	//	m_highlightingRules.append(rule);
	//}
	//m_commandFormat.setFontItalic(true);
	//m_commandFormat.setForeground(Qt::blue);
	//rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
	//rule.format = m_commandFormat;
	//m_highlightingRules.append(rule);
}

void FHighlighter::highlightBlock(const QString &text)
{
	//int nsize = m_highlightingRules.size();
	//for(int i=0; i<nsize; i++) 
	//{
	//	QRegExp expression(m_highlightingRules[i].pattern);
	//	int index = expression.indexIn(text);
	//	while (index >= 0) 
	//	{
	//		int length = expression.matchedLength();
	//		setFormat(index, length, m_highlightingRules[i].format);
	//		index = expression.indexIn(text, index + length);
	//	}
	//}
	if (m_bHighlight)
	{
		cout	<<	text.toLocal8Bit().data();
		setFormat(0, text.size(), m_testFormat);
	}
}

void FHighlighter::setPosAndLength( int pos, int length )
{
	m_startPos	=	pos;
	m_length	=	length;
}

//////////////////////////////////////////////////////////////////////////
FTestDockWnd::FTestDockWnd(QWidget* parent)
{

}

FCodeWnd::FCodeWnd(QWidget* parent)
	:QDockWidget(parent)
{
	this->setAttribute(Qt::WA_StyledBackground, true);

	//setWindowTitle(QObject::tr("Code"));
	setWindowTitle(QString::fromLocal8Bit("NC����"));
	//container widget
	QWidget* dockWidgetContents = new QWidget();
	dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
	//QWidget* widget = new QWidget(dockWidgetContents);
	//widget->setObjectName(QString::fromUtf8("widget"));
	//widget->setGeometry(0, 0, 500, 500);
	m_pTextEdit = new QTextEdit(this);
	m_pHighlighter	= new FHighlighter(m_pTextEdit->document());
	QString dirPath = QApplication::applicationDirPath();
	QString path = dirPath+"/data/example_nc.mpf";
	if(QFile::exists(path))
		LoadFile(path.toLocal8Bit().data());

	//QPushButton* pImportBtn = new QPushButton(QString::fromLocal8Bit("����"));
	//QPushButton* pSaveBtn = new QPushButton(QObject::tr("Save"));
	//QPushButton* pStartBtn = new QPushButton(QObject::tr("Start"));
	//QPushButton* pPauseBtn = new QPushButton(QObject::tr("Pause"));
	//QPushButton* pResetBtn = new QPushButton(QObject::tr("Reset"));
	QGridLayout* mainGridLayout = new QGridLayout(dockWidgetContents);
	mainGridLayout->setObjectName(QString::fromUtf8("mainlayout"));
	mainGridLayout->setContentsMargins(0, 0, 0, 0);
	mainGridLayout->addWidget(m_pTextEdit, 0, 0, 6, 4);
	//mainGridLayout->addWidget(pImportBtn, 1, 4, 1, 1);
	//mainGridLayout->addWidget(pSaveBtn, 2, 4, 1, 1);
	//mainGridLayout->addWidget(pStartBtn, 3, 4, 1, 1);
	//mainGridLayout->addWidget(pPauseBtn, 4, 4, 1, 1);
	//mainGridLayout->addWidget(pResetBtn, 5, 4, 1, 1);

	setWidget(dockWidgetContents);
	//add signal-slots
	//QObject::connect(pPauseBtn,SIGNAL(clicked()), this, SLOT(onPauseBtn()));
	QObject::connect(this,SIGNAL(highlightSignal()), this, SLOT(onHighlight()));

	//style
	{
		//������
		this->setStyleSheet("QDockWidget::title"
			"{"
			"padding-left: 10px;"
			"padding-top: 4px;"
			"background:" + gDockTitleColor +
			"}"
			"QDockWidget"
			"{"
			"color: rgb(247,249,254);"//��������������ɫ
			"}"
		);

		//������
		dockWidgetContents->setStyleSheet(
			"QWidget"
			"{"
			"border: 1px solid rgb(232,232,236);"
			"background:rgb(247,249,254);"
			"}"
			"QScrollBar:vertical"
			"{"
			"width:22px;"
			"background:rgba(0,0,0,0%);"
			"margin:0px,0px,0px,0px;"
			"padding-top:18px;"
			"padding-bottom:18px;"
			"padding-right:5px;"
			"padding-left:5px;"
			"background:rgba(232,232,236,100%);"
			"}"
			// ���������˱����Բ
			"QScrollBar::handle:vertical"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,25%);"
			"min-height:20;"
			"max-width:10;"
			"}"
			// ���ŵ��������ϵ�ʱ����ɫ����
			"QScrollBar::handle:vertical:hover"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,50%);"
			"min-height:20;"
			"}"
			// ���Ӧ���������¼�ͷ�ģ�3.png���Ǽ�ͷ
			"QScrollBar::add-line:vertical"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollDown.png);"
			"subcontrol-position:bottom;"
			"}"
			// �����ϼ�ͷ
			"QScrollBar::sub-line:vertical"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollUp.png);"
			"subcontrol-position:top;"
			"}"
			// �����ŵ��¼�ͷ�ϵ�ʱ��
			"QScrollBar::add-line:vertical:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollDown2.png);"
			"subcontrol-position:bottom;"
			"}"
			// �����ŵ��¼�ͷ�ϵ�ʱ��
			"QScrollBar::sub-line:vertical:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollUp2.png);"
			"subcontrol-position:top;"
			"}"
			// ��������������ʱ������Ĳ��ֺ�����Ĳ���
			"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
			"{"
			"background:rgba(232,232,236,100%);"
			"}"

			//����
			"QScrollBar:horizontal"
			"{"
			"height:22px;"
			"background:rgba(0,0,0,0%);"
			"margin:0px,0px,0px,0px;"
			"padding-top:5px;"
			"padding-bottom:5px;"
			"padding-right:18px;"
			"padding-left:18px;"
			"background:rgba(232,232,236,100%);"
			"}"
			// ���������˱����Բ
			"QScrollBar::handle:horizontal"
			"{"
			"height:8px;"
			"background:rgba(0,0,0,25%);"
			"min-width:10;"
			"max-height:20;"
			"}"
			// ���ŵ��������ϵ�ʱ����ɫ����
			"QScrollBar::handle:horizontal:hover"
			"{"
			"height:8px;"
			"background:rgba(0,0,0,50%);"
			"min-width:20;"
			"}"
			// �������ͷ�ģ�3.png���Ǽ�ͷ
			"QScrollBar::sub-line:horizontal"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollLeft.png);"
			"subcontrol-position:left;"
			"}"
			// �����Ҽ�ͷ
			"QScrollBar::add-line:horizontal"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollRight.png);"
			"subcontrol-position:right;"
			"}"
			// �����ŵ����ͷ�ϵ�ʱ��
			"QScrollBar::sub-line:horizontal:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollLeft2.png);"
			"subcontrol-position:left;"
			"}"
			// �����ŵ��Ҽ�ͷ�ϵ�ʱ��
			"QScrollBar::add-line:horizontal:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollRight2.png);"
			"subcontrol-position:right;"
			"}"
			// ��������������ʱ������Ĳ��ֺ�����Ĳ���
			"QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal"
			"{"
			"background:rgba(232,232,236,100%);"
			"}"
		);
	}
}

void FCodeWnd::LoadFile(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Application"),
			tr("Cannot read file %1:\n%2.")
			.arg(fileName)
			.arg(file.errorString()));
		return;
	}
	QFileInfo info(fileName);
	QTextStream in(&file);
	m_pTextEdit->setPlainText(in.readAll());
//	SetCurrentFile(fileName);
}

void FCodeWnd::onPauseBtn()
{
	int i=0;
}

FCodeWnd::~FCodeWnd(void)
{
}

void FCodeWnd::setTextEdit( QString text )
{
	m_pTextEdit->setPlainText( text );
	m_pTextEdit->setReadOnly(true);
	//QTextDocument* pTextDoc = m_pTextEdit->document();
	//m_pTextEdit->verticalScrollBar()->setRange(1, pTextDoc->lineCount());
}

void FCodeWnd::setHighlightSections( const QList<QTextEdit::ExtraSelection>& sections, 
	QTextCursor	cursor)
{
	m_highlightSections = sections;
	m_pTextEdit->setExtraSelections(sections);
	/*if (m_pTextEdit->textCursor().blockNumber() > cursor.blockNumber())
	{
		m_pTextEdit->verticalScrollBar()->setValue(0);
	}*/
	m_pTextEdit->verticalScrollBar()->setValue(0);
	m_pTextEdit->setTextCursor(cursor);
}

void FCodeWnd::setHighlightRegion( QVector<int> region )
{
	m_highlightRegion = region;
	emit(highlightSignal());
}

void FCodeWnd::onHighlight()
{
	QList<QTextEdit::ExtraSelection> extraSelections;
	QTextDocument* pTextDoc = m_pTextEdit->document();
	for (int i = m_highlightRegion[0]-1; i < m_highlightRegion[1]; i++)
	{
		QTextBlock  block   = pTextDoc->findBlockByLineNumber(i);
		//cout	<<	block.text().toLocal8Bit().data()	<<	endl;
		QTextCursor cursor(block);
		QTextEdit::ExtraSelection selection;
		QColor lineColor = QColor(Qt::yellow);
		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = cursor;
		//selection.cursor.clearSelection();
		extraSelections.append(selection);
		//cout	<<	extraSelections.size()	<<	endl;
	}
	//�ڴ˴������߶�
	QTextBlock  btmBlock = pTextDoc->findBlockByLineNumber(m_highlightRegion[1]+5);
	QTextCursor btmCursor(btmBlock);
	//cout	<<	extraSelections.size()	<<	endl;
	setHighlightSections(extraSelections, btmCursor);
	/*m_pTextEdit->verticalScrollBar()->setValue(m_pTextEdit->verticalScrollBar()->maximum()
			*(double)(m_highlightRegion[0]-5)/pTextDoc->lineCount());*/
	//m_pTextEdit->verticalScrollBar()->setValue(m_highlightRegion[0]);
	//cout	<<	m_pTextEdit->verticalScrollBar()->maximum()	<<	endl;
}

void FCodeWnd::clearHighlight()
{
	m_highlightSections = QList<QTextEdit::ExtraSelection>();
	m_pTextEdit->setExtraSelections(m_highlightSections);
	m_pTextEdit->verticalScrollBar()->setValue(0);
}

void FCodeWnd::clearTextEdit()
{
	clearHighlight();
	setTextEdit("");
}
