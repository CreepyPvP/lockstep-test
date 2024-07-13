@echo off
gcc client.cpp -lws2_32 -lwinmm -o client.exe
gcc server.cpp -lws2_32 -lwinmm -o server.exe
