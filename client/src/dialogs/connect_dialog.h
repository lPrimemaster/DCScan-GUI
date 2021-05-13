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

        (void)connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(auth()));
    }

public slots:
    void auth()
    {
        emit credentialsSignal(ui->usernameLineEdit->text(), ui->passwordLineEdit->text());

        ui->usernameLineEdit->clear();
        ui->passwordLineEdit->clear();
    }

signals:
    void credentialsSignal(QString username, QString password);

private:
	Ui::ConnectDialog* ui;
};
