/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS Intel HD Audio Codecs driver
 * FILE:            drivers/wdm/audio/drivers/hdacodec/debug.cpp
 * PURPOSE:         Debugging functions.
 * PROGRAMMER:      Ivan Doroshenko
 * HISTORY:
 *                  06 Aug 23   Created
 */

#define DBG 1
#define STR_MODULENAME "HDACodec Debug: "

//
// We want the global debug variables here
//

#define DEFINE_DEBUG_VARS

#include "includes/debug.h"
#include "includes/hda_defs.h"


PCCH NTAPI
HDACodecGetFunctionGroupName(IN ULONG Type)
{
    switch (Type)
    {
        case FUNCTION_GROUP_NODETYPE_AUDIO:
            return "Audio Function Group";
        case FUNCTION_GROUP_NODETYPE_MODEM:
            return "Modem Function Group";
            
        default:
            return "Unknown";
    }
}

PCCH NTAPI
HDACodecGetWidgetName(IN ULONG Type)
{
    switch (Type)
    {
        case HDAWidgetType::WT_AUDIO_INPUT:
            return "Audio Input";
        case HDAWidgetType::WT_AUDIO_MIXER:
            return "Audio Mixer";
        case HDAWidgetType::WT_AUDIO_OUTPUT:
            return "Audio Output";
        case HDAWidgetType::WT_AUDIO_SELECTOR:
            return "Audio Selector";
        case HDAWidgetType::WT_BEEP_GENERATOR:
            return "Beep Generator";
        case HDAWidgetType::WT_PIN_COMPLEX:
            return "Pin Complex";
        case HDAWidgetType::WT_POWER:
            return "Power";
        case HDAWidgetType::WT_VOLUME_KNOB:
            return "Volume Knob";

        default:
            return "Unknown";
    }
}
