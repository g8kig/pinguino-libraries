/**
    All BULK functions should go here
**/

#ifndef USB_BULK_C_
#define USB_BULK_C_

//#include <string.h>
#include <typedef.h>
#include <usb/picUSB.c>
#include <usb/usb_bulk.h>
#include <usb/usb_config.c>

//#ifdef USB_USE_BULK

// Put USB I/O buffers into dual port RAM.
#ifdef __XC8
// CDC specific buffers
volatile u8 BULKRxBuffer[BULK_BULK_OUT_SIZE] @ (0x500);
volatile u8 BULKTxBuffer[BULK_BULK_IN_SIZE] @ (0x540);
#else
#pragma udata usbram5 BULKRxBuffer BULKTxBuffer
volatile u8 BULKRxBuffer[BULK_BULK_OUT_SIZE];
volatile u8 BULKTxBuffer[BULK_BULK_IN_SIZE];
#endif

/**
    Initialize
**/

void BULKInitEndpoint(void)
{
    // BULK Data EP is IN and OUT EP
    USB_BULK_DATA_EP_UEP = EP_OUT_IN | HSHK_EN;

    // now build EP
    EP_OUT_BD(USB_BULK_DATA_EP_NUM).Cnt = sizeof(BULKRxBuffer);
    EP_OUT_BD(USB_BULK_DATA_EP_NUM).ADDR = PTR16(&BULKRxBuffer);
    // USB owns buffer
    EP_OUT_BD(USB_BULK_DATA_EP_NUM).Stat.uc = BDS_UOWN | BDS_DTSEN;

    EP_IN_BD(USB_BULK_DATA_EP_NUM).ADDR = PTR16(&BULKTxBuffer);
    // CPU owns buffer
    EP_IN_BD(USB_BULK_DATA_EP_NUM).Stat.uc = BDS_DTS ;
}

#define BULKavailable() (!EP_OUT_BD(USB_BULK_DATA_EP_NUM).Stat.UOWN) && (EP_OUT_BD(USB_BULK_DATA_EP_NUM).Cnt > 0)

/**
    Function to read a string from USB
    @param buffer Buffer for reading data
    @param lenght Number of bytes to be read
    @return number of bytes acutally read
**/

u8 BULKgets(char *buffer)
{
    u8 i=0;
    u8 length=64;

    if (deviceState != CONFIGURED)
        return 0;

    // Only Process if we own the buffer aka not own by SIE
    if (!EP_OUT_BD(USB_BULK_DATA_EP_NUM).Stat.UOWN)
    {
        // check how much bytes came
        if (length > EP_OUT_BD(USB_BULK_DATA_EP_NUM).Cnt)
            length = EP_OUT_BD(USB_BULK_DATA_EP_NUM).Cnt;
            
        for (i=0; i < EP_OUT_BD(USB_BULK_DATA_EP_NUM).Cnt; i++)
            buffer[i] = BULKRxBuffer[i];

        // clear BDT Stat bits beside DTS and then togle DTS
        EP_OUT_BD(USB_BULK_DATA_EP_NUM).Stat.uc &= 0x40;
        EP_OUT_BD(USB_BULK_DATA_EP_NUM).Stat.DTS = !EP_OUT_BD(USB_BULK_DATA_EP_NUM).Stat.DTS;
        // reset buffer count and handle controll of buffer to USB
        EP_OUT_BD(USB_BULK_DATA_EP_NUM).Cnt = sizeof(BULKRxBuffer);
        EP_OUT_BD(USB_BULK_DATA_EP_NUM).Stat.uc |= BDS_UOWN | BDS_DTSEN;
    }
    // return number of bytes read
    return i;
}

/**
    Function writes string to USB
    atm not more than MAX_SIZE is allowed
    if more is needed transfer must be split up
**/

u8 BULKputs(char *buffer, u8 length)
{
    u8 i=0;

    if (deviceState != CONFIGURED) return 0;

    if (!EP_IN_BD(USB_BULK_DATA_EP_NUM).Stat.UOWN)
    {
        if (length > BULK_BULK_IN_SIZE)
            length = BULK_BULK_IN_SIZE;
        for (i=0; i < length; i++)
            BULKTxBuffer[i] = buffer[i];

        // Set counter to num bytes ready for send
        EP_IN_BD(USB_BULK_DATA_EP_NUM).Cnt = i;
        // clear BDT Stat bits beside DTS and then togle DTS
        EP_IN_BD(USB_BULK_DATA_EP_NUM).Stat.uc &= 0x40;
        EP_IN_BD(USB_BULK_DATA_EP_NUM).Stat.DTS = !EP_IN_BD(USB_BULK_DATA_EP_NUM).Stat.DTS;
        // reset Buffer to original state
        EP_IN_BD(USB_BULK_DATA_EP_NUM).Stat.uc |= BDS_UOWN | BDS_DTSEN;
    }
    return i;
}

//#endif /* USB_USE_BULK */

#endif /* USB_BULK_C_  */
