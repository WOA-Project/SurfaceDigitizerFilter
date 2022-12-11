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

//
// HID Report Descriptor for a touch device
//

const UCHAR gReportDescriptor[] = {
    0x75, 0x08,        // Report Size (8)
    0x15, 0x00,        // Logical Minimum (0)
    0x26, 0xFF, 0x00,  // Logical Maximum (255)
    0x06, 0x0B, 0xFF,  // Usage Page (Vendor Defined 0xFF0B)
    0x09, 0x0B,        // Usage (0x0B)
    0xA1, 0x01,        // Collection (Application)
    0x95, 0x02,        //   Report Count (2)
    0x09, 0x48,        //   Usage (0x48)
    0x85, 0x48,        //   Report ID (72)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x01,        //   Report Count (1)
    0x09, 0xFF,        //   Usage (0xFF)
    0x85, 0x49,        //   Report ID (73)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x0F,        //   Report Count (15)
    0x09, 0x29,        //   Usage (0x29)
    0x85, 0x29,        //   Report ID (41)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x1F,        //   Report Count (31)
    0x09, 0x2A,        //   Usage (0x2A)
    0x85, 0x2A,        //   Report ID (42)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x3E,        //   Report Count (62)
    0x09, 0x2B,        //   Usage (0x2B)
    0x85, 0x2B,        //   Report ID (43)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0xFE,        //   Report Count (-2)
    0x09, 0x2C,        //   Usage (0x2C)
    0x85, 0x2C,        //   Report ID (44)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x96, 0xFE, 0x01,  //   Report Count (510)
    0x09, 0x2D,        //   Usage (0x2D)
    0x85, 0x2D,        //   Report ID (45)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x0F,        //   Report Count (15)
    0x09, 0x2E,        //   Usage (0x2E)
    0x85, 0x2E,        //   Report ID (46)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x1F,        //   Report Count (31)
    0x09, 0x2F,        //   Usage (0x2F)
    0x85, 0x2F,        //   Report ID (47)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x3E,        //   Report Count (62)
    0x09, 0x30,        //   Usage (0x30)
    0x85, 0x30,        //   Report ID (48)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0xFE,        //   Report Count (-2)
    0x09, 0x31,        //   Usage (0x31)
    0x85, 0x31,        //   Report ID (49)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x96, 0xFE, 0x01,  //   Report Count (510)
    0x09, 0x32,        //   Usage (0x32)
    0x85, 0x32,        //   Report ID (50)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x96, 0xFE, 0x03,  //   Report Count (1022)
    0x09, 0x33,        //   Usage (0x33)
    0x85, 0x33,        //   Report ID (51)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x96, 0xFE, 0x07,  //   Report Count (2046)
    0x09, 0x34,        //   Usage (0x34)
    0x85, 0x34,        //   Report ID (52)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x96, 0xFE, 0x0F,  //   Report Count (4094)
    0x09, 0x35,        //   Usage (0x35)
    0x85, 0x35,        //   Report ID (53)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x96, 0xFE, 0x0D,  //   Report Count (3582)
    0x09, 0x36,        //   Usage (0x36)
    0x85, 0x36,        //   Report ID (54)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x96, 0x3F, 0x1D,  //   Report Count (7487)
    0x09, 0x37,        //   Usage (0x37)
    0x85, 0x37,        //   Report ID (55)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x02,        //   Report Count (2)
    0x09, 0xFE,        //   Usage (0xFE)
    0x85, 0xFE,        //   Report ID (-2)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x38,        //   Usage (0x38)
    0x85, 0x38,        //   Report ID (56)
    0xA1, 0x02,        //   Collection (Logical)
    0x09, 0x39,        //     Usage (0x39)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x3A,        //     Usage (0x3A)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x3B,        //     Usage (0x3B)

    //0x75, 0x01,        //     Report Size (1)
    //0x95, 0x01,        //     Report Count (1)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x06,        //     Report Count (6)

    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //   End Collection
    0xC0,              // End Collection
    0x15, 0x00,        // Logical Minimum (0)
    0x27, 0xFF, 0xFF, 0x00, 0x00,  // Logical Maximum (65534)
    0x05, 0x0D,        // Usage Page (Digitizer)
    0x09, 0x0F,        // Usage (0x0F)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x07,        //   Report ID (7)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3D,        //   Report Count (61)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x08,        //   Report ID (8)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0xFD,        //   Report Count (-3)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x0A,        //   Report ID (10)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0xE5, 0x03,  //   Report Count (997)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x0B,        //   Report ID (11)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0x01, 0x06,  //   Report Count (1537)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x0C,        //   Report ID (12)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0x19, 0x07,  //   Report Count (1817)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x0D,        //   Report ID (13)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0xCD, 0x07,  //   Report Count (1997)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x12,        //   Report ID (18)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0xA9, 0x0D,  //   Report Count (3497)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x1A,        //   Report ID (26)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0xF1, 0x10,  //   Report Count (4337)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x1C,        //   Report ID (28)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0x3D, 0x1D,  //   Report Count (7485)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x06,        //   Report ID (6)
    0x09, 0x63,        //   Usage (0x63)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x77,        //   Report Count (119)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x05,        //   Report ID (5)
    0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
    0x09, 0xC8,        //   Usage (0xC8)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x09,        //   Report ID (9)
    0x09, 0xC9,        //   Usage (0xC9)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3F,        //   Report Count (63)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x11,        //   Report ID (17)
    0x09, 0x59,        //   Usage (0x59)
    0x95, 0x3F,        //   Report Count (63)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
    0x05, 0x0D,        // Usage Page (Digitizer)
    0x09, 0x0F,        // Usage (0x0F)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x39,        //   Report ID (57)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3D,        //   Report Count (61)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x3A,        //   Report ID (58)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0xFD,        //   Report Count (-3)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x3C,        //   Report ID (60)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0xE5, 0x03,  //   Report Count (997)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x3D,        //   Report ID (61)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0x01, 0x06,  //   Report Count (1537)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x3E,        //   Report ID (62)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0x19, 0x07,  //   Report Count (1817)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x3F,        //   Report ID (63)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0xCD, 0x07,  //   Report Count (1997)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x44,        //   Report ID (68)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0xA9, 0x0D,  //   Report Count (3497)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x4C,        //   Report ID (76)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0xF1, 0x10,  //   Report Count (4337)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x4E,        //   Report ID (78)
    0x09, 0x56,        //   Usage (0x56)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x61,        //   Usage (0x61)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0x3D, 0x1D,  //   Report Count (7485)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x0E,        //   Report ID (14)
    0x09, 0x63,        //   Usage (0x63)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x77,        //   Report Count (119)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x41,        //   Report ID (65)
    0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
    0x09, 0xC8,        //   Usage (0xC8)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x3B,        //   Report ID (59)
    0x09, 0xC9,        //   Usage (0xC9)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3F,        //   Report Count (63)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
    0x06, 0x0F, 0xFF,  // Usage Page (Vendor Defined 0xFF0F)
    0x09, 0x50,        // Usage (0x50)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x1F,        //   Report ID (31)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3C,        //   Report Count (60)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x09, 0x60,        //   Usage (0x60)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x1F,        //   Report ID (31)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3C,        //   Report Count (60)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x09, 0x61,        //   Usage (0x61)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x21,        //   Report ID (33)
    0x75, 0x20,        //   Report Size (32)
    0x95, 0x01,        //   Report Count (1)
    0x17, 0x00, 0x00, 0x00, 0x80,  //   Logical Minimum (-2147483649)
    0x27, 0xFF, 0xFF, 0xFF, 0x7F,  //   Logical Maximum (2147483646)
    0x09, 0x66,        //   Usage (0x66)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x67,        //   Usage (0x67)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x68,        //   Usage (0x68)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x69,        //   Usage (0x69)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x22,        //   Report ID (34)
    0x75, 0x20,        //   Report Size (32)
    0x95, 0x01,        //   Report Count (1)
    0x17, 0x00, 0x00, 0x00, 0x80,  //   Logical Minimum (-2147483649)
    0x27, 0xFF, 0xFF, 0xFF, 0x7F,  //   Logical Maximum (2147483646)
    0x09, 0x72,        //   Usage (0x72)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x73,        //   Usage (0x73)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x74,        //   Usage (0x74)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x75,        //   Usage (0x75)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x22,        //   Report ID (34)
    0x75, 0x20,        //   Report Size (32)
    0x95, 0x01,        //   Report Count (1)
    0x17, 0x00, 0x00, 0x00, 0x80,  //   Logical Minimum (-2147483649)
    0x27, 0xFF, 0xFF, 0xFF, 0x7F,  //   Logical Maximum (2147483646)
    0x09, 0x7A,        //   Usage (0x7A)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x7B,        //   Usage (0x7B)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x7C,        //   Usage (0x7C)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x7D,        //   Usage (0x7D)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x23,        //   Report ID (35)
    0x75, 0x20,        //   Report Size (32)
    0x95, 0x01,        //   Report Count (1)
    0x17, 0x00, 0x00, 0x00, 0x80,  //   Logical Minimum (-2147483649)
    0x27, 0xFF, 0xFF, 0xFF, 0x7F,  //   Logical Maximum (2147483646)
    0x09, 0x86,        //   Usage (0x86)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x87,        //   Usage (0x87)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x88,        //   Usage (0x88)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x89,        //   Usage (0x89)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
    0x06, 0x0F, 0xFF,  // Usage Page (Vendor Defined 0xFF0F)
    0x09, 0x60,        // Usage (0x60)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x19,        //   Report ID (25)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x04,        //   Report Count (4)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x09, 0x00,        //   Usage (0x00)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x19,        //   Report ID (25)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x04,        //   Report Count (4)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x09, 0xC6,        //   Usage (0xC6)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x05, 0x0D,        // Usage Page (Digitizer)
    0x09, 0x02,        // Usage (Pen)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x09, 0x20,        //   Usage (Stylus)
    0x35, 0x00,        //   Physical Minimum (0)
    0xA1, 0x00,        //   Collection (Physical)
    0x09, 0x32,        //     Usage (In Range)
    0x09, 0x42,        //     Usage (Tip Switch)
    0x09, 0x44,        //     Usage (Barrel Switch)
    0x09, 0x3C,        //     Usage (Invert)
    0x09, 0x45,        //     Usage (Eraser)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x05,        //     Report Count (5)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //     Usage (X)
    0x75, 0x10,        //     Report Size (16)
    0x95, 0x01,        //     Report Count (1)
    0xA4,              //     Push
    0x55, 0x0E,        //       Unit Exponent (-2)
    0x65, 0x11,        //       Unit (System: SI Linear, Length: Centimeter)
    0x46, 0x26, 0x0A,  //       Physical Maximum (2598)
    0x26, 0x80, 0x25,  //       Logical Maximum (9600)
    0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x31,        //       Usage (Y)
    0x46, 0xC4, 0x06,  //       Physical Maximum (1732)
    0x26, 0x20, 0x1C,  //       Logical Maximum (7200)
    0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xB4,              //     Pop
    0x05, 0x0D,        //     Usage Page (Digitizer)
    0x09, 0x30,        //     Usage (Tip Pressure)
    0x26, 0x00, 0x10,  //     Logical Maximum (4096)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xA4,              //     Push
    0x55, 0x0E,        //       Unit Exponent (-2)
    0x65, 0x14,        //       Unit (System: English Rotation, Length: Centimeter)
    0x36, 0xD8, 0xDC,  //       Physical Minimum (-9000)
    0x46, 0x28, 0x23,  //       Physical Maximum (9000)
    0x26, 0x50, 0x46,  //       Logical Maximum (18000)
    0x09, 0x3D,        //       Usage (X Tilt)
    0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x3E,        //       Usage (Y Tilt)
    0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x55, 0x0C,        //       Unit Exponent (-4)
    0x66, 0x01, 0x10,  //       Unit (System: SI Linear, Time: Seconds)
    0x35, 0x00,        //       Physical Minimum (0)
    0x47, 0xFF, 0xFF, 0x00, 0x00,  //       Physical Maximum (65534)
    0x27, 0xFF, 0xFF, 0x00, 0x00,  //       Logical Maximum (65534)
    0x09, 0x56,        //       Usage (0x56)
    0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xB4,              //     Pop
    0x06, 0x00, 0xFF,  //     Usage Page (Vendor Defined 0xFF00)
    0x09, 0x01,        //     Usage (0x01)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
    0x05, 0x0D,        // Usage Page (Digitizer)
    0x09, 0x04,        // Usage (Touch Screen)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x40,        //   Report ID (64)
    0x09, 0x42,        //   Usage (Tip Switch)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x07,        //   Report Count (7)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //   Usage (X)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
    0xA4,              //   Push
    0x55, 0x0E,        //     Unit Exponent (-2)
    0x65, 0x11,        //     Unit (System: SI Linear, Length: Centimeter)
    0x46, 0x76, 0x0B,  //     Physical Maximum (2934)
    0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x31,        //     Usage (Y)
    0x46, 0x74, 0x06,  //     Physical Maximum (1652)
    0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xB4,              //   Pop
    0xC0,              // End Collection
    0x05, 0x0D,        // Usage Page (Digitizer)
    0x09, 0x04,        // Usage (Touch Screen)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x42,        //   Report ID (66)
    0x09, 0x42,        //   Usage (Tip Switch)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x07,        //   Report Count (7)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //   Usage (X)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
    0xA4,              //   Push
    0x55, 0x0E,        //     Unit Exponent (-2)
    0x65, 0x11,        //     Unit (System: SI Linear, Length: Centimeter)
    0x46, 0x76, 0x0B,  //     Physical Maximum (2934)
    0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x31,        //     Usage (Y)
    0x46, 0x74, 0x06,  //     Physical Maximum (1652)
    0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xB4,              //   Pop
    0xC0,              // End Collection
    0x06, 0xF4, 0xFF,  // Usage Page (Vendor Defined 0xFFF4)
    0x09, 0x01,        // Usage (0x01)
    0xA1, 0x01,        // Collection (Application)
    0x09, 0x07,        //   Usage (0x07)
    0xA1, 0x02,        //   Collection (Logical)
    0x85, 0x54,        //     Report ID (84)
    0x06, 0x0F, 0xFF,  //     Usage Page (Vendor Defined 0xFF0F)
    0x09, 0x50,        //     Usage (0x50)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x04,        //     Report Count (4)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0xFF,        //     Logical Maximum (-1)
    0x82, 0x02, 0x01,  //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Buffered Bytes)
    0x06, 0xF4, 0xFF,  //     Usage Page (Vendor Defined 0xFFF4)
    0x09, 0x02,        //     Usage (0x02)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x06,        //     Report Count (6)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0xFF,        //     Logical Maximum (-1)
    0x82, 0x02, 0x01,  //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Buffered Bytes)
    0x09, 0x08,        //     Usage (0x08)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x07,        //     Report Size (7)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0x09, 0x05,        //   Usage (0x05)
    0xA1, 0x02,        //   Collection (Logical)
    0x85, 0x55,        //     Report ID (85)
    0x06, 0x0F, 0xFF,  //     Usage Page (Vendor Defined 0xFF0F)
    0x09, 0x50,        //     Usage (0x50)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x04,        //     Report Count (4)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0xFF,        //     Logical Maximum (-1)
    0x92, 0x02, 0x01,  //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile,Buffered Bytes)
    0x06, 0xF4, 0xFF,  //     Usage Page (Vendor Defined 0xFFF4)
    0x09, 0x02,        //     Usage (0x02)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x06,        //     Report Count (6)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0xFF,        //     Logical Maximum (-1)
    0x92, 0x02, 0x01,  //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile,Buffered Bytes)
    0x09, 0x08,        //     Usage (0x08)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x75, 0x07,        //     Report Size (7)
    0x95, 0x01,        //     Report Count (1)
    0x91, 0x01,        //     Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x04,        //     Usage (0x04)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x08,        //     Report Count (8)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0xFF,        //     Logical Maximum (-1)
    0x92, 0x02, 0x01,  //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile,Buffered Bytes)
    0xC0,              //   End Collection
    0x09, 0x06,        //   Usage (0x06)
    0xA1, 0x02,        //   Collection (Logical)
    0x85, 0x56,        //     Report ID (86)
    0x09, 0x03,        //     Usage (0x03)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x06,        //     Report Count (6)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0xFF,        //     Logical Maximum (-1)
    0xB2, 0x02, 0x01,  //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile,Buffered Bytes)
    0x09, 0x09,        //     Usage (0x09)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x75, 0x07,        //     Report Size (7)
    0x95, 0x01,        //     Report Count (1)
    0xB1, 0x01,        //     Feature (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //   End Collection
    0x09, 0x13,        //   Usage (0x13)
    0xA1, 0x02,        //   Collection (Logical)
    0x85, 0x6E,        //     Report ID (110)
    0x06, 0x0F, 0xFF,  //     Usage Page (Vendor Defined 0xFF0F)
    0x09, 0x50,        //     Usage (0x50)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x04,        //     Report Count (4)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0xFF,        //     Logical Maximum (-1)
    0x82, 0x02, 0x01,  //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Buffered Bytes)
    0x06, 0xF4, 0xFF,  //     Usage Page (Vendor Defined 0xFFF4)
    0x09, 0x02,        //     Usage (0x02)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x06,        //     Report Count (6)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0xFF,        //     Logical Maximum (-1)
    0x82, 0x02, 0x01,  //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Buffered Bytes)
    0x09, 0x08,        //     Usage (0x08)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x11,        //     Usage (0x11)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x12,        //     Usage (0x12)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x05,        //     Report Size (5)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0x09, 0x15,        //   Usage (0x15)
    0xA1, 0x02,        //   Collection (Logical)
    0x85, 0x6F,        //     Report ID (111)
    0x06, 0x0F, 0xFF,  //     Usage Page (Vendor Defined 0xFF0F)
    0x09, 0x50,        //     Usage (0x50)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x04,        //     Report Count (4)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0xFF,        //     Logical Maximum (-1)
    0x92, 0x02, 0x01,  //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile,Buffered Bytes)
    0x06, 0xF4, 0xFF,  //     Usage Page (Vendor Defined 0xFFF4)
    0x09, 0x02,        //     Usage (0x02)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x06,        //     Report Count (6)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0xFF,        //     Logical Maximum (-1)
    0x92, 0x02, 0x01,  //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile,Buffered Bytes)
    0x09, 0x08,        //     Usage (0x08)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x75, 0x07,        //     Report Size (7)
    0x95, 0x01,        //     Report Count (1)
    0x91, 0x01,        //     Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x14,        //     Usage (0x14)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x10,        //     Report Count (16)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0xFF,        //     Logical Maximum (-1)
    0x92, 0x02, 0x01,  //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile,Buffered Bytes)
    0xC0,              //   End Collection
    0x09, 0x16,        //   Usage (0x16)
    0x85, 0x70,        //   Report ID (112)
    0xA1, 0x02,        //   Collection (Logical)
    0x09, 0x10,        //     Usage (0x10)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x22,        //     Usage (0x22)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x75, 0x06,        //     Report Size (6)
    0x95, 0x01,        //     Report Count (1)
    0xB1, 0x01,        //     Feature (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //   End Collection
    0x09, 0x21,        //   Usage (0x21)
    0x85, 0x73,        //   Report ID (115)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x02,        //   Report Count (2)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0xFF,        //   Logical Maximum (-1)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
    0x06, 0x0B, 0xFF,  // Usage Page (Vendor Defined 0xFF0B)
    0x0A, 0x01, 0x01,  // Usage (0x0101)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x5F,        //   Report ID (95)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3C,        //   Report Count (60)
    0x09, 0x65,        //   Usage (0x65)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x60,        //   Report ID (96)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3C,        //   Report Count (60)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x09, 0x60,        //   Usage (0x60)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x60,        //   Report ID (96)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3C,        //   Report Count (60)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x09, 0x61,        //   Usage (0x61)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x60,        //   Report ID (96)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x3C,        //   Report Count (60)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x09, 0x62,        //   Usage (0x62)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x62,        //   Report ID (98)
    0x75, 0x20,        //   Report Size (32)
    0x95, 0x01,        //   Report Count (1)
    0x09, 0x66,        //   Usage (0x66)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x67,        //   Usage (0x67)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x68,        //   Usage (0x68)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x69,        //   Usage (0x69)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x65,        //   Report ID (101)
    0x75, 0x20,        //   Report Size (32)
    0x95, 0x01,        //   Report Count (1)
    0x09, 0x8E,        //   Usage (0x8E)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x8F,        //   Usage (0x8F)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x90,        //   Usage (0x90)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x91,        //   Usage (0x91)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x65,        //   Report ID (101)
    0x75, 0x20,        //   Report Size (32)
    0x95, 0x01,        //   Report Count (1)
    0x09, 0x8A,        //   Usage (0x8A)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x8B,        //   Usage (0x8B)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x8C,        //   Usage (0x8C)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x8D,        //   Usage (0x8D)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x06, 0xA1, 0xFF,  // Usage Page (Vendor Defined 0xFFA1)
    0x09, 0x60,        // Usage (0x60)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x58,        //   Report ID (88)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x09, 0x03,        //   Usage (0x03)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x06, 0x0F, 0xFF,  // Usage Page (Vendor Defined 0xFF0F)
    0x09, 0x51,        // Usage (0x51)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x66,        //   Report ID (102)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x09, 0x01,        //   Usage (0x01)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x09, 0x02,        //   Usage (0x02)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x75, 0x20,        //   Report Size (32)
    0x95, 0x0E,        //   Report Count (14)
    0x09, 0x03,        //   Usage (0x03)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
};
const ULONG gdwcbReportDescriptor = sizeof(gReportDescriptor);

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
    WDF_OBJECT_ATTRIBUTES attributes;
    WDFDEVICE device;
    WDF_IO_QUEUE_CONFIG queueConfig;
    NTSTATUS status;
    PDEVICE_EXTENSION devContext;

    UNREFERENCED_PARAMETER(Driver);
    PAGED_CODE();

    //
    // Tell the framework that you are filter driver. Framework
    // takes care of inherting all the device flags & characterstics
    // from the lower device you are attaching to.
    //
    WdfFdoInitSetFilter(DeviceInit);

    //
    // Create a framework device object. This call will in turn create
    // a WDM device object, attach to the lower stack, and set the
    // appropriate flags and attributes.
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, DEVICE_EXTENSION);

    status = WdfDeviceCreate(&DeviceInit, &attributes, &device);

    if (!NT_SUCCESS(status))
    {
        Trace(
            TRACE_LEVEL_ERROR,
            TRACE_INIT,
            "WdfDeviceCreate failed - 0x%08lX",
            status);

        goto exit;
    }

    devContext = (PDEVICE_EXTENSION)GetDeviceContext(device);
    devContext->WdfDevice = device;

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

