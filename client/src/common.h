#pragma once
#include <QString>
#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>
#include <DCS_Network/include/DCS_ModuleNetwork.h>

struct VirtualChannel
{
    QString                 channel_name;
    QString                 task_name;
    DCS::DAQ::ChannelType   channel_type;
    DCS::DAQ::ChannelRef    channel_ref;
    DCS::DAQ::ChannelLimits channel_lim;
};
