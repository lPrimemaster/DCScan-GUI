#include "spectraldisplay_window.h"
#include "./ui_spectraldisplay_window.h"

SpectralDisplayWindow::SpectralDisplayWindow(QWidget* parent) : ui(new Ui::SpectralDisplayWindow)
{
    ui->setupUi(this);
}

SpectralDisplayWindow::~SpectralDisplayWindow()
{

}