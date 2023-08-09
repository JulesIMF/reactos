/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS Intel HD Audio Codecs driver
 * FILE:            drivers/wdm/audio/drivers/hdacodec/includes/common.h
 * PURPOSE:         Common functionality header.
 * PROGRAMMER:      Ivan Doroshenko
 * HISTORY:
 *                  06 Aug 23   Created
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#define PC_IMPLEMENTATION 1

//
// Get the NTDDK headers instead of the WDM headers that portcls.h
// wants to include.
//
#define WIN9X_COMPAT_SPINLOCK

#ifdef UNDER_NT
#ifdef __cplusplus
extern "C"
{
#endif
#include <ntddk.h>
#ifdef __cplusplus
} // extern "C"
#endif
#endif

#define HDA_CODEC_H
#include "../../../hdaudbus/hdaudbus.h"
#include <hdaudio.h>
#include <ks.h>
#include <portcls.h>
#include <stdunk.h>

#include "debug.h"
#include "hda_defs.h"

#ifdef __REACTOS__
#endif

NTSTATUS
NTAPI
HDACodecSendSingleVerb(IN PHDAUDIO_BUS_INTERFACE   BusInterface,
                       IN UCHAR                    Address,
                       IN UCHAR                    NodeId,
                       IN ULONG                    VerbId,
                       IN ULONG                    Payload,
                       OUT PHDAUDIO_CODEC_RESPONSE Response);

#endif //_COMMON_H_
