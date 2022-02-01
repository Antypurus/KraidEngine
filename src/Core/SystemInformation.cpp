#include "SystemInformation.h"

#include <intrin.h>

#include <Core/Utils/Log.h>

/*
 *
 * Function 0x1
 *  - EBX
 *      - second byte of ebx ((char*)&ebx)[1]
 *  - ECX
 *      - bit 0 - SSE 3 support
 *      - bit 9 - SSSE3 support
 *      - bit 19 - SSE4.1 support
 *      - bit 20 - SSE4.2 support
 *      - bit 28 - AVX support
 *  - EDX
 *      - bit 3 - 4MB page size support
 *      - bit 18 - CPU serial number support
 *      - bit 23 - Intel MMX support
 *      - bit 25 - SSE support
 *      - bit 26 - SSE2 support
 *
 * Function 0x2
 *  - Gives information about the processor TLB, cache, and prefetch hardware.
 *  - AL register is always 0x1
 *  - Bit 31 of each register indicates if it has valid date (0 -> valid)
 *  - Rest of the information is encoded in bytes and has to be parsed
 *
 * NOTE(Tiago): turns out my assumptions were wrong and all of this only applies to Intel CPUs,
 * the CPUID instruction reports information differently between vendors, with only function 0
 * beign the same as it is used to report the vendor of the cpu
 *
 */

#define BIT(x, b) (x & (0x1 << b)) >> b

#define KB(x) x * 1024
#define MB(x) KB(x) * 1024

namespace Kraid
{

    CPUFeatures::CPUFeatures()
    {
        int32 registers[4];
        __cpuid(registers, 0x1);

        uint8 instruction_cache_line_size = ((uint8*)&registers[1])[1];

        //NOTE(Tiago): check SSE feature supporit
        {
            const bool sse = BIT(registers[3], 25);
            const bool sse2 = BIT(registers[3], 26);
            const bool sse3 = BIT(registers[2], 0);
            const bool ssse3 = BIT(registers[2], 9);
            const bool sse4_1 = BIT(registers[2], 19);
            const bool sse4_2 = BIT(registers[2], 20);

            if(sse4_2)          this->sse = SSESupport::SSE4_2;
            else if(sse4_1)     this->sse = SSESupport::SSE4_1;
            else if(sse3)       this->sse = SSESupport::SSE3;
            else if(sse2)       this->sse = SSESupport::SSE2;
            else if(sse)        this->sse = SSESupport::SSE;
            else                this->sse = SSESupport::None;
        }

        //NOTE(Tiago): check AVX feature support
        {
            bool avx = BIT(registers[2], 28);

            if(avx)     this->avx = AVXSupport::AVX;
            else        this->avx = AVXSupport::None;
        }
        
        bool large_4MB_pages = BIT(registers[3], 3);
        bool cpu_serial_number = BIT(registers[3], 18);
        bool mmx = BIT(registers[3], 23);

        this->ExtractCPUIDFunction0x2Information();
    }

