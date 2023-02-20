//
// The name that is printed in debug output messages
//

#include <ntdef.h>
#include <ntstatus.h>
#include <wdm.h>

#define DBG 1
#define STR_MODULENAME "HDACodec Adapter: "

//
// All the GUIDs from portcls and your own defined GUIDs end up in this object.
//
#define PUT_GUIDS_HERE

#include "includes/adapter.h"

DRIVER_ADD_DEVICE AddDevice;

#ifdef _MSC_VER
#pragma code_seg("PAGE")
#endif

NTSTATUS
NTAPI
HDACodecIoCompletion(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PKEVENT Event)
{
    PAGED_CODE();

    DOUT(DBG_PRINT, ("CmBattIoCompletion: Event (%x)\n", Event));

    // Set the completion event
    KeSetEvent(Event, IO_NO_INCREMENT, FALSE);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS
NTAPI
HDACodecQueryBusInterface(IN PDEVICE_OBJECT DeviceObject, OUT PHDAUDIO_BUS_INTERFACE_V2 BusInterface)
{
    //
    // Variables
    //

    NTSTATUS           Status;
    KEVENT             Event;
    PIO_STACK_LOCATION IoStackLocation;
    PIRP               BusIrp;

    // Request for HDAudBus interface
    BusIrp = IoAllocateIrp(DeviceObject->StackSize, 0);
    if (!BusIrp)
    {
        DOUT(DBG_ERROR, ("Failed to allocate Irp\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    BusIrp->IoStatus.Status = STATUS_NOT_SUPPORTED;

    PHDA_FDO_DEVICE_EXTENSION DeviceExtension = (PHDA_FDO_DEVICE_EXTENSION)(DeviceObject->DeviceExtension);

    // Build the query
    IoStackLocation = IoGetNextIrpStackLocation(BusIrp);
    IoStackLocation->MajorFunction = IRP_MJ_PNP;
    IoStackLocation->MinorFunction = IRP_MN_QUERY_INTERFACE;
    IoStackLocation->Parameters.QueryInterface.InterfaceType = &GUID_HDAUDIO_BUS_INTERFACE_V2;
    IoStackLocation->Parameters.QueryInterface.Size = sizeof(HDAUDIO_BUS_INTERFACE_V2);
    IoStackLocation->Parameters.QueryInterface.Version = 2;
    IoStackLocation->Parameters.QueryInterface.Interface = (PINTERFACE)BusInterface;
    IoStackLocation->Parameters.QueryInterface.InterfaceSpecificData = NULL;

    // Initialize our wait event
    KeInitializeEvent(&Event, SynchronizationEvent, 0);

    // Set the completion routine
    IoSetCompletionRoutine(BusIrp, (PIO_COMPLETION_ROUTINE)HDACodecIoCompletion, &Event, TRUE, TRUE, TRUE);

    // Now call HDAudBus
    PDEVICE_OBJECT LowerDevice = DeviceExtension->LowerDevice;
    Status = IoCallDriver(LowerDevice, BusIrp);
    if (Status == STATUS_PENDING)
    {
        // Wait for completion
        KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
        Status = BusIrp->IoStatus.Status;
    }

    // Free the IRP
    IoFreeIrp(BusIrp);

    return Status;
}

NTSTATUS
NTAPI
StartDevice(
    IN PDEVICE_OBJECT DeviceObject, // Device object.
    IN PIRP           Irp,          // IO request packet.
    IN PRESOURCELIST  ResourceList  // List of hardware resources.
)
{
    //
    // Checks
    //

    PAGED_CODE();

    ASSERT(DeviceObject);
    ASSERT(Irp);
    ASSERT(ResourceList);

    //
    // Variables
    //

    HDAUDIO_BUS_INTERFACE_V2 BusInterface;

    //
    // Code
    //

    BREAK();

    DOUT(DBG_PRINT, ("[StartDevice]"));

    // Probe the codec
    HDACodecQueryBusInterface(DeviceObject, &BusInterface);
    HDACodecProbeFunctionGroup(&BusInterface);

    return STATUS_UNSUCCESSFUL;
}

#ifdef _MSC_VER
#pragma warning(disable : 28152)
#endif
NTSTATUS
NTAPI
AddDevice(IN PDRIVER_OBJECT DriverObject, IN PDEVICE_OBJECT PhysicalDeviceObject)
{
    PAGED_CODE();

    DOUT(DBG_PRINT, ("[AddDevice]"));

    // Tell portcls (the class driver) to add the device
    return PcAddAdapterDevice(DriverObject, PhysicalDeviceObject, (PCPFNSTARTDEVICE)StartDevice, MAX_MINIPORTS, 0);
}

extern "C" DRIVER_INITIALIZE DriverEntry;
extern "C" NTSTATUS NTAPI
DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPathName)
{
    PAGED_CODE();

    DOUT(DBG_PRINT, ("[DriverEntry]"));

    // Tell the class driver to initialize the driver
    NTSTATUS RetValue = PcInitializeAdapterDriver(DriverObject, RegistryPathName, (PDRIVER_ADD_DEVICE)AddDevice);

    return RetValue;
}
