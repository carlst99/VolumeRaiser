#pragma once

const char* FRIENDLY_AUDIO_SESSION_STATES[] = { "Inactive", "Active", "Expired" };

int main();
bool GetDefaultDevice(IMMDevice** device);
bool PrintDeviceName(IMMDevice* device);
bool PrintEndpointVolume(IMMDevice* device);
bool GetAudioSessionEnumerator(IMMDevice* device, IAudioSessionEnumerator** sessionEnumerator);
bool ManipulateSessions(IAudioSessionEnumerator* enumerator);

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}