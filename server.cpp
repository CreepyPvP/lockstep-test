#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#define ENET_IMPLEMENTATION
#include "enet.h"

#include "common.cpp"

typedef int32_t i32;
typedef uint32_t u32;

u32 PlayoutBufferSize = 0;
u32 PlayoutBuffer[32];

u32 LastSequence = 0;
u32 Correct = 0;
u32 Wrong = 0;

void LockInput(u32 Sequence)
{
    if (Sequence == LastSequence + 1 || (Sequence == 0 && LastSequence == 0))
    {
        Correct++;
        printf("%u\n", Sequence);
    }
    else
    {
        printf("%u WRONG!\n", Sequence);
        Wrong++;
    }

    LastSequence = Sequence;
}

void PlayoutInsert(u32 Sequence)
{
    for (u32 I = 0; I < PlayoutBufferSize; I++)
    {
        if (PlayoutBuffer[I] == Sequence)
        {
            return;
        }
    }

    PlayoutBuffer[PlayoutBufferSize++] = Sequence;
}

i32 main()
{
    SetupTimer();

    if (enet_initialize()) 
    {
        printf("An error occurred while initializing ENet.\n");
        return 1;
    }

    ENetAddress Addr = {};
    Addr.host = ENET_HOST_ANY;
    Addr.port = 4975;

    ENetHost *Host = enet_host_create(&Addr, 2, 1, 0, 0);

    if (!Host) 
    {
        printf("Failed to bind to port");
        return 1;
    }

    bool Active = false;

    u32 Frame = 1000;
    while (Frame)
    {
        ENetEvent Event;
        while (enet_host_service(Host, &Event, 0))
        {
            switch (Event.type) 
            {
                case ENET_EVENT_TYPE_CONNECT: {
                    printf("Connected\n");
                    Active = true;
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    u32 *Recv = (u32 *) Event.packet->data;
                    PlayoutInsert(Recv[0]);
                    PlayoutInsert(Recv[1]);
                    PlayoutInsert(Recv[2]);
                    enet_packet_destroy(Event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    printf("Disconnected\n");
                    break;
                };
            }
        }

        if (!Active)
        {
            continue;
        }

        while (PlayoutBufferSize > 2)
        {
            u32 Min = 0;
            for (u32 I = 1; I < PlayoutBufferSize; ++I)
            {
                if (PlayoutBuffer[I] < PlayoutBuffer[Min])
                {
                    Min = I;
                }
            }

            LockInput(PlayoutBuffer[Min]);
            PlayoutBuffer[Min] = PlayoutBuffer[PlayoutBufferSize - 1];
            PlayoutBufferSize--;
        }

        Sleep(16);

        Frame--;
    }

    printf("Probability correct: \t%f\n", ((f64) (Correct)) / ((f64) (Correct + Wrong)));
    printf("Probability wrong: \t%f\n", ((f64) (Wrong)) / ((f64) (Correct + Wrong)));
}
