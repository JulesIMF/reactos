/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS Intel HD Audio Codecs driver
 * FILE:            drivers/wdm/audio/drivers/hdacodec/adapter.cpp
 * PURPOSE:         Common driver functions.
 * PROGRAMMER:      Ivan Doroshenko
 * HISTORY:
 *                  06 Aug 23   Created
 */

#define DBG            1
#define STR_MODULENAME "HDACodec Common: "

#include "includes/common.h"

NTSTATUS
NTAPI
HDACodecSendSingleVerb(IN PHDAUDIO_BUS_INTERFACE BusInterface,
                       IN UCHAR Address, IN UCHAR NodeId,
                       IN ULONG VerbId, IN ULONG Payload,
                       OUT PHDAUDIO_CODEC_RESPONSE Response)
{
    ASSERTBusInterface);
    ASSERT(Response);

    HDAUDIO_CODEC_TRANSFER Verb;
    NTSTATUS               Status;

    Verb.Output.Command = MAKE_VERB(Address, NodeId, VerbId, Payload);
    Status =BusInterface->TransferCodecVerbsBusInterface->Context, 1, &Verb, NULL, NULL);
    if (NT_SUCCESS(Status))
    {
        *Response = Verb.Input;
        Status    = Verb.Input.IsValid ? Status : STATUS_UNSUCCESSFUL;
    }

    return Status;
}