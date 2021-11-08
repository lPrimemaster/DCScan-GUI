#include "windows/main_window.h"

#include <fstream>
#include <iterator>
#include <sstream>

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QStyleFactory>
#include <QFile>

int main(int argc, char *argv[])
{
	DCS::Utils::Logger::Init(DCS::Utils::Logger::Verbosity::DEBUG);

    QApplication a(argc, argv);

	// Register custom signal data
	//qRegisterMetaType<GraphSettings>("GraphSettings");
	qRegisterMetaType<QVector<QPointF>>("QVector<QPointF>");
	qRegisterMetaType<DCS::DAQ::MCACountEventData>("DCS::DAQ::MCACountEventData");
	qRegisterMetaType<DCS::DAQ::DCSCountEventData>("DCS::DAQ::DCSCountEventData");

    MainWindow w(&a);

	// TODO : Create a black color theme as well
	QPalette pal; // White palette
	/*pal.setColor(QPalette::Normal, QPalette::Window,		  QColor("#323232"));
	pal.setColor(QPalette::Normal, QPalette::WindowText,	  QColor("#ffffff"));
	pal.setColor(QPalette::Normal, QPalette::Text,			  QColor("#ffffff"));
	pal.setColor(QPalette::Inactive, QPalette::Text,		  QColor("#ffffff"));
	pal.setColor(QPalette::Normal, QPalette::Base,			  QColor("#9c9c9c"));
	pal.setColor(QPalette::Normal, QPalette::AlternateBase,	  QColor("#9c9c9c"));
	pal.setColor(QPalette::Inactive, QPalette::ToolTipBase,   QColor("#212121"));
	pal.setColor(QPalette::Inactive, QPalette::ToolTipText,	  QColor("#ffffff"));
	pal.setColor(QPalette::Normal, QPalette::PlaceholderText, QColor("#ffffff"));
	pal.setColor(QPalette::Normal, QPalette::Button,		  QColor("#3aadde"));
	pal.setColor(QPalette::Normal, QPalette::ButtonText,	  QColor("#ffffff"));
	pal.setColor(QPalette::Normal, QPalette::BrightText,	  QColor("#ffffff"));*/

	pal.setColor(QPalette::Normal, QPalette::Light,	   		  QColor("#a1a1a1"));
	pal.setColor(QPalette::Normal, QPalette::Midlight, 		  QColor("#808080"));
	pal.setColor(QPalette::Normal, QPalette::Mid,	   		  QColor("#5c5c5c"));
	pal.setColor(QPalette::Normal, QPalette::Dark,	   		  QColor("#333333"));
	pal.setColor(QPalette::Normal, QPalette::Shadow,   		  QColor("#000000"));

	pal.setColor(QPalette::Normal, QPalette::Highlight,		  QColor("#ed7e00"));
	pal.setColor(QPalette::Normal, QPalette::HighlightedText, QColor("#000000"));
	pal.setColor(QPalette::Normal, QPalette::Link,			  QColor("#ffffff"));
	pal.setColor(QPalette::Normal, QPalette::LinkVisited,	  QColor("#ffffff"));

	a.setPalette(pal);

	QFile f("../../../client/styles/default.css");
	if (!f.open(QFile::ReadOnly | QFile::Text))
	{
		LOG_ERROR("Failed to load stylesheet. Maybe location is wrong?");
	}
	else
	{
		QTextStream in(&f);

		if (f.size() > 0)
		{
			LOG_MESSAGE("Loaded custom stylesheet.");
		}

		QString s = in.readAll();
		a.setStyleSheet(s);
		f.close();
	}

    w.show();
	const int retval = a.exec();

	DCS::Utils::Logger::Destroy();

	return retval;
}
