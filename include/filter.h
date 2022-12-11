// Copyright (c) Microsoft Corporation. All Rights Reserved.
// Copyright (c) DuoWoA authors. All Rights Reserved. 

#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <wdmsec.h> // for SDDLs
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <hidport.h>
#include <trace.h>

typedef struct _DEVICE_EXTENSION
{
    WDFDEVICE WdfDevice;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DEVICE_CONTEXT_CLEANUP OnContextCleanup;

EVT_WDF_DRIVER_DEVICE_ADD OnDeviceAdd;

EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL OnInternalDeviceControl;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_EXTENSION, GetDeviceContext)