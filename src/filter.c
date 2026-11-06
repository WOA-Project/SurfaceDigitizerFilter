// Copyright (c) Microsoft Corporation. All Rights Reserved.
// Copyright (c) DuoWoA authors. All Rights Reserved.

#include "filter.h"
#include <filter.tmh>

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, OnDeviceAdd)
#pragma alloc_text (PAGE, OnInternalDeviceControl)
#pragma alloc_text (PAGE, OnContextCleanup)
#endif

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.

Arguments:

    DriverObject - pointer to the driver object
    RegistryPath - pointer to a unicode string representing the path,
                   to driver-specific key in the registry.

Return Value:

    STATUS_SUCCESS if successful, error code otherwise.

--*/
{
    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;

    //
    // Initialize tracing via WPP
    //
    WPP_INIT_TRACING(DriverObject, RegistryPath);

    //
    // Create a framework driver object
    //
    WDF_DRIVER_CONFIG_INIT(&config, OnDeviceAdd);

    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = OnContextCleanup;

    status = WdfDriverCreate(
        DriverObject,
        RegistryPath,
        &attributes,
        &config,
        WDF_NO_HANDLE
    );

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error creating WDF driver object - 0x%08lX",
            status);

        WPP_CLEANUP(DriverObject);

        return status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OnDeviceAdd(
    IN WDFDRIVER Driver,
    IN PWDFDEVICE_INIT DeviceInit
)
/*++

Routine Description:

    OnDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager when a device is found. We create and
    initialize a WDF device object to represent the new instance of
    an touch device. Per-device objects are also instantiated.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry
    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS indicating success or failure

--*/
{
    WDFDEVICE device;
    WDF_IO_QUEUE_CONFIG queueConfig;
    NTSTATUS status;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    //
    // Tell the framework that you are filter driver. Framework
    // takes care of inherting all the device flags & characterstics
    // from the lower device you are attaching to.
    //
    WdfFdoInitSetFilter(DeviceInit);

    status = WdfDeviceCreate(
        &DeviceInit,
        WDF_NO_OBJECT_ATTRIBUTES,
        &device);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "WdfDeviceCreate failed - 0x%08lX",
            status);

        return status;
    }

    //
    // Create a parallel dispatch queue to handle requests from HID Class
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
        &queueConfig,
        WdfIoQueueDispatchParallel);

    queueConfig.EvtIoInternalDeviceControl = OnInternalDeviceControl;

    status = WdfIoQueueCreate(
        device,
        &queueConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        WDF_NO_HANDLE); // pointer to default queue

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error creating WDF default queue - 0x%08lX",
            status);

        return status;
    }

    return STATUS_SUCCESS;
}

VOID
OnInternalDeviceControl(
    IN WDFQUEUE      Queue,
    IN WDFREQUEST    Request,
    IN size_t        OutputBufferLength,
    IN size_t        InputBufferLength,
    IN ULONG         IoControlCode
    )
/*++

Routine Description:

    This routine is the dispatch routine for internal device control requests.

Arguments:

    Queue - Handle to the framework queue object that is associated
            with the I/O request.
    Request - Handle to a framework request object.

    OutputBufferLength - length of the request's output buffer,
                        if an output buffer is available.
    InputBufferLength - length of the request's input buffer,
                        if an input buffer is available.

    IoControlCode - the driver-defined or system-defined I/O control code
                    (IOCTL) that is associated with the request.

Return Value:

   VOID

--*/
{
    NTSTATUS status;
    WDFDEVICE device;
    BOOLEAN requestSent = TRUE;
    WDF_REQUEST_SEND_OPTIONS options;
    WDFMEMORY memory;
    WDFIOTARGET Target;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    PAGED_CODE();

    device = WdfIoQueueGetDevice(Queue);
    Target = WdfDeviceGetIoTarget(device);

    //
    // Please note that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl. So depending on the ioctl code, we will either
    // use retreive function or escape to WDM to get the UserBuffer.
    //

    //
    // Forward the request down. WdfDeviceGetIoTarget returns
    // the default target, which represents the device attached to us below in
    // the stack.
    //

    //
    // Obtains the report descriptor for the HID device
    //
    if (IoControlCode ==
        IOCTL_HID_GET_REPORT_DESCRIPTOR)
    {
        //
        // Format the request with the output memory so the completion routine
        // can access the return data in order to cache it into the context area
        //
        status = WdfRequestRetrieveOutputMemory(Request, &memory);

        if (!NT_SUCCESS(status)) {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "WdfRequestRetrieveOutputMemory failed: 0x%x\n",
                status);

            WdfRequestComplete(Request, status);
            return;
        }

        status = WdfIoTargetFormatRequestForInternalIoctl(
            Target, 
            Request,
            IoControlCode,
            NULL,
            NULL,
            memory,
            NULL);

        if (!NT_SUCCESS(status)) {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "WdfIoTargetFormatRequestForInternalIoctl failed: 0x%x\n",
                status);

            WdfRequestComplete(Request, status);
            return;
        }

        //
        // Set our completion routine with a context area that we will save
        // the output data into
        //
        WdfRequestSetCompletionRoutine(
            Request,
            OnRequestCompletionRoutine,
            WDF_NO_CONTEXT);

        requestSent = WdfRequestSend(
            Request,
            Target,
            WDF_NO_SEND_OPTIONS);
    }
    else
    {
        //
        // We are not interested in post processing the IRP so
        // fire and forget.
        //
        WDF_REQUEST_SEND_OPTIONS_INIT(
            &options,
            WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

        requestSent = WdfRequestSend(Request, Target, &options);
    }

    if (requestSent == FALSE) {
        status = WdfRequestGetStatus (Request);

        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "WdfRequestSend failed: 0x%x\n",
            status);

        WdfRequestComplete(Request, status);
    }

    return;
}