    void CPUFeatures::ExtractCPUIDFunction0x2Information()
    {
        int32 registers[4];
        __cpuid(registers, 0x2);

        registers[0] &= 0xFFFFFF00;//NOTE(Tiago):zero the al register since it doesnt have usefull information

        for(size_t i = 0; i < 4; ++i)
        {
            uint32 value = BIT(registers[i], 31);
            bool valid = value == 0 ? true : false;
            if(valid)
            {
                registers[i] &= 0x7FFFFFFF; //mask off validation bit

                //NOTE(Tiago):parse each of the 4 descriptor in the register
                uint8* descriptor_array = (uint8*)&registers[i];
                for(size_t j = 0; j < 4; ++i)
                {
                    uint8 descriptor = descriptor_array[j];
                    switch(descriptor)
                    {
                        case(0x0)://NULL Descriptor
                        {
                            break;
                        }
                        case(0x01)://Instruction TLB: 4KB page, 4-way set associative, 32 entries
                        {
                            this->instruction_TLB.page_size = KB(4);
                            this->instruction_TLB.entry_count = 32;
                            this->instruction_TLB.set_associativity = 4;
                            break;
                        }
                        case(0x02)://Instruction TLB: 4MB pages, fully-associative, 2 entries
                        {
                            this->instruction_TLB.page_size = MB(4);
                            this->instruction_TLB.entry_count = 2;
                            this->instruction_TLB.set_associativity = FULLY_ASSOCIATIVE_TLB;
                            break;
                        }
                        case(0x03)://Data TLB: 4KB pages, 4-way set associative, 64 entries
                        {
                            this->data_TLB.page_size = KB(4);
                            this->data_TLB.entry_count = 64;
                            this->data_TLB.set_associativity = 4;
                            break;
                        }
                        case(0x04)://Data TLB: 4MB pages, 4-way set associative, 8 entries
                        {
                            this->data_TLB.page_size = MB(4);
                            this->data_TLB.entry_count = 8;
                            this->data_TLB.set_associativity = 4;
                            break;
                        }
                        case(0x05)://Data TLB: 4MB pages, 4-way set associative, 32 entries
                        {
                            this->data_TLB.page_size = MB(4);
                            this->data_TLB.entry_count = 32;
                            this->data_TLB.set_associativity = 4;
                            break;
                        }
                        case(0x06)://L1 I-cache 8KB, 4-way set associative, 32byte cache line
                        {
                            this->L1_icache.cache_size = KB(8);
                            this->L1_icache.line_size = 32;
                            this->L1_icache.set_associativity = 4;
                            break;
                        }
                        case(0x08)://L1 I-cache 16KB, 4-way set associative, 32byte cache line
                        {
                            this->L1_icache.cache_size = KB(16);
                            this->L1_icache.line_size = 32;
                            this->L1_icache.set_associativity = 4;
                            break;
                        }
                        case(0x09)://L1 I-cache 32KB, 4-way set associative, 64byte cache line
                        {
                            this->L1_icache.cache_size = KB(32);
                            this->L1_icache.line_size = 64;
                            this->L1_icache.set_associativity = 4;
                            break;
                        }
                        case(0x0A)://L1 D-cache 8KB, 2-way set associative, 32byte cache line
                        {
                            this->L1_dcache.cache_size = KB(8);
                            this->L1_dcache.line_size = 32;
                            this->L1_dcache.set_associativity = 2;
                            break;
                        }
                        case(0x0B)://Instruction TLB: 4MB pages, 4-way set associative, 4 entries 
                        {
                            this->instruction_TLB.entry_count = 4;
                            this->instruction_TLB.page_size = MB(4);
                            this->instruction_TLB.set_associativity = 4;
                            break;
                        }
                        case(0x0C)://L1 D-cache 16KB, 4-way set associative, 32byte cache line
                        {
                            this->L1_dcache.cache_size = KB(16);
                            this->L1_dcache.line_size = 32;
                            this->L1_dcache.set_associativity = 4;
                            break;
                        }
                        case(0x0D)://L1 D-cache 16KB, 4-way set associative, 64byte cache line
                        {
                            this->L1_dcache.cache_size = KB(160);
                            this->L1_dcache.line_size = 64;
                            this->L1_dcache.set_associativity = 4;
                            break;
                        }
                        case(0x0E)://L1 D-cache 24KB, 6-way associative, 64byte cache line
                        {
                            this->has_l2_cache = true;
                            this->L1_dcache.cache_size = KB(24);
                            this->L1_dcache.line_size = 64;
                            this->L1_dcache.set_associativity = 6;
                            break;
                        }
                        case(0x1D)://L2 d-cache 128KB, 2-way set associative, 64byte cache line
                        {
                            this->has_l2_cache = true;
                            this->L2_dcache.cache_size = KB(128);
                            this->L2_dcache.line_size = 64;
                            this->L2_dcache.set_associativity = 2;
                            break;
                        }
                        case(0x21)://L2 d-cache 256KB, 8-way set associative, 64byte cache line
                        {
                            this->has_l2_cache = true;
                            this->L2_dcache.cache_size = KB(256);
                            this->L2_dcache.line_size = 64;
                            this->L2_dcache.set_associativity = 8;
                            break;
                        }
                        case(0x22)://L3 d-cache 512KB, 4-way set associative, 64bytes cache line, 2 lines per sector
                        {
                            this->has_l2_cache = true;
                            this->has_l3_cache = true;
                            this->L3_dcache.cache_size = KB(512);
                            this->L3_dcache.line_size = 64;
                            this->L3_dcache.set_associativity = 4;
                            this->L3_dcache.has_sectors = true;
                            this->L3_dcache.lines_per_sector = 2;
                            break;
                        }
                        case(0x23)://L3 d-cache 1MB, 8-way set associative, 64byte cache line, 2 lines per sector
                        {
                            this->has_l2_cache = true;
                            this->has_l3_cache = true;
                            this->L3_dcache.cache_size = MB(1);
                            this->L3_dcache.line_size = 64;
                            this->L3_dcache.set_associativity = 8;
                            this->L3_dcache.has_sectors = true;
                            this->L3_dcache.lines_per_sector = 2;
                            break;
                        }
                        case(0x24)://L2 d-cache 1MB, 16-way set associative, 64byte cache line
                        {
                            this->has_l2_cache = true;
                            this->L2_dcache.cache_size = MB(1);
                            this->L2_dcache.line_size = 64;
                            this->L2_dcache.set_associativity = 16;
                            break;
                        }
                        case(0x25)://L3 d-cache 2MB, 8-way set associative, 64byte cache line, 2 lines per sector
                        {
                            this->has_l2_cache = true;
                            this->has_l3_cache = true;
                            this->L3_dcache.cache_size = MB(2);
                            this->L3_dcache.line_size = 64;
                            this->L3_dcache.set_associativity = 8;
                            this->L3_dcache.has_sectors = true;
                            this->L3_dcache.lines_per_sector = 2;
                            break;
                        }
                        case(0x29)://L3 d-cache 4MB, 8-way set associative, 64byte cache line, 2 lines per sector
                        {
                            this->has_l2_cache = true;
                            this->has_l3_cache = true;
                            this->L3_dcache.cache_size = MB(4);
                            this->L3_dcache.line_size = 64;
                            this->L3_dcache.set_associativity = 8;
                            this->L3_dcache.has_sectors = true;
                            this->L3_dcache.lines_per_sector = 2;
                            break;
                        }
                        case(0x2C)://L1 d-cache 32KB, 8-way set associative, 64byte cache line
                        {
                            this->L1_dcache.cache_size = KB(32);
                            this->L1_dcache.line_size = 64;
                            this->L1_dcache.set_associativity = 8;
                            break;
                        }
                        case(0x30)://L1 i-cache 32KB, 8-way set associative, 64byte cache line
                        {
                            this->L1_icache.cache_size = KB(32);
                            this->L1_icache.line_size = 64;
                            this->L1_icache.set_associativity = 8;
                            break;
                        }
                        case(0x40)://No L2 cache, or no L3 cache if the processor has L2
                        {
                            if(this->has_l2_cache)
                            {
                                this->has_l3_cache = false;
                            }
                            else
                            {
                                this->has_l2_cache = false;
                            }
                            break;
                        }
                        case(0x41)://L2 d-cache 128KB, 4-way set associative, 32byte line size
                        {
                            this->has_l2_cache = true;
                            this->L2_dcache.cache_size = KB(128);
                            this->L2_dcache.line_size = 32;
                            this->L2_dcache.set_associativity = 4;
                            break;
                        }
                        case(0x42)://Cache 2nd-level cache: 256 KBytes, 4-way set associative, 32 byte line size
                        {
                            this->has_l2_cache = true;
                            this->L2_dcache.cache_size = KB(256);
                            this->L2_dcache.line_size = 32;
                            this->L2_dcache.set_associativity = 4;
                            break;
                        }
                        case(0x43):// Cache 2nd-level cache: 512 KBytes, 4-way set associative, 32 byte line size
                        {
                            this->has_l2_cache = true;
                            this->L2_dcache.cache_size = KB(512);
                            this->L2_dcache.line_size = 32;
                            this->L2_dcache.set_associativity = 4;
                            break;
                        }
                        case(0x44):// Cache 2nd-level cache: 1 MByte, 4-way set associative, 32 byte line size
                        {
                            this->has_l2_cache = true;
                            this->L2_dcache.cache_size = MB(1);
                            this->L2_dcache.line_size  = 32;
                            this->L2_dcache.set_associativity = 4;
                            break;
                        }
                        case(0x45)://Cache 2n-level cache: 2 MByte, 4-way set associative, 32 byte line size
                        {
                            this->has_l2_cache = true;
                            this->L2_dcache.cache_size = MB(2);
                            this->L2_dcache.line_size = 32;
                            this->L2_dcache.set_associativity= 4;
                            break;
                        }
                        case(0x46)://Cache 3rd-level cache: 4 MByte, 4-way set associative, 64 byte line size
                        {
                            this->has_l3_cache = true;
                            this->has_l2_cache = true;
                            this->L3_dcache.cache_size = MB(4);
                            this->L3_dcache.line_size = 64;
                            this->L3_dcache.set_associativity = 4;
                            break;
                        }
                        case(0x47):// Cache 3rd-level cache: 8 MByte, 8-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x48):// Cache 2nd-level cache: 3MByte, 12-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x49):// Cache 3rd-level cache: 4MB, 16-way set associative, 64-byte line size (Intel Xeon processor MP, Family 0FH, Model 06H); 2nd-level cache: 4 MByte, 16-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x4A):// Cache 3rd-level cache: 6MByte, 12-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x4B):// Cache 3rd-level cache: 8MByte, 16-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x4C):// Cache 3rd-level cache: 12MByte, 12-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x4D):// Cache 3rd-level cache: 16MByte, 16-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x4E):// Cache 2nd-level cache: 6MByte, 24-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x4F):// TLB Instruction TLB: 4 KByte pages, 32 entries
                        {
                            break;
                        }
                        case(0x50):// TLB Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 64 entries
                        {
                            break;
                        }
                        case(0x51):// TLB Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 128 entries
                        {
                            break;
                        }
                        case(0x52):// TLB Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 256 entries
                        {
                            break;
                        }
                        case(0x55):// TLB Instruction TLB: 2-MByte or 4-MByte pages, fully associative, 7 entries
                        {
                            break;
                        }
                        case(0x56):// TLB Data TLB0: 4 MByte pages, 4-way set associative, 16 entries
                        {
                            break;
                        }
                        case(0x57):// TLB Data TLB0: 4 KByte pages, 4-way associative, 16 entries
                        {
                            break;
                        }
                        case(0x59):// TLB Data TLB0: 4 KByte pages, fully associative, 16 entries
                        {
                            break;
                        }
                        case(0x5A):// TLB Data TLB0: 2 MByte or 4 MByte pages, 4-way set associative, 32 entries
                        {
                            break;
                        }
                        case(0x5B):// TLB Data TLB: 4 KByte and 4 MByte pages, 64 entries
                        {
                            break;
                        }
                        case(0x5C):// TLB Data TLB: 4 KByte and 4 MByte pages,128 entries
                        {
                            break;
                        }
                        case(0x5D):// TLB Data TLB: 4 KByte and 4 MByte pages,256 entries
                        {
                            break;
                        }
                        case(0x60):// Cache 1st-level data cache: 16 KByte, 8-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x61):// TLB Instruction TLB: 4 KByte pages, fully associative, 48 entries
                        {
                            break;
                        }
                        case(0x63):// TLB Data TLB: 2 MByte or 4 MByte pages, 4-way set associative, 32 entries and a separate array with 1 GByte pages, 4-way set associative, 4 entries
                        {
                            break;
                        }
                        case(0x64):// TLB Data TLB: 4 KByte pages, 4-way set associative, 512 entries
                        {
                            break;
                        }
                        case(0x66):// Cache 1st-level data cache: 8 KByte, 4-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x67):// Cache 1st-level data cache: 16 KByte, 4-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x68):// Cache 1st-level data cache: 32 KByte, 4-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x6A):// Cache uTLB: 4 KByte pages, 8-way set associative, 64 entries
                        {
                            break;
                        }
                        case(0x6B):// Cache DTLB: 4 KByte pages, 8-way set associative, 256 entries
                        {
                            break;
                        }
                        case(0x6C):// Cache DTLB: 2M/4M pages, 8-way set associative, 128 entries
                        {
                            break;
                        }
                        case(0x6D):// Cache DTLB: 1 GByte pages, fully associative, 16 entries
                        {
                            break;
                        }
                        case(0x70):// Cache Trace cache: 12 K-μop, 8-way set associative
                        {
                            break;
                        }
                        case(0x71):// Cache Trace cache: 16 K-μop, 8-way set associative
                        {
                            break;
                        }
                        case(0x72):// Cache Trace cache: 32 K-μop, 8-way set associative
                        {
                            break;
                        }
                        case(0x76):// TLB Instruction TLB: 2M/4M pages, fully associative, 8 entries
                        {
                            break;
                        }
                        case(0x78):// Cache 2nd-level cache: 1 MByte, 4-way set associative, 64byte line size
                        {
                            break;
                        }
                        case(0x79):// Cache 2nd-level cache: 128 KByte, 8-way set associative, 64 byte line size, 2 lines per sector
                        {
                            break;
                        }
                        case(0x7A):// Cache 2nd-level cache: 256 KByte, 8-way set associative, 64 byte line size, 2 lines per sector
                        {
                            break;
                        }
                        case(0x7B):// Cache 2nd-level cache: 512 KByte, 8-way set associative, 64 byte line size, 2 lines per sector
                        {
                            break;
                        }
                        case(0x7C):// Cache 2nd-level cache: 1 MByte, 8-way set associative, 64 byte line size, 2 lines per sector
                        {
                            break;
                        }
                        case(0x7D):// Cache 2nd-level cache: 2 MByte, 8-way set associative, 64byte line size
                        {
                            break;
                        }
                        case(0x7F):// Cache 2nd-level cache: 512 KByte, 2-way set associative, 64-byte line size
                        {
                            break;
                        }
                        case(0x80):// Cache 2nd-level cache: 512 KByte, 8-way set associative, 64-byte line size
                        {
                            break;
                        }
                        case(0x82):// Cache 2nd-level cache: 256 KByte, 8-way set associative, 32 byte line size
                        {
                            break;
                        }
                        case(0x83):// Cache 2nd-level cache: 512 KByte, 8-way set associative, 32 byte line size
                        {
                            break;
                        }
                        case(0x84):// Cache 2nd-level cache: 1 MByte, 8-way set associative, 32 byte line size
                        {
                            break;
                        }
                        case(0x85):// Cache 2nd-level cache: 2 MByte, 8-way set associative, 32 byte line size
                        {
                            break;
                        }
                        case(0x86):// Cache 2nd-level cache: 512 KByte, 4-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0x87):// Cache 2nd-level cache: 1 MByte, 8-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xA0):// DTLB DTLB: 4k pages, fully associative, 32 entries
                        {
                            break;
                        }
                        case(0xB0):// TLB Instruction TLB: 4 KByte pages, 4-way set associative, 128 entries
                        {
                            break;
                        }
                        case(0xB1):// TLB Instruction TLB: 2M pages, 4-way, 8 entries or 4M pages, 4-way, 4 entries
                        {
                            break;
                        }
                        case(0xB2):// TLB Instruction TLB: 4KByte pages, 4-way set associative, 64 entries
                        {
                            break;
                        }
                        case(0xB3):// TLB Data TLB: 4 KByte pages, 4-way set associative, 128 entries
                        {
                            break;
                        }
                        case(0xB4):// TLB Data TLB1: 4 KByte pages, 4-way associative, 256 entries
                        {
                            break;
                        }
                        case(0xB5):// TLB Instruction TLB: 4KByte pages, 8-way set associative, 64 entries
                        {
                            break;
                        }
                        case(0xB6):// TLB Instruction TLB: 4KByte pages, 8-way set associative, 128 entries
                        {
                            break;
                        }
                        case(0xBA):// TLB Data TLB1: 4 KByte pages, 4-way associative, 64 entries
                        {
                            break;
                        }
                        case(0xC0):// TLB Data TLB: 4 KByte and 4 MByte pages, 4-way associative, 8 entries
                        {
                            break;
                        }
                        case(0xC1):// STLB Shared 2nd-Level TLB: 4 KByte/2MByte pages, 8-way associative, 1024 entries
                        {
                            break;
                        }
                        case(0xC2):// DTLB DTLB: 4 KByte/2 MByte pages, 4-way associative, 16 entries
                        {
                            break;
                        }
                        case(0xC3):// STLB Shared 2nd-Level TLB: 4 KByte /2 MByte pages, 6-way associative, 1536 entries. Also 1GBbyte pages, 4-way, 16 entries.
                        {
                            break;
                        }
                        case(0xC4):// DTLB DTLB: 2M/4M Byte pages, 4-way associative, 32 entries
                        {
                            break;
                        }
                        case(0xCA):// STLB Shared 2nd-Level TLB: 4 KByte pages, 4-way associative, 512 entries
                        {
                            break;
                        }
                        case(0xD0):// Cache 3rd-level cache: 512 KByte, 4-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xD1):// Cache 3rd-level cache: 1 MByte, 4-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xD2):// Cache 3rd-level cache: 2 MByte, 4-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xD6):// Cache 3rd-level cache: 1 MByte, 8-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xD7):// Cache 3rd-level cache: 2 MByte, 8-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xD8):// Cache 3rd-level cache: 4 MByte, 8-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xDC):// Cache 3rd-level cache: 1.5 MByte, 12-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xDD):// Cache 3rd-level cache: 3 MByte, 12-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xDE):// Cache 3rd-level cache: 6 MByte, 12-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xE2):// Cache 3rd-level cache: 2 MByte, 16-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xE3):// Cache 3rd-level cache: 4 MByte, 16-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xE4):// Cache 3rd-level cache: 8 MByte, 16-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xEA):// Cache 3rd-level cache: 12MByte, 24-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xEB):// Cache 3rd-level cache: 18MByte, 24-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xEC):// Cache 3rd-level cache: 24MByte, 24-way set associative, 64 byte line size
                        {
                            break;
                        }
                        case(0xF0):// Prefetch 64-Byte prefetching
                        {
                            break;
                        }
                        case(0xF1):// Prefetch 128-Byte prefetching
                        {
                            break;
                        }
                        case(0xFE):// General CPUID leaf 2 does not report TLB descriptor information; use CPUID leaf 18H to query TLB and other address translation parameters.
                        {
                            break;
                        }
                        case(0xFF):// General CPUID leaf 2 does not report cache descriptor information, use CPUID leaf 4 to query cache parameters
                        {
                            break;
                        }
                        default: break;
                    }
                }
            }
            else 
            {
                LWARNING("Register does not contain valid information");
            }
        }
    }

}

