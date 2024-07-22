#include <stdint.h>

typedef int32_t i32;
typedef uint8_t u8;
typedef uint32_t u32;
typedef float f32;
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

struct buffer
{
    u8 *Memory;
    u32 Offset;
};

buffer FromMemory(u8 *Memory)
{
    buffer Buffer = {};
    Buffer.Memory = Memory;
    return Buffer;
}

void WriteU32(buffer *Buffer, u32 Value)
{
    *((u32 *) (Buffer->Memory + Buffer->Offset)) = Value;
    Buffer->Offset += sizeof(Value);
}

u32 ReadU32(buffer *Buffer)
{
    u32 Value = *((u32 *) (Buffer->Memory + Buffer->Offset));
    Buffer->Offset += sizeof(Value);
    return Value;
}

void WriteF32(buffer *Buffer, f32 Value)
{
    *((f32 *) (Buffer->Memory + Buffer->Offset)) = Value;
    Buffer->Offset += sizeof(Value);
}

f32 ReadF32(buffer *Buffer)
{
    f32 Value = *((f32 *) (Buffer->Memory + Buffer->Offset));
    Buffer->Offset += sizeof(Value);
    return Value;
}
