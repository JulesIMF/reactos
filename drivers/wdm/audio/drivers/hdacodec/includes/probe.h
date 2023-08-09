/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS Intel HD Audio Codecs driver
 * FILE:            drivers/wdm/audio/drivers/hdacodec/includes/probe.h
 * PURPOSE:         probe.h header.
 * PROGRAMMER:      Ivan Doroshenko
 * HISTORY:
 *                  06 Aug 23   Created
 */

#ifndef _PROBE_H_
#define _PROBE_H_

#include "common.h"


NTSTATUS NTAPI
HDACodecProbeFunctionGroup(IN PHDAUDIO_BUS_INTERFACE BusInterface);

NTSTATUS NTAPI
HDACodecProbeWidget(IN PHDAUDIO_BUS_INTERFACE BusInterface, IN UCHAR Address, IN UCHAR NodeId);

#endif //_PROBE_H_
