#include "IoctlPipe.h"

IoctlPipe::IoctlPipe(PDRIVER_OBJECT driverObject) : CommunicationPipe("Ioctl"), driverObject(driverObject) {}

CommunicateRoutine ioctlRoutine;

NTSTATUS MessageRecieved(PDEVICE_OBJECT, PIRP irp) {
    PIO_STACK_LOCATION stackLocation = IoGetCurrentIrpStackLocation(irp);

    if (stackLocation->Parameters.DeviceIoControl.IoControlCode == 0x100) {
        PVOID buffer = irp->AssociatedIrp.SystemBuffer;
        SIZE_T size = irp->IoStatus.Information;

        ioctlRoutine(buffer, size);
    } else
        irp->IoStatus.Information = 0;

    irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS Dummy(PDEVICE_OBJECT, PIRP) {
    return STATUS_SUCCESS;
}

NTSTATUS IoctlPipe::Install(CommunicateRoutine routine) {
    ioctlRoutine = routine;

    driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MessageRecieved;
    driverObject->MajorFunction[IRP_MJ_CREATE] = driverObject->MajorFunction[IRP_MJ_CLOSE] = Dummy;

    return STATUS_SUCCESS;
}
