#include "channelmanager_window.h"
#include "./ui_channelmanager_window.h"
#include "connect_window.h"
#include "../dialogs/newchannel_dialog.h"

#include <QtWidgets>

TableView::TableView(QObject* parent) : QAbstractTableModel(parent), cmw(dynamic_cast<ChannelManagerWindow*>(parent))
{
    qRegisterMetaType<TableView::cols_type>("TableView::cols_type");
}

void TableView::pushRow(const cols_type& cols)
{
    cmw->comboAddItem(cols[0].toString());

    beginInsertRows(QModelIndex(), (int)rows.size(), (int)rows.size());
    rows.push_back(cols);
    endInsertRows();
}

void TableView::popRowByIndex(size_t i)
{
    if(i >= rows.size()) return;
    cmw->comboRemoveItem(rows[i][0].toString());

    beginRemoveRows(QModelIndex(), (int)i, (int)i);
    rows.erase(rows.begin() + i);
    endRemoveRows();
}


ChannelManagerWindow::ChannelManagerWindow(QWidget* parent) : ui(new Ui::ChannelManagerWindow)
{
    ui->setupUi(this);
    ui->pushButton->setEnabled(false);
    ncd = new NewChannelDialog(this, parent);
	ncd->setModal(true);
    (void)connect(ui->pushButton, &QPushButton::clicked, ncd, &NewChannelDialog::exec);
    (void)connect(this, &ChannelManagerWindow::pushRowSig, this, &ChannelManagerWindow::createNewChannel);

    auto connect_window = dynamic_cast<MainWindow*>(parent)->GetWindow<ConnectWindow>("Remote Control");
    (void)connect(connect_window, &ConnectWindow::connectionChanged, this, [&](bool status) {
        if(status)
        {
            comboClear();
            comboAddItem("None");

            for(int i = 0; i < table_model->rowCount(); i++)
            {
                comboAddItem(table_model->getRow(i)[0].toString());
            }

            comboEnable(true);
            ui->pushButton->setEnabled(true);

            for(const auto& c : table_model->getRows())
            {
                createNewChannel(c);
            }
        }
        else
        {
            comboClear();
            comboAddItem("No Connection");
            labelSetOff();
            comboEnable(false);
            ui->pushButton->setEnabled(false);
        }
    }, Qt::QueuedConnection);

    // Network cleanup before close
    (void)connect(connect_window, &ConnectWindow::connectionAboutToClose, this, [&]() {
        int idx = 0;
        for(const auto& c : table_model->getRows())
        {
            deleteChannel(c[0].toString());
            emit popRowSig(idx++);
        }
    }, Qt::DirectConnection);

    ui->led_0->setStateTooltip(QIndicator::State::RED,    "Event Disabled");
    ui->led_1->setStateTooltip(QIndicator::State::RED, "Timebase Disabled");
    ui->led_2->setStateTooltip(QIndicator::State::RED,    "Event Disabled");
    ui->led_3->setStateTooltip(QIndicator::State::RED, "Timebase Disabled");
    ui->led_4->setStateTooltip(QIndicator::State::RED,  "Trigger Disabled");
    ui->led_5->setStateTooltip(QIndicator::State::RED,  "Trigger Disabled");

    ui->led_0->setStateTooltip(QIndicator::State::YELLOW, "Event Ready");
    ui->led_2->setStateTooltip(QIndicator::State::YELLOW, "Event Ready");
    ui->led_4->setStateTooltip(QIndicator::State::YELLOW, "Trigger Off");
    ui->led_5->setStateTooltip(QIndicator::State::YELLOW, "Trigger Off");

    ui->led_0->setStateTooltip(QIndicator::State::GREEN,  "Event Triggered");
    ui->led_1->setStateTooltip(QIndicator::State::GREEN, "Timebase Enabled");
    ui->led_2->setStateTooltip(QIndicator::State::GREEN,  "Event Triggered");
    ui->led_3->setStateTooltip(QIndicator::State::GREEN, "Timebase Enabled");
    ui->led_4->setStateTooltip(QIndicator::State::GREEN,       "Trigger On");
    ui->led_5->setStateTooltip(QIndicator::State::GREEN,       "Trigger On");

    comboClear();
    comboAddItem("No Connection");
    labelSetOff();
    comboEnable(false);

    table_model = new TableView(this);
    ui->tableView->setModel(table_model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SelectionMode::ContiguousSelection);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setGridStyle(Qt::PenStyle::NoPen);

    enum class ComboLedType
    {
        EVENT_VOLT,
        TIMEBASE_VOLT,
        EVENT_CNT,
        TIMEBASE_CNT,
        TRIGGER_TEMP1,
        TRIGGER_TEMP2
    };

    auto event_combo_led = {
        std::make_tuple(ui->led_0, ui->comboBox, ComboLedType::EVENT_VOLT),
        std::make_tuple(ui->led_1, ui->comboBox_5, ComboLedType::TIMEBASE_VOLT),
        std::make_tuple(ui->led_2, ui->comboBox_6, ComboLedType::EVENT_CNT),
        std::make_tuple(ui->led_3, ui->comboBox_7, ComboLedType::TIMEBASE_CNT),
        std::make_tuple(ui->led_4, ui->comboBox_8, ComboLedType::TRIGGER_TEMP1),
        std::make_tuple(ui->led_5, ui->comboBox_9, ComboLedType::TRIGGER_TEMP2)
    };

    // Table Columns Memo
	// Channel
	// Connector
	// I/O
	// Type
    // Rate
    std::for_each(event_combo_led.begin(), event_combo_led.end(), [this](const std::tuple<QIndicator*, QComboBox*, ComboLedType> led_box_tuple) {
        QIndicator* led = std::get<0>(led_box_tuple);
        QComboBox* cbox = std::get<1>(led_box_tuple);
        ComboLedType type = std::get<2>(led_box_tuple);
        (void)connect(cbox, &QComboBox::currentTextChanged, this, [this, led, type, cbox](const QString& channel) {
            if(channel.isEmpty()) return;
            if(channel == "None" || channel == "No Connection")
            {
                led->setState(QIndicator::State::RED);
            }
            else
            {
                auto table_data = getTableView()->getRows();
                if(table_data.empty()) return;
                auto row = *std::find_if(table_data.begin(), table_data.end(), [channel](const TableView::cols_type& row) { return row[0] == channel; });
                switch (type)
                {
                case ComboLedType::EVENT_CNT:
                {
                    if(row[2] != "Input" || row[3] != "Counter")
                    {
                        LOG_ERROR("Cannot select channel \'%s\' for the event counter. Event requires a counter input type channel.", channel.toStdString().c_str());
                        cbox->setCurrentIndex(0); // None
                        break;
                    }
                    led->setState(QIndicator::State::YELLOW);
                } break;

                case ComboLedType::EVENT_VOLT:
                {
                    if(row[2] != "Input" || row[3] != "Analog")
                    {
                        LOG_ERROR("Cannot select channel \'%s\' for the event voltage. Event requires an analog input type channel.", channel.toStdString().c_str());
                        cbox->setCurrentIndex(0); // None
                        break;
                    }
                    led->setState(QIndicator::State::YELLOW);
                } break;

                case ComboLedType::TIMEBASE_CNT: [[falltrhrough]];
                case ComboLedType::TIMEBASE_VOLT:
                {
                    if(row[3] != "Digital" && row[3] != "Internal")
                    {
                        LOG_ERROR("Cannot select channel \'%s\' for the selected timebase. Timebase requires a digital or internal type channel.", channel.toStdString().c_str());
                        cbox->setCurrentIndex(0); // None
                        break;
                    }
                    led->setState(QIndicator::State::GREEN);
                } break;

                case ComboLedType::TRIGGER_TEMP1: [[falltrhrough]];
                case ComboLedType::TRIGGER_TEMP2:
                {
                    if(row[2] != "Output" || row[3] != "Digital")
                    {
                        LOG_ERROR("Cannot select channel \'%s\' for the selected trigger out. Trigger out requires a digital output type channel.", channel.toStdString().c_str());
                        cbox->setCurrentIndex(0); // None
                        break;
                    }
                    led->setState(QIndicator::State::YELLOW);
                } break;
                
                default:
                    break;
                }
            }
        });
    });

    (void)connect(this, &ChannelManagerWindow::popRowSig, table_model, &TableView::popRowByIndex);
    (void)connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
        table_selection.clear();

        // TODO : Disable this if something is active / running

        for(const auto& s : selected)
        {
            if(s.isEmpty() || !s.isValid())
            {
                continue;
            }

            for(const auto& idx : s.indexes())
            {
                table_selection.insert(idx.row());
            }
        }
        
        if(table_selection.empty())
        {
            ui->pushButton_2->setEnabled(false);
        }
        else
        {
            ui->pushButton_2->setEnabled(true);
        }
    });

    (void)connect(ui->pushButton_2, &QPushButton::clicked, this, [this](){
        if(!table_selection.empty())
        {
            while(!table_selection.empty())
            {
                const int idx = *(--table_selection.end());
                deleteChannel(table_model->getRow(idx)[0].toString());
                emit popRowSig(idx);
                table_selection.erase(idx);
            }
            ui->pushButton_2->setEnabled(false);
        }
    });

    (void)connect(ui->comboBox,    SIGNAL(currentIndexChanged(int)), this, SIGNAL(eventValueChanged()));
    (void)connect(ui->comboBox_5,  SIGNAL(currentIndexChanged(int)), this, SIGNAL(eventValueChanged()));
    (void)connect(ui->comboBox_6,  SIGNAL(currentIndexChanged(int)), this, SIGNAL(eventValueChanged()));
    (void)connect(ui->comboBox_7,  SIGNAL(currentIndexChanged(int)), this, SIGNAL(eventValueChanged()));
    (void)connect(ui->comboBox_8,  SIGNAL(currentIndexChanged(int)), this, SIGNAL(eventValueChanged()));
    (void)connect(ui->comboBox_9,  SIGNAL(currentIndexChanged(int)), this, SIGNAL(eventValueChanged()));
    (void)connect(ui->comboBox_10, SIGNAL(currentIndexChanged(int)), this, SIGNAL(eventValueChanged()));
    (void)connect(ui->comboBox_11, SIGNAL(currentIndexChanged(int)), this, SIGNAL(eventValueChanged()));
}

