#include "freejog_window.h"
#include "./ui_freejog_window.h"

FreejogWindow::FreejogWindow(QWidget* parent) : ui(new Ui::FreejogWindow)
{
    auto connect_window = dynamic_cast<MainWindow*>(parent)->GetWindow<ConnectWindow>("Remote Control");
    (void)connect(connect_window, SIGNAL(connectionChanged(bool)), this, SLOT(enableFreejog(bool)), Qt::QueuedConnection);
    ui->setupUi(this);
    ui->warning_msg->setText("Free motion is disabled.\nConnect to server to enable.");
    enableFreejog(false);

    (void)connect(ui->t1_slider, SIGNAL(valueChanged(int)), this, SLOT(log_test()));
    (void)connect(ui->t1_slider, SIGNAL(sliderReleased()), this, SLOT(resetSlider1()));
    (void)connect(ui->t2_slider, SIGNAL(sliderReleased()), this, SLOT(resetSlider2()));
}

FreejogWindow::~FreejogWindow()
{

}

void FreejogWindow::log_test()
{
    qDebug() << "Did it just dragged?";
}

void FreejogWindow::resetSlider1()
{
    ui->t1_slider->setValue(0);
}

void FreejogWindow::resetSlider2()
{
    ui->t2_slider->setValue(0);
}

void FreejogWindow::enableFreejog(bool e)
{
    if(!e)
    {
        ui->warning_msg->setText("Free motion is disabled.\nConnect to server to enable.");
    }

    is_enabled = e;

    ui->warning_msg->setVisible(!e);
    ui->warning_sign->setVisible(!e);

    ui->doubleSpinBox->setEnabled(e);
    ui->doubleSpinBox_2->setEnabled(e);
    ui->doubleSpinBox_3->setEnabled(e);
    ui->doubleSpinBox_4->setEnabled(e);

    ui->spinBox->setEnabled(e);
    ui->spinBox_2->setEnabled(e);
    ui->spinBox_3->setEnabled(e);
    ui->spinBox_4->setEnabled(e);

    ui->t1_slider->setEnabled(e);
    ui->t2_slider->setEnabled(e);
}