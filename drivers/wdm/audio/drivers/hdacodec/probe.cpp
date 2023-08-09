/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS Intel HD Audio Codecs driver
 * FILE:            drivers/wdm/audio/drivers/hdacodec/adapter.cpp
 * PURPOSE:         Codec probing helpers.
 * PROGRAMMER:      Ivan Doroshenko
 * HISTORY:
 *                  06 Aug 23   Created
 */

#define DBG            1
#define STR_MODULENAME "HDACodec Probe: "

#include "includes/probe.h"

NTSTATUS
NTAPI
HDACodecProbeWidget(IN PHDAUDIO_BUS_INTERFACE BusInterface,
                    IN UCHAR Address, IN UCHAR NodeId)
{
    NTSTATUS               Status = STATUS_SUCCESS;
    ULONG                  Capabilities;
    ULONG                  Type;
    HDAUDIO_CODEC_RESPONSE Response;
    UCHAR                  ConnectionListLength;
    UCHAR                  ConnectionListThresh;
    BOOL                   IsListLong;
    BOOL                   IsRangedEntry;
    USHORT                 Connection;
    USHORT                 ConnectionPrev;
    USHORT                 ConnectionValue;
    USHORT                 ConnectionPrevValue;

    DOUT(DBG_PRINT, ("Node %x \n", NodeId));

    // 1. Get widget capabilities
    Status = HDACodecSendSingleVerb(BusInterface, Address, NodeId,
                                    VID_GET_PARAMETER,
                                    PID_AUDIO_WIDGET_CAP, &Response);
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    Capabilities = Response.Response;
    Type         = AUDIO_CAP_TYPE(Capabilities);

    DOUT(DBG_PRINT, ("\tCaps: %x, widget type: %x (%s)", Capabilities,
                     Type, HDACodecGetWidgetName(Type)));

    // 2. Get connections
    if (TRUE)
    {
        Response.Response = 0; // For case if unsupported
        Status            = HDACodecSendSingleVerb(
            BusInterface, Address, NodeId, VID_GET_PARAMETER,
            PID_CONNECTION_LIST_LENGTH, &Response);

        if (!NT_SUCCESS(Status))
        {
            return Status;
        }

        ConnectionListLength =
            CONNECTION_LIST_ACTUAL(Response.Response);
        IsListLong = CONNECTION_LIST_IS_LONG(Response.Response);
        ConnectionListThresh = IsListLong ? 2 : 4;

        DOUT(DBG_PRINT,
             ("\tList connections (%x):", ConnectionListLength));

        Connection      = 0;
        ConnectionValue = 0;

        for (UCHAR Index = 0, ConnIndex = 0;
             ConnIndex < ConnectionListLength; ConnIndex++)
        {
            //
            // Entries are pulled in multiples of 2 or 4 depending on
            // entry length.
            //
            if (Index % ConnectionListThresh == 0)
            {
                Status = HDACodecSendSingleVerb(
                    BusInterface, Address, NodeId,
                    VID_GET_CONNECTION_LIST_ENTRY, ConnIndex,
                    &Response);

                if (!NT_SUCCESS(Status))
                {
                    return Status;
                }
            }

            ConnectionPrev      = Connection;
            ConnectionPrevValue = ConnectionValue;

            if (IsListLong)
            {
                Connection = CONNECTION_LIST_ENTRY_LONG(
                    Response.Response, ConnIndex % 2);
                IsRangedEntry =
                    ConnIndex > 0 &&
                    (ConnectionPrev &
                     CONNECTION_LIST_ENTRY_LONG_IS_RANGE) == 0 &&
                    Connection & CONNECTION_LIST_ENTRY_LONG_IS_RANGE;

                ConnectionValue =
                    CONNECTION_LIST_ENTRY_LONG_VALUE(Connection);
            }
            else
            {
                Connection = CONNECTION_LIST_ENTRY_SHORT(
                    Response.Response, ConnIndex % 4);
                IsRangedEntry =
                    ConnIndex > 0 &&
                    (ConnectionPrev &
                     CONNECTION_LIST_ENTRY_SHORT_IS_RANGE) == 0 &&
                    Connection & CONNECTION_LIST_ENTRY_SHORT_IS_RANGE;

                ConnectionValue =
                    CONNECTION_LIST_ENTRY_SHORT_VALUE(Connection);
            }

            //
            // Do we have a connection list range?
            // The first entry cannot be a range, nor can there be two
            // sequential entries marked as a range.
            //
            if (IsRangedEntry &&
                ConnectionValue > ConnectionPrevValue)
            {
                while (ConnectionValue > ConnectionPrevValue)
                {
                    ConnectionPrevValue++;
                    DOUT(DBG_PRINT, ("\t\t[long]  %x: %x", Index,
                                     ConnectionPrevValue));
                    Index++;
                }
            }
            else
            {
                DOUT(DBG_PRINT,
                     ("\t\t[short] %x: %x", Index, ConnectionValue));
            }

            Index++;
        }
    }

    else
    {
        DOUT(DBG_PRINT, ("\tNo connection list capability!"));
    }

    return Status;
}

NTSTATUS
NTAPI
HDACodecProbeFunctionGroup(IN PHDAUDIO_BUS_INTERFACE BusInterface)
{
    ASSERT(BusInterface);

    NTSTATUS               Status = STATUS_SUCCESS;
    HDAUDIO_CODEC_RESPONSE Response;
    UCHAR                  Address;
    UCHAR                  StartNode;
    UCHAR                  WidgetStart;
    UCHAR                  WidgetCount;

    // 1. Obtain start node information and function group type
    BusInterface->GetResourceInformation(BusInterface->Context,
                                         &Address, &StartNode);
    Status = HDACodecSendSingleVerb(
        BusInterface, Address, StartNode, VID_GET_PARAMETER,
        PID_FUNCTION_GROUP_TYPE, &Response);

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    DOUT(DBG_PRINT, ("NodeId: %x", StartNode));

    DOUT(DBG_PRINT,
         ("\tFunctional group type: %s",
          HDACodecGetFunctionGroupName(
              Response.Response & FUNCTION_GROUP_NODETYPE_MASK)));

    if ((Response.Response & FUNCTION_GROUP_NODETYPE_MASK) !=
        FUNCTION_GROUP_NODETYPE_AUDIO)
    {
        DOUT(DBG_ERROR, ("Unsupported group type!"));
        return STATUS_UNSUCCESSFUL;
    }

    // 2. Get widgets count
    Status = HDACodecSendSingleVerb(BusInterface, Address, StartNode,
                                    VID_GET_PARAMETER,
                                    PID_SUB_NODE_COUNT, &Response);

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    WidgetStart = SUB_NODE_COUNT_START(Response.Response);
    WidgetCount = SUB_NODE_COUNT_TOTAL(Response.Response);

    // 3. Power up
    Status =
        HDACodecSendSingleVerb(BusInterface, Address, StartNode,
                               VID_SET_POWER_STATE, 0, &Response);

    // 4. Probe widgets
    DOUT(DBG_PRINT, ("Start widgets probing"));
    for (UCHAR WidgetNodeId = WidgetStart;
         WidgetNodeId < WidgetStart + WidgetCount; WidgetNodeId++)
    {
        HDACodecProbeWidget(BusInterface, Address, WidgetNodeId);
    }

    return Status;
}