ChannelManagerWindow::~ChannelManagerWindow()
{

}

void ChannelManagerWindow::comboAddItem(const QString& name)
{
    ui->comboBox->addItem(name);
    ui->comboBox_5->addItem(name);
    ui->comboBox_6->addItem(name);
    ui->comboBox_7->addItem(name);
    ui->comboBox_8->addItem(name);
    ui->comboBox_9->addItem(name);
}

void ChannelManagerWindow::comboRemoveItem(const QString& name)
{
    int remove_index = -1;
    for(int i = 0; i < ui->comboBox->count(); i++)
    {
        if(ui->comboBox->itemText(i) == name)
        {
            remove_index = i;
            break;
        }
    }

    if(remove_index < 0) return;

    ui->comboBox->removeItem(remove_index);
    ui->comboBox_5->removeItem(remove_index);
    ui->comboBox_6->removeItem(remove_index);
    ui->comboBox_7->removeItem(remove_index);
    ui->comboBox_8->removeItem(remove_index);
    ui->comboBox_9->removeItem(remove_index);
}

void ChannelManagerWindow::comboClear()
{
    ui->comboBox->clear();
    ui->comboBox_5->clear();
    ui->comboBox_6->clear();
    ui->comboBox_7->clear();
    ui->comboBox_8->clear();
    ui->comboBox_9->clear();
}

