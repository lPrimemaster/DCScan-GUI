#pragma once

#include "../autogen_macros.h"
#include "./ui_dnd_outputconfig.h"
#include <QPushButton>

UI_AUTOGEN(DragNDropOutputConfig)

class DragNDropOutputConfig : public QWidget
{
	Q_OBJECT

public:
	DragNDropOutputConfig(QWidget* parent = nullptr) : ui(new Ui::DragNDropOutputConfig)
    {
        ui->setupUi(this);
    }

	~DragNDropOutputConfig() {  }

    inline void setComboOptions(const QList<QString>& options)
    {
        ui->property_name->clear();
        for(const auto& option : options)
        {
            ui->property_name->addItem(option);
        }
    }

    inline void setPropertyLabel(const QString& name)
    {
        ui->label->setText(name);
    }

    inline QPushButton* getButtonUp()   const { return ui->pushButton; }
    inline QPushButton* getButtonDown() const { return ui->pushButton_2; }
    inline QPushButton* getButtonDel()  const { return ui->pushButton_3; }

private:
	Ui::DragNDropOutputConfig* ui;
};