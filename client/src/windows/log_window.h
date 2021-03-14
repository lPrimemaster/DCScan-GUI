#pragma once

#include <QTextBrowser>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>


class LogWindow : public QTextBrowser
{
	Q_OBJECT

public:
	LogWindow(QWidget* parent = nullptr)
	{
		DCS::Utils::Logger::SetLogWriteCallback(log_callback, this);
		(void)connect(this, &LogWindow::receiveLogSig, this, &LogWindow::publishLog);
	};

	~LogWindow() {  };

	inline static void log_callback(DCS::Utils::String msg, void* ptr)
	{
		LogWindow* p = reinterpret_cast<LogWindow*>(ptr);
		p->receiveLogSig(msg.c_str());
	}

public slots:
	void publishLog(QString msg) { append(msg); }

signals:
	void receiveLogSig(QString msg);
};
