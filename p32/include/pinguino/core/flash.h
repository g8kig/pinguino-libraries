/***********************************************************************
    Title:  Pinguino Flash Memory Operations
    File:   flash.h
    Descr.: flash operations for supported PIC32MX
    Author: R�gis Blanchot <rblanchot@gmail.com>

    This file is part of Pinguino (http://www.pinguino.cc)
    Released under the LGPL license (http://www.gnu.org/licenses/lgpl.html)
***********************************************************************/

#ifndef _FLASH_H_
#define _FLASH_H_

#include "typedefs.h"

// The Flash page size is
// - 1 KB on PIC32MX-1XX/2XX devices
// - 4 KB on PIC32MX-3XX/7XX devices

#if defined(__32MX220F032D__) || defined(__32MX220F032B__) || \
    defined(__32MX250F128B__) || defined(__32MX270F256B__)
#define FLASH_PAGE_SIZE                 0x400
#else
#define FLASH_PAGE_SIZE                 0x1000
#endif

// PIC32MX270F256B issues
// - BMXPFMSZ returns 512K instead of 256K
// - BMXDRMSZ returns 128K instead of 64K

#if defined(__32MX270F256B__)
#define FLASH_TOTAL_LENGTH              (BMXPFMSZ/2)
#else
#define FLASH_TOTAL_LENGTH              BMXPFMSZ
#endif

#define FLASH_NOP                       0      // NOP operation
#define FLASH_WORD_WRITE                1      // Word program operation
#define FLASH_ROW_WRITE                 3      // Row program operation
#define FLASH_PAGE_ERASE                4      // Page erase operation
#define FLASH_ALL_ERASE                 5      // Program Flash Memory erase operation

u8 FlashOperation(u8);
u8 FlashErasePage(void*);
u8 FlashWriteWord(void*, u32);
u8 FlashWriteRow(void*, void*);

#define FlashError()        (NVMCON & (_NVMCON_WRERR_MASK | _NVMCON_LVDERR_MASK))
#define FlashClearError()   FlashOperation(FLASH_NOP)
#define FlashGetSize()      (FLASH_TOTAL_LENGTH)

/*******************************************************************
 * To translate the kernel address (KSEG0 or KSEG1) to a physical address,
 * perform a "Bitwise AND" operation of the virtual address with 0x1FFFFFFF:
 * Physical Address = Virtual Address and 0x1FFFFFFF
 *******************************************************************/

#define KVA_TO_PA(va)       ( (u32) (va) & 0x1FFFFFFF )
#define ConvertToPhysicalAddress(a) KVA_TO_PA(a)

/*
//static inline void * ConvertToPhysicalAddress (volatile void *addr);
static inline u32 ConvertToPhysicalAddress (volatile void *addr)
{
    u32 virt = (u32) addr;
    u32 phys;

    // kseg
    if (virt & 0x80000000)
    {
        // kseg2 or kseg3 - no mapping
        if (virt & 0x40000000)
            phys = virt;

        // kseg0� kseg1, cut bits A[31:29]
        else
            phys = virt & 0x1fffffff;
    }

    // kuseg
    else
        phys = virt + 0x40000000;

    //return (void*) phys;
    return phys;
}
*/

/*******************************************************************
 * For physical address to KSEG0 virtual address translation,
 * perform a "Bitwise OR" operation of the physical address with 0x80000000:
 * KSEG0 Virtual Address = Physical Address | 0x80000000
 *******************************************************************/

#define PA_TO_KVA0(pa)      ( (u32) (pa)  | 0x80000000 )
#define ConvertFlashToVirtualAddress(a)  PA_TO_KVA0(a)

/*******************************************************************
 * For physical address to KSEG1 virtual address translation,
 * perform a "Bitwise OR" operation of the physical address with 0xA0000000:
 * KSEG1 Virtual Address = Physical Address | 0xA0000000
 ******************************************************************/

#define PA_TO_KVA1(pa)      ( (u32) (pa)  | 0xA0000000 )
#define ConvertRAMToVirtualAddress(a)  PA_TO_KVA1(a)

/*******************************************************************
 * To translate from KSEG0 to KSEG1 virtual address,
 * perform a "Bitwise OR" operation of the KSEG0 virtual address with 0x20000000:
 * KSEG1 Virtual Address = KSEG0 Virtual Address | 0x20000000
 *******************************************************************/

#define KVA0_TO_KVA1(va)   ( (u32) (va) |  0x20000000 )
#define KVA1_TO_KVA0(va)   ( (u32) (va) & ~0x20000000 )

#endif /* _FLASH_H_ */
