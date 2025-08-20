//main.cpp : Defines the entry point for the PrinterCAD application.

#define WNT

#include "../External/fkernel/Include/FKernelUtilities.h"
#include "FMainWindow.h"
#include "MainView.h"
#include "MainApp/GeneratedFiles/ui_MainWindow.h"

FMainWindow*	gWindow;
FMainView*		gView;
QString			gAppPath;	//file directory of the exe
QSettings*		gAppSetting;
QSettings*		gDlgSetting;// ��¼����Ĭ����ֵ״̬
QString			gDockTitleColor;
int				gInsFeaIndex;

clock_t			gTStart, gTEnd;

int main(int argc, char* argv[])
{
	//FreeConsole();
	gDockTitleColor = "rgb(64,86,141);";
	gInsFeaIndex = 0;

	QApplication app(argc, argv);
	QPixmap pixmap("../icons/App.png");
	QSplashScreen splash(pixmap);
	//splash.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	splash.show();
	splash.showMessage(QString::fromLocal8Bit("����..."), Qt::AlignLeft | Qt::AlignBottom, Qt::black);
	Sleep(100);

	gAppPath = QApplication::applicationDirPath();
	splash.showMessage(QString::fromLocal8Bit("������Ӧ��·��..."), Qt::AlignLeft | Qt::AlignBottom, Qt::black);
	Sleep(200);

	gAppSetting = new QSettings(gAppPath + "\\settings\\THP_Lite.ini", QSettings::IniFormat);
	splash.showMessage(QString::fromLocal8Bit("���ػ��������ļ�..."), Qt::AlignLeft | Qt::AlignBottom, Qt::black);

	gDlgSetting = new QSettings(gAppPath + "\\settings\\DlgSetting.ini", QSettings::IniFormat);
	Sleep(400);

	app.addLibraryPath("../sqldrivers");
	splash.showMessage(QString::fromLocal8Bit("���ؿ��ļ�·��..."), Qt::AlignLeft | Qt::AlignBottom, Qt::black);
	Sleep(100);

	gWindow = new FMainWindow;

	splash.showMessage(QString::fromLocal8Bit("����ע���..."), Qt::AlignLeft | Qt::AlignBottom, Qt::black);
	Sleep(200);

	splash.showMessage(QString::fromLocal8Bit("���ڳ�ʼ��.Net���л���..."), Qt::AlignLeft | Qt::AlignBottom, Qt::black);
	Sleep(300);

	splash.showMessage(QString::fromLocal8Bit("����Ӧ�ó���..."), Qt::AlignLeft | Qt::AlignBottom, Qt::black);
	Sleep(2000);
	
	//gWindow->showMaximized();
	gWindow->GetWindowTitle()->p_MenuLayout->addWidget(gWindow->getUI()->menubar);
	gWindow->GetFrameLessWidget()->show();
	//gWindow->selMainWidget->showMaximized();
	gWindow->GetFrameLessWidget()->setGeometry(QApplication::desktop()->availableGeometry());
	splash.close();
	//gView = new FMainView;
	//gView->Update();
	//gView->show();
	//QString tex = QApplication::applicationDirPath()+"/Material/"+"Gold.bmp";
	//GLuint texID(0);
	//gCreateTexture(tex, texID);
	FDoc* pDoc = gPDoc;
	int iSucc = app.exec();
	//gCloseLogFile();
	return iSucc;
}