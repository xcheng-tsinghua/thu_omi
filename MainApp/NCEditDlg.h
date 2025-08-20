/*
编辑NC代码的窗口类
程曦
*/
#pragma once
#include <qwidget.h>

class QTextEdit;

class NCEditDlg :public QWidget
{
	Q_OBJECT
public:
	NCEditDlg(const QString& fpContent, const QString& fpSavePathDef, QWidget* parent = NULL);
	NCEditDlg();
	~NCEditDlg();

	void setContent(const QString& fpContent);
	void setSavePathDef(const QString& fpSavePathDef);

private slots:
	void onSaveExit();

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	void InitWidget();
	void SaveStrToFile(QString fpText, QString fpFilePath);

	QTextEdit* m_pTextEdit;
	QString m_Content;
	QString m_SavePathDef;

};