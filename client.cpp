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
    u32 Counter = 0;

    u32 Buffer[3] = {};

    while (true)
    {
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
                    // u32 Recv = *((u32*) (Event.packet->data));
                    // printf("%u\n", Recv);
                    enet_packet_destroy(Event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    Connected = false;
                    printf("Disconnected\n");
                    break;
                };
            }
        }

        if (Connected) 
        {
            Buffer[0] = Buffer[1];
            Buffer[1] = Buffer[2];
            Buffer[2] = Counter;
            ENetPacket *Packet = enet_packet_create(Buffer, sizeof(Buffer), ENET_PACKET_FLAG_UNSEQUENCED);
            enet_peer_send(Peer, 0, Packet);

            Counter++;
        }

        Sleep(16);
    }
}
