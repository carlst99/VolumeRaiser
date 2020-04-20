#pragma once

#include <mmdeviceapi.h>

const char* FRIENDLY_AUDIO_SESSION_STATES[] = { "Inactive", "Active", "Expired" };

int main();
LPWSTR GetDeviceName(IMMDevice* device);
void GetSessions(IAudioSessionEnumerator* enumerator);

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}