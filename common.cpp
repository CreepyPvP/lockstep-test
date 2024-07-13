#include <stdint.h>

typedef int32_t i32;
typedef uint32_t u32;
typedef double f64;

f64 TickSpeed = 100.0f;
f64 PerfCountFrequency;

void SetupTimer()
{
    LARGE_INTEGER PerfCountFreqRes;
    QueryPerformanceFrequency(&PerfCountFreqRes);
    PerfCountFrequency = (f64) PerfCountFreqRes.QuadPart;
}

f64 GetTime()
{
    LARGE_INTEGER TimestampNow;
    QueryPerformanceCounter(&TimestampNow);
    return (1000.0f * TimestampNow.QuadPart) / PerfCountFrequency;
}
