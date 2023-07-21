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
#include <QCheckBox>
#include <QLineSeries>
#include <QValueAxis>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>
#include <DCS_Network/include/DCS_ModuleNetwork.h>

#include "main_window.h"
#include "../extras/qindicator.h"

#include <QAbstractTableModel>
#include <array>
#include <set>

UI_AUTOGEN(ChannelManagerWindow)

class NewChannelDialog;
class ChannelManagerWindow;

class TableView : public QAbstractTableModel
{
    Q_OBJECT

public:
	static constexpr const char* column_names[] = {
		"Channel",
		"Connector",
		"I/O",
		"Type",
		"Rate"
	};
	using cols_type = std::array<QVariant, sizeof(column_names)/sizeof(column_names[0])>;
    TableView(QObject* parent = nullptr);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE { return static_cast<int>(rows.size()); }
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE { return sizeof(column_names)/sizeof(column_names[0]); }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE
    {
        if(!index.isValid() || role != Qt::DisplayRole)
        {
            return QVariant();
        }
        return rows[index.row()][index.column()];
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE
    {
        if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
        {
            return column_names[section];
        }
        return QVariant();
    }

	cols_type getRow(int i) const
	{
		return rows[i];
	}

	cols_type getRow(const QString& name) const
	{
		auto row = std::find_if(rows.begin(), rows.end(), [name](const auto& row) { return row[0] == name; });
		if(row != rows.end())
		{
			return *row;
		}
		return cols_type();
	}

	const std::vector<cols_type>& getRows() const
	{
		return rows;
	}

public slots:
	void pushRow(const cols_type& cols);
	void popRowByIndex(size_t i);
    // If there are multiple -> remove first only
    // void popRowByProperty(const QString& column, const QVariant& value)
    // {
    //     std::find_if(rows.begin(), rows.end(), [](const auto& row) {  });
    // }

private:
    std::vector<cols_type> rows;
	ChannelManagerWindow* cmw;
};

// TODO : Allow this to know the current acquisition state and disable channel visualization
class ChannelManagerWindow : public QWidget
{
	Q_OBJECT

public:
	ChannelManagerWindow(QWidget* parent = nullptr);
	~ChannelManagerWindow();

	void comboAddItem(const QString& name);
	void comboRemoveItem(const QString& name);
	void comboClear();
	void comboEnable(bool e);
	void labelSetOff();

	inline const TableView* getTableView() const
	{
		return table_model;
	}

	QString getCounterAcquisitionHardwareChannel() const;
	QString getCounterTimebaseHardwareChannel() const;
	double  getCounterTimebaseRate() const;
	bool    getCounterEventValid() const;

	QString getVoltageAcquisitionHardwareChannel() const;
	QString getVoltageTimebaseHardwareChannel() const;
	double  getVoltageTimebaseRate() const;
	bool    getVoltageEventValid() const;

signals:
	void pushRowSig(const TableView::cols_type& cols);
	void popRowSig(int i);
	void eventValueChanged();

public slots:
	void createNewChannel(const TableView::cols_type& values);
	void deleteChannel(const QString& name);

private:
	inline QString getComboPropertyByIdx(const QComboBox* cb, int idx) const
	{
		auto row = table_model->getRow(cb->currentText());
		if(row[0].isNull())
		{
			return QString();
		}
		return row[idx].toString();
	}

private:
	Ui::ChannelManagerWindow* ui;
	NewChannelDialog* ncd;
	TableView* table_model;
	std::set<int> table_selection;
	QList<QIndicator> leds;
};
