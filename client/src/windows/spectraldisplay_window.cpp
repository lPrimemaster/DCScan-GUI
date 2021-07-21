#include "spectraldisplay_window.h"
#include "./ui_spectraldisplay_window.h"

#include <QtWidgets>

static void SetComboBoxItemEnabled(QComboBox * comboBox, int index, bool enabled)
{
    auto* model = qobject_cast<QStandardItemModel*>(comboBox->model());
    assert(model);
    if(!model) return;

    if(comboBox->currentIndex() == index)
    {
        if(comboBox->currentIndex() < comboBox->count() - 1)
        {
            comboBox->setCurrentIndex(index + 1);
        }
        else if(comboBox->currentIndex() > 0)
        {
            comboBox->setCurrentIndex(index - 1);
        }
    }

    auto * item = model->item(index);
    assert(item);
    if(!item) return;
    item->setEnabled(enabled);
}

SpectralDisplayWindow::SpectralDisplayWindow(QWidget* parent) : ui(new Ui::SpectralDisplayWindow)
{
    ui->setupUi(this);

    (void)connect(ui->checkBox_3, &QCheckBox::clicked, this, [&](bool click) {
        auto idx = ui->comboBox_3->currentIndex();
        emit acquisitionEnableScheduleChanged("DCS", idx, click);
        ui->comboBox_3->setEnabled(!click);
        
        SetComboBoxItemEnabled(ui->comboBox_4, idx, !click);
    });

    (void)connect(ui->checkBox_4, &QCheckBox::clicked, this, [&](bool click) {
        auto idx = ui->comboBox_4->currentIndex();
        emit acquisitionEnableScheduleChanged("MCA", idx, click);
        ui->comboBox_4->setEnabled(!click);

        SetComboBoxItemEnabled(ui->comboBox_3, idx, !click);
    });
}

SpectralDisplayWindow::~SpectralDisplayWindow()
{

}