#pragma once

#include "../autogen_macros.h"

#include <QTextBrowser>
#include <QSound>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>

#include "./ui_log_window.h"

UI_AUTOGEN(LogWindow)

class LogWindow : public QWidget
{
	Q_OBJECT

public:
	LogWindow(QWidget* parent = nullptr) : ui(new Ui::LogWindow), critical_notify(":/sound/cerror.wav", this)
	{
		ui->setupUi(this);

		ui->cb_verbosity->setCurrentIndex(4);

		default_c = ui->text_log->textColor();

		DCS::Utils::Logger::SetLogWriteCallback(log_callback, this);
		(void)connect(this, &LogWindow::receiveLogSig, this, &LogWindow::publishLog);

		(void)connect(ui->cb_verbosity, SIGNAL(currentIndexChanged(int)), this, SLOT(changeVerbosity(int)));
		(void)connect(ui->btn_clearLog, SIGNAL(clicked()), ui->text_log, SLOT(clear()));

	};

	~LogWindow() {  };

	inline static void log_callback(DCS::Utils::String msg, DCS::Utils::Logger::Verbosity v, void* ptr)
	{
		LogWindow* p = reinterpret_cast<LogWindow*>(ptr);

		QColor log_color;

		switch(v)
		{
			case DCS::Utils::Logger::Verbosity::CRITICAL: log_color = QColor("darkRed"); p->playCriticalSound(); break;
			case DCS::Utils::Logger::Verbosity::ERROR:    log_color = QColor("red"); break;
			case DCS::Utils::Logger::Verbosity::WARNING:  log_color = QColor("yellow"); break;
			case DCS::Utils::Logger::Verbosity::MESSAGE:  log_color = QColor("blue"); break;
			case DCS::Utils::Logger::Verbosity::DEBUG:    log_color = QColor("black"); break;
			default: 									  log_color = QColor("black"); break;
		}
		p->receiveLogSig(msg.c_str(), log_color);
	}

	inline void playCriticalSound()
	{
		critical_notify.play();
	}

public slots:
	void publishLog(QString msg, QColor log_c)
	{
		ui->text_log->setTextColor(log_c);
		ui->text_log->append(msg);
		ui->text_log->setTextColor(default_c);
	}

	void changeVerbosity(int value) 
	{
		DCS::Utils::Logger::ChangeVerbosity(static_cast<DCS::Utils::Logger::Verbosity>(value));
	}
	
signals:
	void receiveLogSig(QString msg, QColor log_c);

private:
	Ui::LogWindow* ui;
	QColor default_c;
	QSound critical_notify;
};
