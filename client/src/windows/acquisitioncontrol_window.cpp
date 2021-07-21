#include "acquisitioncontrol_window.h"
#include "./ui_acquisitioncontrol_window.h"

AcquisitionControlWindow::AcquisitionControlWindow(QWidget* parent) : ui(new Ui::AcquisitionControlWindow)
{
    ui->setupUi(this);

    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        QPixmap pixmap(15, 15);
        pixmap.fill(QColor("red"));
        ui->listWidget->item(i)->setIcon(QIcon(pixmap)); // Set all tasks as disabled on start (icons)
    }

    auto sdw = dynamic_cast<MainWindow*>(parent)->GetWindow<SpectralDisplayWindow>("Acquisition Details");

    (void)connect(sdw, &SpectralDisplayWindow::acquisitionEnableScheduleChanged, this, [&](const QString& task, int channel, bool status) {
        if(status)
        {
            QPixmap pixmap(15, 15);
            pixmap.fill(QColor("aqua"));
            auto item = ui->listWidget->item(channel);
            item->setIcon(QIcon(pixmap));
            item->setText("ai" + QString::number(channel) + " (" + task + " Task) [Scheduled]");
            channels.insert(task, channel);
        }
        else
        {
            QPixmap pixmap(15, 15);
            pixmap.fill(QColor("red"));
            auto item = ui->listWidget->item(channels[task]);
            item->setIcon(QIcon(pixmap));
            item->setText("ai" + QString::number(channels[task]) + " (No Task)");
            channels.remove(task);
        }
    });
}

AcquisitionControlWindow::~AcquisitionControlWindow()
{
    
}

