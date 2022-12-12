// Copyright (c) Microsoft Corporation. All Rights Reserved.
// Copyright (c) DuoWoA authors. All Rights Reserved.

#include "filter.h"
#include <filter.tmh>

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, OnDeviceAdd)
#pragma alloc_text (PAGE, OnInternalDeviceControl)
#pragma alloc_text (PAGE, OnContextCleanup)
#pragma alloc_text (PAGE, OnRequestCompletionRoutine)
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
    WDFDRIVER hDriver;

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
        &hDriver
    );

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "Error creating WDF driver object - 0x%08lX",
            status);

        WPP_CLEANUP(DriverObject);

        goto exit;
    }

exit:

    return status;
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

        goto exit;
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

        goto exit;
    }

exit:

    return status;
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
    BOOLEAN forwardWithCompletionRoutine = FALSE;
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

    switch (IoControlCode) {

    case IOCTL_HID_GET_REPORT_DESCRIPTOR:
        //
        // Obtains the report descriptor for the HID device
        //
        forwardWithCompletionRoutine = TRUE;
        break;

    default:
        break;
    }

    //
    // Forward the request down. WdfDeviceGetIoTarget returns
    // the default target, which represents the device attached to us below in
    // the stack.
    //
    if (forwardWithCompletionRoutine) {
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
    WDFMEMORY   memory = Params->Parameters.Ioctl.Output.Buffer;
    NTSTATUS    status = Params->IoStatus.Status;

    ULONG reportDescriptorLength = 0;
    PUCHAR reportDescriptor = NULL;
    ULONG i = 0;
    UCHAR b1, b2, b3, b4, b5, b6 = 0;

    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Context);

    if (NT_SUCCESS(status) &&
        Params->Type == WdfRequestTypeDeviceControlInternal &&
        Params->Parameters.Ioctl.IoControlCode == IOCTL_HID_GET_REPORT_DESCRIPTOR) {

        reportDescriptorLength = (ULONG)Params->Parameters.Ioctl.Output.Length;

        reportDescriptor = (PUCHAR)ExAllocatePoolWithTag(
            NonPagedPoolNx,
            reportDescriptorLength,
            HID_DESCRIPTOR_POOL_TAG);

        if (NULL == reportDescriptor) {
            Trace(
                TRACE_LEVEL_ERROR,
                TRACE_INIT,
                "Could not allocate report descriptor buffer!");

            status = STATUS_UNSUCCESSFUL;
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
                "WdfMemoryCopyToBuffer failed: 0x%x\n",
                status);

            status = STATUS_UNSUCCESSFUL;
            goto free_buffer;
        }

        for (i = 0; i < reportDescriptorLength - 5; i++) {
            b1 = *(reportDescriptor + i);
            b2 = *(reportDescriptor + i + 1);
            b3 = *(reportDescriptor + i + 2);
            b4 = *(reportDescriptor + i + 3);
            b5 = *(reportDescriptor + i + 4);
            b6 = *(reportDescriptor + i + 5);

            // Fix HID descriptor bad alignment problem

            // 0x09, 0x3B,        //     Usage (0x3B)
            // //0x75, 0x01,        //     Report Size (1)
            // //0x95, 0x01,        //     Report Count (1)
            // 0x75, 0x01,        //     Report Size (1)
            // 0x95, 0x06,        //     Report Count (6)

            if (b1 == 0x09 && b2 == 0x3B && 
                b3 == 0x75 && b4 == 0x01 && 
                b5 == 0x95 && b6 == 0x01) {
                *(reportDescriptor + i + 5) = 0x06; // Report Count (6)
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
                "WdfMemoryCopyFromBuffer failed: 0x%x\n",
                status);

            status = STATUS_UNSUCCESSFUL;
            goto free_buffer;
        }

free_buffer:
        ExFreePoolWithTag(reportDescriptor, HID_DESCRIPTOR_POOL_TAG);
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