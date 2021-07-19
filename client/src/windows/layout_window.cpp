#include "layout_window.h"
#include "./ui_layout_window.h"

#include <QEvent>
#include <QBitmap>

LayoutWindow::LayoutWindow(QWidget* parent) : ui(new Ui::LayoutWindow)
{
    ui->setupUi(this);
    main_window = dynamic_cast<MainWindow*>(parent);
}

LayoutWindow::~LayoutWindow()
{
    
}
