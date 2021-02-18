#include "windows/main_window.h"
#include "windows/histsett_window.h"

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QStyleFactory>
#include <QFile>

using namespace DCS::Utils;

int main(int argc, char *argv[])
{
	Logger::Init(Logger::Verbosity::DEBUG);


    QApplication a(argc, argv);

	// Register custom signal data
	qRegisterMetaType<GraphSettings>("GraphSettings");

    MainWindow w;

	QFile f("../../styles/default.css");
	if (!f.open(QFile::ReadOnly | QFile::Text))
	{
		Logger::Error("Failed to load stylesheet. Maybe location is wrong?");
	}
	else
	{
		QTextStream in(&f);

		if (f.size() > 0)
		{
			Logger::Message("Loaded custom stylesheet.");
		}

		QString s = in.readAll();
		a.setStyleSheet(s);
	}

    w.show();
	const int retval = a.exec();

	Logger::Destroy();

	return retval;
}
