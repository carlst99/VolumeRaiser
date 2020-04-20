#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <strsafe.h>
#include <psapi.h>
#include "VolumeRaiserPlus.h"

int main()
{
    HRESULT hr;
    IMMDevice* device = NULL;
    IAudioSessionEnumerator* sessionEnumerator = NULL;

    // Initialise COM
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        printf("Unable to initialize COM: %x\n", hr);
        goto Exit;
    }

    // Get the default render device
    if (!GetDefaultDevice(&device))
        goto Exit;

    // Print the default device's friendly name
    if (!PrintDeviceName(device))
        goto Exit;

    // Print the master volume scalar of the default device
    if (!PrintEndpointVolume(device))
        goto Exit;

    // Get an AudioSessionEnumerator
    if (!GetAudioSessionEnumerator(device, &sessionEnumerator))
        goto Exit;

    printf("\Manipulating audio sessions...\n");
    ManipulateSessions(sessionEnumerator);
    printf("\nVolumes Reset!");

Exit:
    SafeRelease(&device);
    SafeRelease(&sessionEnumerator);
    CoUninitialize();
    return 0;
}

// 
// Gets the default render device for the console role
// 
bool GetDefaultDevice(IMMDevice** device)
{
    HRESULT hr;
    bool success = false;
    IMMDeviceEnumerator* deviceEnumerator = NULL;

    // Get a device enumerator
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
        printf("Unable to instantiate device enumerator: %x\n", hr);
        goto Exit;
    }

    // Get the default audio device
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, device);
    if (FAILED(hr))
    {
        printf("Unable to retrive default audio device: %x\n", hr);
        goto Exit;
    }

    success = true;
Exit:
    SafeRelease(&deviceEnumerator);
    return success;
}

// 
// Prints the friendly name of a device
// 
bool PrintDeviceName(IMMDevice* device)
{
    PWSTR deviceId;
    HRESULT hr;
    bool success = false;

    hr = device->GetId(&deviceId);
    if (FAILED(hr))
    {
        printf("Unable to get device id: %x\n", hr);
        goto Exit;
    }

    IPropertyStore* propertyStore;
    hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
    if (FAILED(hr))
    {
        printf("Unable to open device property store: %x\n", hr);
        goto Exit;
    }

    PROPVARIANT friendlyName;
    PropVariantInit(&friendlyName);
    hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
    SafeRelease(&propertyStore);

    if (FAILED(hr))
    {
        printf("Unable to retrieve friendly name for device : %x\n", hr);
        goto Exit;
    }

    wchar_t deviceName[128];
    hr = StringCbPrintf(deviceName, sizeof(deviceName), L"%s", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal);
    if (FAILED(hr))
    {
        printf("Unable to format friendly name for device : %x\n", hr);
        goto Exit;
    }
    printf("Default Render/Console Device: %S\n", deviceName);

    success = true;
Exit:
    PropVariantClear(&friendlyName);
    CoTaskMemFree(deviceId);
    return success;
}

// 
// Prints the master volume scalar of the specified device
// 
bool PrintEndpointVolume(IMMDevice* device)
{
    HRESULT hr;
    IAudioEndpointVolume* endpointVolume = NULL;
    bool success = false;

    // Get the endpoint volume controls
    hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&endpointVolume);
    if (FAILED(hr))
    {
        printf("Unable to get endpoint volume: %x\n", hr);
        goto Exit;
    }

    // Print the endpoint volume
    float masterVolumeScalar;
    endpointVolume->GetMasterVolumeLevelScalar(&masterVolumeScalar);
    printf("Master Volume Scalar: %f\n", masterVolumeScalar);

    success = true;
Exit:
    SafeRelease(&endpointVolume);
    return success;
}

