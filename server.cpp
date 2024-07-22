#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#define ENET_IMPLEMENTATION
#include "enet.h"

#include "common.cpp"

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

    u32 IncomingSequence = 0;
    u32 OutgoingSequence = 0;
    u32 Frame = 2000;

    f32 PlayerPosition = 0;
    bool ReceivedPacket = false;

    u8 Memory[256];

    f32 Time = 0;
    f32 FrameTime = 1.0f / 60.0f;

    while (Frame)
    {
        Time += FrameTime;

        ENetEvent Event;
        while (enet_host_service(Host, &Event, 0))
        {
            switch (Event.type) 
            {
                case ENET_EVENT_TYPE_CONNECT: {
                    printf("Connected\n");
                    break;
                }

                case ENET_EVENT_TYPE_RECEIVE: {
                    Active = true;
                    buffer Buffer = FromMemory(Event.packet->data);
                    u32 Sequence = ReadU32(&Buffer);

                    if (Sequence > IncomingSequence) 
                    {
                        PlayerPosition += Sequence - IncomingSequence;
                        IncomingSequence = Sequence;

                        ReceivedPacket = true;
                    }

                    enet_packet_destroy(Event.packet);
                    break;
                }

                case ENET_EVENT_TYPE_DISCONNECT: {
                    printf("Disconnected\n");
                    break;
                };
            }
        }

        if (Active)
        {
            Frame--;

            if (ReceivedPacket) 
            {
                buffer Buffer = FromMemory(Memory);
                WriteU32(&Buffer, ++OutgoingSequence);
                WriteF32(&Buffer, Time);
                WriteF32(&Buffer, PlayerPosition);

                ENetPacket *Packet = enet_packet_create(Buffer.Memory, Buffer.Offset, ENET_PACKET_FLAG_UNSEQUENCED);
                enet_host_broadcast(Host, 0, Packet);

                ReceivedPacket = false;
            }

        }

        Sleep(1000.0f * FrameTime);

    }
}
