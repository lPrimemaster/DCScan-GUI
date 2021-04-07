#include "test_window.h"
#include "./ui_test_window.h"
#include <QFile>
#include <QTextStream>

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

TestWindow::TestWindow(QApplication* app, QWidget* parent) : ui(new Ui::TestWindow)
{
	ui->setupUi(this);

	a = app;

	(void)connect(ui->btn_reload_css, &QPushButton::clicked, this, &TestWindow::applyStyle);
}

TestWindow::~TestWindow()
{
	delete ui;
}
