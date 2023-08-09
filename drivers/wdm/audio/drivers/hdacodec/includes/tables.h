/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS Intel HD Audio Codecs driver
 * FILE:            drivers/wdm/audio/drivers/hdacodec/includes/tables.h
 * PURPOSE:         Miniport topology and wave filters description.
 * PROGRAMMER:      Ivan Doroshenko
 * HISTORY:
 *                  06 Aug 23   Created
 */

#ifndef _TABLES_H_
#define _TABLES_H_

#include "common.h"

/*
    Здесь описывается топология устройства.
    Пока что захардкожена очень простая топология только для воспроизведения
*/
enum KSNODE_TOPO
{
    KSNODE_TOPO_VOLUME,
    KSNODE_TOPO_MUTE
};


enum KSPIN_TOPO
{
    KSPIN_TOPO_WAVEOUT_SOURCE = 0,
    KSPIN_TOPO_LINEOUT_DEST,
};

static PCPROPERTY_ITEM PropertiesFilter[] = {};

DEFINE_PCAUTOMATION_TABLE_PROP(AutomationFilter, PropertiesFilter);

static PCNODE_DESCRIPTOR TopologyNodes[] = {
  // KSNODE_TOPO_VOLUME
    {
     0, // Flags
        &AutomationSpeakerVolume, // AutomationTable
        &KSNODETYPE_VOLUME, // Type
        &KSAUDFNAME_MASTER_VOLUME // Name
    },
 // KSNODE_TOPO_MUTE
    {
     0, // Flags
        &AutomationSpeakerMute,   // AutomationTable
        &KSNODETYPE_MUTE,     // Type
        &KSAUDFNAME_MASTER_MUTE // Name
    }
};

static KSDATARANGE TopologyPinDataRangesBridge[] = {
    {{sizeof(KSDATARANGE),
      0,
      0,
      0,
      {STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO)},
      {STATICGUIDOF(KSDATAFORMAT_SUBTYPE_ANALOG)},
      {STATICGUIDOF(KSDATAFORMAT_SPECIFIER_NONE)}}}};

static PKSDATARANGE TopologyPinDataRangePointersBridge[] = {
    &TopologyPinDataRangesBridge[0]};

static PCPIN_DESCRIPTOR TopologyPins[] = {
  // KSPIN_TOPO_WAVEOUT_SOURCE
    {0,
     0, 0,
     NULL, {0,
      NULL,
      0,
      NULL,
      SIZEOF_ARRAY(TopologyPinDataRangePointersBridge),
      TopologyPinDataRangePointersBridge,
      KSPIN_DATAFLOW_IN,
      KSPIN_COMMUNICATION_NONE,
      &KSCATEGORY_AUDIO,
      NULL,
      {0}}   },
 // KSPIN_TOPO_LINEOUT_DEST
    {0,
    0, 0,
    NULL, 
    {

        0,
        NULL,
        0,
        NULL,
        SIZEOF_ARRAY(TopologyPinDataRangePointersBridge),
        TopologyPinDataRangePointersBridge,
        KSPIN_DATAFLOW_OUT,
        KSPIN_COMMUNICATION_NONE,
        &KSNODETYPE_SPEAKER,
        NULL,
        {0}
    }}
};

static PCCONNECTION_DESCRIPTOR TopologyConnections[] = {
    {PCFILTER_NODE,      KSPIN_TOPO_WAVEOUT_SOURCE, KSNODE_TOPO_VOLUME, 1                      },
    {KSNODE_TOPO_VOLUME, 0,                         KSNODE_TOPO_MUTE,   1                      },
    {KSNODE_TOPO_MUTE,   0,                         PCFILTER_NODE,      KSPIN_TOPO_LINEOUT_DEST}
};

static PCFILTER_DESCRIPTOR TopologyFilterDescriptor = {
    0,                                 // Version
    &AutomationFilter,                 // AutomationTable
    sizeof(PCPIN_DESCRIPTOR),          // PinSize
    SIZEOF_ARRAY(TopologyPins),        // PinCount
    TopologyPins,                      // Pins
    sizeof(PCNODE_DESCRIPTOR),         // NodeSize
    SIZEOF_ARRAY(TopologyNodes),       // NodeCount
    TopologyNodes,                     // Nodes
    SIZEOF_ARRAY(TopologyConnections), // ConnectionCount
    TopologyConnections,               // Connections
    0,                                 // CategoryCount
    NULL};

#endif //_TABLES_H_