// 
// Gets an AudioSessionEnumerator for the specified device
// 
bool GetAudioSessionEnumerator(IMMDevice* device, IAudioSessionEnumerator** sessionEnumerator)
{
    HRESULT hr;
    IAudioSessionManager2* sessionManager2 = NULL;
    bool success = false;

    // Get an AudioSessionManager2
    hr = device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, NULL, (void**)&sessionManager2);
    if (FAILED(hr))
    {
        printf("Unable to get session manager 2: %x\n", hr);
        goto Exit;
    }

    // Get a session enumerator
    hr = sessionManager2->GetSessionEnumerator(sessionEnumerator);
    if (FAILED(hr))
    {
        printf("Unable to get session enumerator: %x\n", hr);
        goto Exit;
    }

    success = true;
Exit:
    SafeRelease(&sessionManager2);
    return success;
}

// 
// Prints some properties for all audio sessions in the provided enumerator,
// and resets their relative volume to 1.0
// 
bool ManipulateSessions(IAudioSessionEnumerator* enumerator)
{
    HRESULT hr;
    IAudioSessionControl* sessionControl = NULL;
    ISimpleAudioVolume* sessionSimpleVolume = NULL;
    bool success = false;

    // Get session count
    int sessionCount;
    hr = enumerator->GetCount(&sessionCount);
    if (FAILED(hr))
    {
        printf("Unable to get session count: %x\n", hr);
        goto Exit;
    }

    for (int i = 0; i < sessionCount; i++)
    {
        // Get audio session control
        hr = enumerator->GetSession(i, &sessionControl);
        if (FAILED(hr))
        {
            printf("Unable to get session control: %x\n", hr);
            goto Exit;
        }

        if (!PrintSessionName(sessionControl))
            goto Exit;

        // Get and print session state
        AudioSessionState state;
        hr = sessionControl->GetState(&state);
        if (FAILED(hr))
        {
            printf("Unable to get session state: %x\n", hr);
            goto Exit;
        }
        printf("\t- State: %s\n", FRIENDLY_AUDIO_SESSION_STATES[state]);

        // Get session volume control
        hr = sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&sessionSimpleVolume);
        if (FAILED(hr))
        {
            printf("Unable to get session volume controls: %x\n", hr);
            goto Exit;
        }

        // Get and print session volume
        float volume;
        hr = sessionSimpleVolume->GetMasterVolume(&volume);
        if (FAILED(hr))
        {
            printf("Unable to get session volume: %x\n", hr);
            goto Exit;
        }
        printf("\t- Volume: %f\n", volume);

        // Set session volume to max
        hr = sessionSimpleVolume->SetMasterVolume(1.0f, NULL);
        if (FAILED(hr))
        {
            printf("Unable to set session volume: %x\n", hr);
            goto Exit;
        }

        SafeRelease(&sessionSimpleVolume);
        SafeRelease(&sessionControl);
    }

    success = true;
Exit:
    SafeRelease(&sessionControl);
    SafeRelease(&sessionSimpleVolume);
    return success;
}

bool PrintSessionName(IAudioSessionControl* sessionControl)
{
    HRESULT hr;
    IAudioSessionControl2* sessionControl2 = NULL;
    HANDLE process = NULL;
    bool success = false;

    // Get the session display name
    PWSTR sessionName;
    hr = sessionControl->GetDisplayName(&sessionName);
    if (FAILED(hr))
    {
        printf("Unable to get session display name: %x\n", hr);
        goto Exit;
    }

    // Print the session name
    if (*sessionName != NULL)
    {
        printf("Name: %S\n", sessionName);
    }
    else // Or if null, the process name
    {
        hr = sessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&sessionControl2);
        if (FAILED(hr))
        {
            printf("Unable to get session control 2 interface: %x\n", hr);
            goto Exit;
        }

        DWORD processId;
        hr = sessionControl2->GetProcessId(&processId);
        if (FAILED(hr))
        {
            printf("Unable to get session process ID: %x\n", hr);
            goto Exit;
        }

        process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processId);
        if (process == NULL)
        {
            printf("Unable to get process handle: %u\n", GetLastError());
            goto Exit;
        }

        wchar_t processName[128] = L"Unknown";
        GetModuleBaseName(process, NULL, processName, 128);

        printf("Name: %S\n", processName);
    }

    success = true;
Exit:
    CoTaskMemFree(sessionName);
    SafeRelease(&sessionControl2);
    CloseHandle(process);
    return success;
}