NTSTATUS
TchGetReportDescriptor(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request
)
/*++

Routine Description:

	Finds the report descriptor and copies it into the buffer provided by the
	Request.

Arguments:

	Device - Handle to WDF Device Object

	Request - Handle to request object

Return Value:

	NT status code.
	 success - STATUS_SUCCESS
	 failure:
	 STATUS_INVALID_PARAMETER - An invalid parameter was detected.

--*/
{
	WDFMEMORY memory;
	NTSTATUS status;

    UNREFERENCED_PARAMETER(Device);
    
	//
	// This IOCTL is METHOD_NEITHER so WdfRequestRetrieveOutputMemory
	// will correctly retrieve buffer from Irp->UserBuffer. 
	// Remember that HIDCLASS provides the buffer in the Irp->UserBuffer
	// field irrespective of the ioctl buffer type. However, framework is very
	// strict about type checking. You cannot get Irp->UserBuffer by using
	// WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
	// internal ioctl.
	//
	status = WdfRequestRetrieveOutputMemory(Request, &memory);

	if (!NT_SUCCESS(status))
	{
		Trace(
			TRACE_LEVEL_ERROR,
			TRACE_HID,
			"Error getting HID report descriptor request memory - 0x%08lX",
			status);
		goto exit;
	}

	//
	// Use hardcoded Report descriptor
	//
    status = WdfMemoryCopyFromBuffer(
        memory,
        0,
        (PVOID)gReportDescriptor,
        gdwcbReportDescriptor
    );


	if (!NT_SUCCESS(status))
	{
		Trace(
			TRACE_LEVEL_ERROR,
			TRACE_HID,
			"Error copying HID report descriptor to request memory - 0x%08lX",
			status);
		goto exit;
	}

	//
	// Report how many bytes were copied
	//
	WdfRequestSetInformation(Request, gdwcbReportDescriptor);

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
    PDEVICE_EXTENSION devContext;
    WDFDEVICE device;
    BOOLEAN requestSent = TRUE;
    WDF_REQUEST_SEND_OPTIONS options;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    PAGED_CODE();

    device = WdfIoQueueGetDevice(Queue);
    devContext = (PDEVICE_EXTENSION)GetDeviceContext(device);

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
        status = TchGetReportDescriptor(device, Request);
        WdfRequestComplete(Request, status);
        return;

    default:
        break;
    }

    //
    // We are not interested in post processing the IRP so 
    // fire and forget.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(&options,
                                    WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    requestSent = WdfRequestSend(Request, WdfDeviceGetIoTarget(device), &options);

    if (requestSent == FALSE) {
        status = WdfRequestGetStatus (Request);
        Trace(TRACE_LEVEL_ERROR, TRACE_INIT, "WdfRequestSend failed: 0x%x\n", status);
        WdfRequestComplete(Request, status);
    }

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