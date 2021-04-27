#pragma once

#include "../autogen_macros.h"

#include <QTabWidget>
#include <QWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

#include "./ui_connect_dialog.h"

UI_AUTOGEN(ConnectDialog)

class ConnectDialog : public QDialog
{
	Q_OBJECT

public:
	ConnectDialog(QWidget* parent = nullptr) : QDialog(parent), ui(new Ui::ConnectDialog) 
    {
        ui->setupUi(this);
    }

private:
	Ui::ConnectDialog* ui;
};
