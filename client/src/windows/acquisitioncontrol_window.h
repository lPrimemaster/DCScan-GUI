#pragma once

#include "../autogen_macros.h"

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QList>
#include <QMap>
#include <array>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>
#include <DCS_Network/include/DCS_ModuleNetwork.h>

#include "main_window.h"
#include "connect_window.h"
#include "channelmanager_window.h"

UI_AUTOGEN(AcquisitionControlWindow)

class DragNDropOutputConfig;

// TODO : Allow this to know the current acquisition state and disable channel visualization
class AcquisitionControlWindow : public QWidget
{
	Q_OBJECT

public:
	AcquisitionControlWindow(QWidget* parent = nullptr);
	~AcquisitionControlWindow();

private slots:
	void addNewListModeField();
	void deleteListModeField(DragNDropOutputConfig* item);
	void swapField(DragNDropOutputConfig* item, int direction);

private:
	bool currentEventValid();

private:
	ChannelManagerWindow* cmw;
	Ui::AcquisitionControlWindow* ui;
	QList<QString> listmode_output_vars;
	static constexpr std::array AVAILABLE_OUT_VARS = {
		"Time Wall",
		"Time Real",
		"Time Dead",
		
		"Counts",
		"Counts Delta",
		
		"Bin",
		"Corrected Bin",

		"Calculated Bragg Angle",
		"Calculated Lattice Spacing",

		"Rotation Crystal 1",
		"Rotation Crystal 2",
		"Rotation Table",

		"Temperature Crystal 1",
		"Temperature Crystal 2"
	};
};
