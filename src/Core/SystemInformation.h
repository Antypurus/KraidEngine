#pragma once

#include <Core/types.h>

namespace Kraid
{
    enum class SSESupport
    {
        None    = 0,
        SSE     = 1,
        SSE2    = 2,
        SSE3    = 3,
        SSE4_1  = 4,
        SSE4_2  = 5
    };

    enum class AVXSupport
    {
        None    = 0,
        AVX     = 1,
        AVX2    = 2,
        AVX512  = 3
    };

    #define FULLY_ASSOCIATIVE_TLB 0xFF
    struct TLBInfo
    {
        uint32 page_size;
        uint16 entry_count;
        uint8 set_associativity;
    };
   
    struct CacheInfo
    {
        uint32 cache_size;
        uint8 line_size;
        uint8 set_associativity;
        bool has_sectors = false;
        uint8 lines_per_sector = 0;
    };

    struct CPUFeatures
    {
        SSESupport sse = SSESupport::None;
        AVXSupport avx = AVXSupport::None;
        TLBInfo instruction_TLB = {};
        TLBInfo data_TLB = {};
        CacheInfo L1_icache = {};
        CacheInfo L1_dcache = {};
        CacheInfo L2_dcache = {};
        CacheInfo L3_dcache = {};
        bool has_l2_cache = false;
        bool has_l3_cache = false;

        CPUFeatures();

        void ExtractCPUIDFunction0x2Information();
    };

}
