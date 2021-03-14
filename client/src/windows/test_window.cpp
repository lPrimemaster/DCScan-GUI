#include "test_window.h"
#include "./ui_test_window.h"

TestWindow::TestWindow(QWidget* parent) : ui(new Ui::TestWindow)
{
	ui->setupUi(this);
	ui->sb_count->setDisabled(true);
}

TestWindow::~TestWindow()
{
	delete ui;
}