void ChannelManagerWindow::comboEnable(bool e)
{
    ui->comboBox->setEnabled(e);
    ui->comboBox_5->setEnabled(e);
    ui->comboBox_6->setEnabled(e);
    ui->comboBox_7->setEnabled(e);
    ui->comboBox_8->setEnabled(e);
    ui->comboBox_9->setEnabled(e);
}

void ChannelManagerWindow::labelSetOff()
{
    ui->led_0->setState(QIndicator::State::RED);
    ui->led_1->setState(QIndicator::State::RED);
    ui->led_2->setState(QIndicator::State::RED);
    ui->led_3->setState(QIndicator::State::RED);
    ui->led_4->setState(QIndicator::State::RED);
    ui->led_5->setState(QIndicator::State::RED);
}

QString ChannelManagerWindow::getCounterAcquisitionHardwareChannel() const { return getComboPropertyByIdx(ui->comboBox_6, 1); }
QString ChannelManagerWindow::getCounterTimebaseHardwareChannel() const { return getComboPropertyByIdx(ui->comboBox_7, 1); }
double  ChannelManagerWindow::getCounterTimebaseRate() const { return getComboPropertyByIdx(ui->comboBox_7, 4).toDouble(); }
bool    ChannelManagerWindow::getCounterEventValid() const { return !(ui->comboBox_6->currentText().contains("None") || ui->comboBox_7->currentText().contains("None")); }

