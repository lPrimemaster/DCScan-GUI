#include "histsett_window.h"

#include <QPushButton>
#include <QFrame>
#include <QHideEvent>
#include <QCheckBox>

#include "hist_window.h"

constexpr int SPINBOX_MAX = 0xFFFFFF;

HistSettingsWindow::HistSettingsWindow(HistWindow* hw, QWidget* parent)
{
	// Set window layout
	parent_layout = new QVBoxLayout();
	setLayout(parent_layout);

	// Set warning label text, color and wrapping mode
	warning_label = new QLabel("Acquisition is in progress. Some options may not be available.");
	warning_label->setStyleSheet("QLabel { color : red; }");
	warning_label->setWordWrap(true);
	warning_label->hide();

	// Configure settings window master buttons
	QPushButton* sve_button = new QPushButton("Save");
	QPushButton* rst_button = new QPushButton("Reset");
	(void)connect(sve_button, &QPushButton::clicked, this, &HistSettingsWindow::updateAllSettingsValue);
	(void)connect(rst_button, &QPushButton::clicked, this, &HistSettingsWindow::resetAllSettingsValue);
	sve_button->setStyleSheet("QPushButton { max-width: 100px; }");
	rst_button->setStyleSheet("QPushButton { max-width: 100px; }");

	// Configure master layout sub-layouts
	QTabWidget* tabs = new QTabWidget(this);
	tab_bot = new QHBoxLayout();
	tab_bot->addWidget(warning_label, Qt::AlignLeft);
	tab_bot->addStretch();
	tab_bot->addWidget(sve_button, Qt::AlignRight);
	tab_bot->addWidget(rst_button, Qt::AlignRight);
	parent_layout->addWidget(tabs);
	parent_layout->addLayout(tab_bot);

	// Configure output events
	if (!hw)
	{
		LOG_CRITICAL("Failed to establish link from [Graph Settings] to [Graph] windows.");
		LOG_CRITICAL("Settings window will not function properly!!!");
	}
	else
	{
		(void)connect(this, &HistSettingsWindow::settingsChangedSig, hw, &HistWindow::updateAllSettings);
		LOG_DEBUG("[Graph Settings] -> [Graph] link successful!");
	}

	// 'Axis' Tab settings
	QFrame* axis_tab = new QFrame(this);
	QGridLayout* axis_tab_layout = new QGridLayout();
	axis_tab->setLayout(axis_tab_layout);
	int c_row = 0;
	constexpr int col_span = 3;

	axis_tab_layout->setAlignment(Qt::AlignTop);
	auto add_header = [&](const char* name)-> void {
		QFrame* line = new QFrame(this);
		line->setGeometry(QRect(320, 150, 500, 3));
		line->setFrameShape(QFrame::HLine);
		line->setFrameShadow(QFrame::Sunken);
		auto title_t = new QLabel(name);
		title_t->setStyleSheet("QLabel { font-weight:600; }");
		if(c_row > 0)
			axis_tab_layout->addItem(new QSpacerItem(0, 20, QSizePolicy::Ignored, QSizePolicy::Minimum), c_row++, 0);

		axis_tab_layout->addWidget(title_t, c_row++, 0, 1, col_span);
		axis_tab_layout->addWidget(line, c_row++, 0, 1, col_span);
	};
	auto add_property = [&](const char* name, QWidget* w)-> void {
		auto mb_name = new QLabel(name);
		mb_name->setBuddy(w);
		axis_tab_layout->addWidget(mb_name, c_row, 0);
		axis_tab_layout->addWidget(w, c_row++, 1, 1, col_span-1);
	};
	auto add_widget_col = [&](QWidget* w, int col) {
		axis_tab_layout->addWidget(w, c_row, col);
	};

	add_header("Bins");

	widgets.max_bin_spinBox = new QSpinBox(this);
	widgets.max_bin_spinBox->setRange(0, SPINBOX_MAX);
	widgets.max_bin_spinBox->setValue(100);
	add_property("Number:", widgets.max_bin_spinBox);

	widgets.bin_color_comboBox = new QComboBox(this);
	widgets.bin_color_comboBox->addItem(QtUtils::CreateBasicColorIcon("lightblue"), "Blue", QColor("lightblue"));
	widgets.bin_color_comboBox->addItem(QtUtils::CreateBasicColorIcon("red"), "Red", QColor("red"));
	add_property("Color:", widgets.bin_color_comboBox);

	add_header("View");

	QPushButton* reset_barGraph_window = new QPushButton("Adjust Graph View");
	(void)connect(reset_barGraph_window, &QPushButton::clicked, this, &HistSettingsWindow::updateGraphView);
	add_widget_col(reset_barGraph_window, 0);


	QCheckBox* auto_reset_barGraph_window = new QCheckBox(this);
	
	(void)connect(auto_reset_barGraph_window, &QCheckBox::toggled, this, &HistSettingsWindow::updateGraphView);
	auto qhl = new QHBoxLayout();
	qhl->addWidget(new QLabel("Auto"));
	qhl->addWidget(auto_reset_barGraph_window);
	QWidget* container = new QWidget(this);
	container->setLayout(qhl);
	add_widget_col(container, 1);

	tabs->addTab(axis_tab, "Axis");
}

void HistSettingsWindow::hideEvent(QHideEvent* event)
{
	resetAllSettingsValue();
	QWidget::hideEvent(event);
}

void HistSettingsWindow::updateAllSettingsValue()
{
	LOG_DEBUG("Save new graph settings.");
	settings.maximum_bins = widgets.max_bin_spinBox->value();
	settings.bins_color = widgets.bin_color_comboBox->currentData().value<QColor>();
	settings.bins_color_idx = widgets.bin_color_comboBox->currentIndex();

	emit settingsChangedSig(settings);
}

void HistSettingsWindow::resetAllSettingsValue()
{
	LOG_DEBUG("Reset all graph settings.");
	widgets.max_bin_spinBox->setValue(settings.maximum_bins);
	widgets.bin_color_comboBox->setCurrentIndex(settings.bins_color_idx);
}

void HistSettingsWindow::updateStatusAcqRunning(bool state)
{
	if (!state) warning_label->show(); else warning_label->hide();
	widgets.max_bin_spinBox->setEnabled(state);
}

void HistSettingsWindow::updateGraphView(bool auto_adj)
{
	emit axisViewAdjustSig(auto_adj);
}
