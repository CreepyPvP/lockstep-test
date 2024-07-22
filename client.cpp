#include <stdio.h>
#include <windows.h>
#include <math.h>

#define ENET_IMPLEMENTATION
#include "enet.h"

#include "common.cpp"

void LockIn(f32 Position)
{
    static f32 PrevPosition = 0;
    static f32 TotalDX = 0;
    static f32 TotalDeviation = 0;
    static u32 Counter = 0;

    f32 DX = Position - PrevPosition;
    TotalDX += DX;
    ++Counter;

    f32 AvgDX = TotalDX / (f32) Counter;

    f32 Deviation = (DX - AvgDX) * (DX - AvgDX);
    TotalDeviation += Deviation;

    f32 StandardDeviation = sqrt(TotalDeviation / (f32) Counter);

    printf("%.3f %.3f %.3f\n", DX, AvgDX, StandardDeviation);

    PrevPosition = Position;
}


f32 Time = 0;
f32 MPosition[2];
f32 MTime[2];


f32 Lerp()
{
    f32 F = MTime[0] - MTime[1];
    if (!F)
    {
        Time = MTime[0];
        return 1;
    }

    f32 T = (Time - MTime[1]) / F;

    if (T < 0) 
    {
        Time = MTime[1];
        T = 0;
    }

    if (T > 1)
    {
        Time = MTime[0];
        T = 1;
    }

    return T;
}

i32 main()
{
    SetupTimer();

    if (enet_initialize()) 
    {
        printf("An error occurred while initializing ENet.\n");
        return 1;
    }

    ENetHost *Client = enet_host_create(NULL, 1, 1, 0, 0);
    if (!Client) 
    {
        printf("An error occurred while trying to create an ENet client host.\n");
        return 1;
    }

    ENetAddress Addr = {};
    enet_address_set_host(&Addr, "127.0.0.1");
    Addr.port = 4975;

    ENetPeer *Peer = enet_host_connect(Client, &Addr, 1, 0);

    if (!Peer) 
    {
        printf("Failed to connect to peer. Or whatever that means??\n");
        return 1;
    }

    bool Connected = false;

    u32 IncomingSequence = 0;
    u32 OutgoingSequence = 0;

    f32 Position = 0;

    u8 Memory[256];

    f32 FrameTime = 1.0f / 60.0f;

    while (true)
    {
        Time += FrameTime;

        if (Connected) 
        {
            ++OutgoingSequence;
            ENetPacket *Packet = enet_packet_create(&OutgoingSequence, sizeof(OutgoingSequence), ENET_PACKET_FLAG_UNSEQUENCED);
            enet_peer_send(Peer, 0, Packet);
        }

        ENetEvent Event;
        while (enet_host_service(Client, &Event, 0))
        {
            switch (Event.type) 
            {
                case ENET_EVENT_TYPE_CONNECT: {
                    Connected = true;
                    printf("Connected\n");
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    buffer Buffer = FromMemory(Event.packet->data);
                    u32 Sequence = ReadU32(&Buffer);
                    f32 ServerTime = ReadF32(&Buffer);
                    f32 IncomingPosition = ReadF32(&Buffer);

                    if (Sequence > IncomingSequence) 
                    {
                        MPosition[1] = MPosition[0];
                        MPosition[0] = IncomingPosition;
                        MTime[1] = MTime[0];
                        MTime[0] = ServerTime;
                        IncomingSequence = Sequence;
                    }

                    if (IncomingSequence > 2500) 
                    {
                        exit(0);
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

        if (Connected) 
        {
            f32 T = Lerp();
            f32 Delta = MPosition[0] - MPosition[1];
            // printf("[%.3f - %.3f] %.3f: %.3f, %.3f: %.3f\n", Time, T, MTime[0], MPosition[0], MTime[1], MPosition[1]);
            Position = MPosition[1] + Delta * T;

            LockIn(Position);
        }

        Sleep(16);
    }
}
