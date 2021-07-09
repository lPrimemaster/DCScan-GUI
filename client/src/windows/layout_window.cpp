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

bool LayoutWindow::event(QEvent* e)
{
    if (e->type() == QEvent::WindowActivate) 
    {
        // QPixmap pixmap = windowIcon().pixmap(16);
        // QBitmap mask = pixmap.createMaskFromColor(QColor("transparent"), Qt::MaskOutColor);
        // pixmap.fill(QColor("white"));
        // pixmap.setMask(mask);
        // main_window->SetSubWIcon("Layout Manager", QIcon(pixmap));
    }
    else if (e->type() == QEvent::WindowDeactivate) 
    {
        
    }

    return QWidget::event(e);
}
