#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <strsafe.h>
#include "VolumeRaiserPlus.h"

int main()
{
    HRESULT hr;
    IMMDeviceEnumerator* deviceEnumerator = NULL;
    IMMDevice* device = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    IAudioSessionManager2* sessionManager2 = NULL;
    IAudioSessionEnumerator* sessionEnumerator = NULL;

    // Initialise COM
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        printf("Unable to initialize COM: %x\n", hr);
        goto Exit;
    }

    // Get a device enumerator
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
        printf("Unable to instantiate device enumerator: %x\n", hr);
        goto Exit;
    }

    // Get the default audio device
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    if (FAILED(hr))
    {
        printf("Unable to retrive default audio device: %x\n", hr);
        goto Exit;
    }

    // Print the default device name
    LPWSTR deviceName;
    deviceName = GetDeviceName(device);
    if (deviceName == NULL)
    {
        goto Exit;
    }
    printf("Default Render Device: %S\n", deviceName);
    free(deviceName);

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

    // Get an AudioSessionManager2
    hr = device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, NULL, (void**)&sessionManager2);
    if (FAILED(hr))
    {
        printf("Unable to get session manager 2: %x\n", hr);
        goto Exit;
    }

    // Get a session enumerator
    hr = sessionManager2->GetSessionEnumerator(&sessionEnumerator);
    if (FAILED(hr))
    {
        printf("Unable to get session enumerator: %x\n", hr);
        goto Exit;
    }

    printf("\nGetting audio sessions...\n");
    GetSessions(sessionEnumerator);
    printf("\nVolumes Reset!");

Exit:
    SafeRelease(&deviceEnumerator);
    SafeRelease(&device);
    SafeRelease(&endpointVolume);
    SafeRelease(&sessionManager2);
    SafeRelease(&sessionEnumerator);
    CoUninitialize();
    return 0;
}

// 
// Gets the friendly name of a device
// 
LPWSTR GetDeviceName(IMMDevice* device)
{
    LPWSTR deviceId;
    HRESULT hr;

    hr = device->GetId(&deviceId);
    if (FAILED(hr))
    {
        printf("Unable to get device id: %x\n", hr);
        return NULL;
    }

    IPropertyStore* propertyStore;
    hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
    if (FAILED(hr))
    {
        printf("Unable to open device property store: %x\n", hr);
        return NULL;
    }

    PROPVARIANT friendlyName;
    PropVariantInit(&friendlyName);
    hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
    SafeRelease(&propertyStore);

    if (FAILED(hr))
    {
        printf("Unable to retrieve friendly name for device : %x\n", hr);
        return NULL;
    }

    wchar_t deviceName[128];
    hr = StringCbPrintf(deviceName, sizeof(deviceName), L"%s", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal);
    if (FAILED(hr))
    {
        printf("Unable to format friendly name for device : %x\n", hr);
        return NULL;
    }

    PropVariantClear(&friendlyName);
    CoTaskMemFree(deviceId);

    wchar_t* returnValue = _wcsdup(deviceName);
    if (returnValue == NULL)
    {
        printf("Unable to allocate buffer for return\n");
        return NULL;
    }
    return returnValue;
}

void GetSessions(IAudioSessionEnumerator* enumerator)
{
    HRESULT hr;

    // Get session count
    int sessionCount;
    hr = enumerator->GetCount(&sessionCount);
    if (FAILED(hr))
    {
        printf("Unable to get session count: %x\n", hr);
    }

    for (int i = 0; i < sessionCount; i++)
    {
        IAudioSessionControl* sessionControl = NULL;
        ISimpleAudioVolume* sessionSimpleVolume = NULL;

        // Get audio session control
        hr = enumerator->GetSession(i, &sessionControl);
        if (FAILED(hr))
        {
            printf("Unable to get session control: %x\n", hr);
        }

        // Get and print session name
        LPWSTR sessionName;
        hr = sessionControl->GetDisplayName(&sessionName);
        if (FAILED(hr))
        {
            printf("Unable to get session display name: %x\n", hr);
        }
        printf("Name: %S\n", sessionName);
        CoTaskMemFree(sessionName);

        // Get and print session state
        AudioSessionState state;
        hr = sessionControl->GetState(&state);
        if (FAILED(hr))
        {
            printf("Unable to get session state: %x\n", hr);
        }
        printf("\t- State: %s\n", FRIENDLY_AUDIO_SESSION_STATES[state]);

        // Get session volume control
        hr = sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&sessionSimpleVolume);
        if (FAILED(hr))
        {
            printf("Unable to get session volume controls: %x\n", hr);
        }

        // Get and print session volume
        float volume;
        hr = sessionSimpleVolume->GetMasterVolume(&volume);
        if (FAILED(hr))
        {
            printf("Unable to get session volume: %x\n", hr);
        }
        printf("\t- Volume: %f\n", volume);

        // Set session volume to max
        hr = sessionSimpleVolume->SetMasterVolume(1.0f, NULL);
        if (FAILED(hr))
        {
            printf("Unable to set session volume: %x\n", hr);
        }

        SafeRelease(&sessionSimpleVolume);
        SafeRelease(&sessionControl);
    }
}