#define MAX_HID_REPORT_DESCRIPTOR_SIZE 4096

VOID
OnRequestCompletionRoutine (
    IN WDFREQUEST  Request,
    IN WDFIOTARGET  Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS  Params,
    IN WDFCONTEXT  Context
    )
/*++

Routine Description:

    Completion Routine

Arguments:

    Target - Target handle
    Request - Request handle
    Params - request completion params
    Context - Driver supplied context


Return Value:

    VOID

--*/
{
    WDFMEMORY memory = NULL;
    NTSTATUS    status = Params->IoStatus.Status;

    ULONG reportDescriptorLength = 0;
    PUCHAR reportDescriptor = NULL;
    ULONG i = 0;

    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Context);

    if (!NT_SUCCESS(status))
    {
        goto exit;
    }

    if (Params->Type != WdfRequestTypeDeviceControlInternal)
    {
        goto exit;
    }

    NT_ASSERT(
        Params->Parameters.Ioctl.IoControlCode ==
        IOCTL_HID_GET_REPORT_DESCRIPTOR);

    if (Params->Parameters.Ioctl.IoControlCode !=
        IOCTL_HID_GET_REPORT_DESCRIPTOR)
    {
        goto exit;
    }

    memory = Params->Parameters.Ioctl.Output.Buffer;

    if (memory == NULL)
    {
        status = STATUS_INVALID_PARAMETER;
        goto exit;
    }

    reportDescriptorLength = (ULONG)Params->Parameters.Ioctl.Output.Length;

    NT_ASSERT(reportDescriptorLength <= MAX_HID_REPORT_DESCRIPTOR_SIZE);

    if (reportDescriptorLength == 0 ||
        reportDescriptorLength > MAX_HID_REPORT_DESCRIPTOR_SIZE)
    {
        status = STATUS_INVALID_BUFFER_SIZE;
        goto exit;
    }

    size_t bufferSize;
    if (WdfMemoryGetBuffer(
            memory,
            &bufferSize) == NULL)
    {
        status = STATUS_INVALID_PARAMETER;
        goto exit;
    }

    NT_ASSERT(bufferSize >= reportDescriptorLength);

    if (reportDescriptorLength > bufferSize)
    {
        status = STATUS_BUFFER_TOO_SMALL;
        goto exit;
    }

    reportDescriptor = (PUCHAR)ExAllocatePoolWithTag(
        NonPagedPoolNx,
        reportDescriptorLength,
        HID_DESCRIPTOR_POOL_TAG);

    if (NULL == reportDescriptor) {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Could not allocate report descriptor buffer!");

        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    status = WdfMemoryCopyToBuffer(
        memory,
        Params->Parameters.Ioctl.Output.Offset,
        reportDescriptor,
        reportDescriptorLength);

    if (!NT_SUCCESS(status)) {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "WdfMemoryCopyToBuffer failed: 0x%x",
            status);

        goto free_buffer;
    }

    for (i = 0; (i + 5) < reportDescriptorLength; i++) {
        if (reportDescriptor[i + 0] == 0x09 &&
            reportDescriptor[i + 1] == 0x3B &&
            reportDescriptor[i + 2] == 0x75 &&
            reportDescriptor[i + 3] == 0x01 &&
            reportDescriptor[i + 4] == 0x95 &&
            reportDescriptor[i + 5] == 0x01)
        {
            reportDescriptor[i + 5] = 0x06;
            break;
        }
    }

    status = WdfMemoryCopyFromBuffer(
        memory,
        0,
        (PVOID)reportDescriptor,
        reportDescriptorLength
    );

    if (!NT_SUCCESS(status)) {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "WdfMemoryCopyFromBuffer failed: 0x%x",
            status);

        goto free_buffer;
    }

free_buffer:

    if (reportDescriptor != NULL)
    {
        ExFreePoolWithTag(
            reportDescriptor,
            HID_DESCRIPTOR_POOL_TAG);
    }

exit:
    WdfRequestComplete(Request, status);
    return;
}

VOID
OnContextCleanup(
    IN WDFOBJECT Driver
    )
/*++
Routine Description:

    Free resources allocated in DriverEntry that are not automatically
    cleaned up framework.

Arguments:

    Driver - handle to a WDF Driver object.

Return Value:

    VOID.

--*/
{
    PAGED_CODE();

    WPP_CLEANUP(WdfDriverWdmGetDriverObject(Driver));
}