QString ChannelManagerWindow::getVoltageAcquisitionHardwareChannel() const  { return getComboPropertyByIdx(ui->comboBox, 1); }
QString ChannelManagerWindow::getVoltageTimebaseHardwareChannel() const  { return getComboPropertyByIdx(ui->comboBox_5, 1); }
double  ChannelManagerWindow::getVoltageTimebaseRate() const  { return getComboPropertyByIdx(ui->comboBox_5, 4).toDouble(); }
bool    ChannelManagerWindow::getVoltageEventValid() const { return !(ui->comboBox->currentText().contains("None") || ui->comboBox_5->currentText().contains("None")); }

void ChannelManagerWindow::createNewChannel(const TableView::cols_type& values)
{
    if(std::find_if(getTableView()->getRows().begin(), getTableView()->getRows().end(), [values](const auto& value){ return value[0] == values[0]; }) != getTableView()->getRows().end())
    {
        LOG_ERROR("Could not create channel. Choosen name already exists.");
        return;
    }

    DCS::Utils::BasicString name;
    DCS::Utils::BasicString connector;
    strcpy(name.buffer, values[0].toString().toStdString().c_str());
    strcpy(connector.buffer, values[1].toString().toStdString().c_str());

    if(values[3] == "Analog")
    {
        unsigned char buffer[4096];
        DCS::i32 size = DCS::Registry::SVParams::GetDataFromParams(buffer,
            SV_CALL_DCS_DAQ_NewAIVChannel,
            name,
            connector,
            DCS::DAQ::ChannelRef::SingleEnded,
            DCS::DAQ::ChannelLimits{ -10.0, 10.0 } // NOTE : Use max limits always, maybe change this later (?)
        );
        DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);
    }
    else if(values[3] == "Digital")
    {
        // TODO
    }
    else if(values[3] == "Counter")
    {
        unsigned char buffer[4096];
        DCS::i32 size = DCS::Registry::SVParams::GetDataFromParams(buffer,
            SV_CALL_DCS_DAQ_NewCIChannel,
            name,
            connector,
            DCS::DAQ::ChannelCountRef::CountUp
        );
        DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);
    }
    else if(values[3] == "Generator")
    {
        unsigned char buffer[4096];
        DCS::i32 size = DCS::Registry::SVParams::GetDataFromParams(buffer,
            SV_CALL_DCS_DAQ_NewPTGChannel,
            name,
            connector,
            values[4].toString().toDouble()
        );
        DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);
    }
    // Internal type does not require any server side setup

    table_model->pushRow(values);
}

void ChannelManagerWindow::deleteChannel(const QString& name)
{
    if(std::find_if(getTableView()->getRows().begin(), getTableView()->getRows().end(), [name](const auto& value){ return value[0] == name; }) != getTableView()->getRows().end())
    {
        LOG_MESSAGE("Deleting channel: %s", name.toStdString().c_str());
        DCS::Utils::BasicString name_raw;
        strcpy(name_raw.buffer, name.toStdString().c_str());
        unsigned char buffer[4096];
        DCS::i32 size = DCS::Registry::SVParams::GetDataFromParams(buffer,
            SV_CALL_DCS_DAQ_DeleteChannel,
            name_raw
        );
        DCS::Network::Message::SendSync(DCS::Network::Message::Operation::REQUEST, buffer, size);
    }
}
