#include "test_window.h"
#include "./ui_test_window.h"
#include <QFile>
#include <QTextStream>
#include <QSettings>


void TestWindow::applyStyle()
{
	QFile f("../../../client/styles/default.css");
	f.open(QFile::ReadOnly | QFile::Text);
	QTextStream in(&f);

	if (f.size() > 0)
	{
		LOG_MESSAGE("Reloaded custom stylesheet.");
		QString s = in.readAll();
		a->setStyleSheet(s);
	}
	f.close();
}

void TestWindow::overrideADSDefaultPerspective()
{
	QSettings s("ads_perspectives.ini", QSettings::Format::IniFormat);
	manager->addPerspective("Default");
	manager->savePerspectives(s);
}

TestWindow::TestWindow(QWidget* parent) : ui(new Ui::TestWindow)
{
	ui->setupUi(this);

	a = dynamic_cast<MainWindow*>(parent)->GetApp();

	manager = dynamic_cast<MainWindow*>(parent)->GetDockManager();

	(void)connect(ui->btn_reload_css, &QPushButton::clicked, this, &TestWindow::applyStyle);

	(void)connect(ui->btn_override_perspective, &QPushButton::clicked, this, &TestWindow::overrideADSDefaultPerspective);
}

TestWindow::~TestWindow()
{
	delete ui;
}
