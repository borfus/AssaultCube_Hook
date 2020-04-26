#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dll_generic_memory_manipulation.h"

DWORD WINAPI run(LPVOID hModule) {
    // create console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    int new_base = (int)GetModuleHandle(NULL); // running with game codebase
    char new_base_string[16];
    _itoa(new_base, new_base_string, 16);
    printf("DLL Attached! New Base: %i", new_base_string);

    DWORD *player_pointer = (DWORD*)(new_base + 0x10F4F4);
    bool health_hack = false, ammo_hack = false, recoil_hack = false, rapid_fire_hack = false;

    while (true) {
        if (GetAsyncKeyState(VK_END) & 1) {
            break;
        }

        if (GetAsyncKeyState(VK_F1) & 1) {
            health_hack = !health_hack;
        }

        if (GetAsyncKeyState(VK_F2) & 1) {
            ammo_hack = !ammo_hack;
        }

        if (GetAsyncKeyState(VK_F3) & 1) {
            recoil_hack = !recoil_hack;

            if (recoil_hack) {
                dll_nop_memory(((DWORD)new_base + 0x63786), 10);
            } else {
                BYTE original_bytes[10] = { 0x50, 0x8D, 0x4C, 0x24, 0x1C, 0x51, 0x8B, 0xCE, 0xFF, 0xD2 };
                dll_write_memory_bytes((LPVOID)(new_base + 0x63786), original_bytes, 10);
            }
        }

        if (GetAsyncKeyState(VK_F4) & 1) {
            rapid_fire_hack = !rapid_fire_hack;

            if (rapid_fire_hack) {
                dll_nop_memory(((DWORD)new_base + 0x637E4), 2);
            } else {
                BYTE original_bytes[2] = { 0x89, 0x0A };
                dll_write_memory_bytes((LPVOID)(new_base + 0x637E4), original_bytes, 2);
            }
        }

        if (player_pointer) {
            if (health_hack) {
                LPVOID health_address = (LPVOID)(*player_pointer + 0xF8);
                dll_write_memory(DWORD)(health_address, 999);
            }

            if (ammo_hack) {
                DWORD ammo_pointer_list[3] = { (DWORD)player_pointer, 0x374, 0x14 };
                DWORD *ammo = dll_jump_through_pointers(ammo_pointer_list, sizeof(ammo_pointer_list)/sizeof(DWORD));
                *ammo = 999;
            }
        }

        Sleep(5);
    }

    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread((HMODULE)hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  reason,
    LPVOID lpReserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(NULL, 0, &run, hModule, 0, NULL));
        break;
    case DLL_PROCESS_DETACH:
        printf("DLL detached!\n");
        break;
    }
    return TRUE;
}

