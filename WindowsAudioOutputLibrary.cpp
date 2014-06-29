#include <stdio.h>
#include <wchar.h>
#include <tchar.h>
#include "windows.h"
#include "Mmdeviceapi.h"
#include "PolicyConfig.h"
#include "Propidl.h"
#include "Functiondiscoverykeys_devpkey.h"

typedef void (*EnumerateAudioPlaybackDevicesCallback)(LPWSTR, PROPVARIANT);  

HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID)
{	
	IPolicyConfigVista *pPolicyConfig;
	ERole reserved = eConsole;

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);

	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
		pPolicyConfig->Release();
	}

	return hr;
}

void EnumerateAudioPlaybackDevices(EnumerateAudioPlaybackDevicesCallback enumerate_callback)
{
	HRESULT hr = CoInitialize(NULL);

	if (SUCCEEDED(hr))
	{
		IMMDeviceEnumerator *pEnum = NULL;
		
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnum);
		
		if (SUCCEEDED(hr))
		{
			IMMDeviceCollection *pDevices;
			
			hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);

			if (SUCCEEDED(hr))
			{
				UINT count;

				pDevices->GetCount(&count);

				if (SUCCEEDED(hr))
				{
					for (UINT i = 0; i < count; ++i)
					{
						IMMDevice *pDevice;
						hr = pDevices->Item(i, &pDevice);

						if (SUCCEEDED(hr))
						{
							LPWSTR wstrID = NULL;

							hr = pDevice->GetId(&wstrID);

							if (SUCCEEDED(hr))
							{
								IPropertyStore *pStore;
								hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);

								if (SUCCEEDED(hr))
								{
									PROPVARIANT friendlyName;

									PropVariantInit(&friendlyName);
									hr = pStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);

									if (SUCCEEDED(hr))
									{
										enumerate_callback(wstrID, friendlyName);
										PropVariantClear(&friendlyName);
									}

									pStore->Release();
								}
							}

							pDevice->Release();
						}
					}
				}

				pDevices->Release();
			}

			pEnum->Release();
		}
	}
}

int i = 0;

void SetAudioPlaybackDeviceDefault(LPWSTR wstrID, PROPVARIANT friendlyName)
{
	if (i++ == 3) SetDefaultAudioPlaybackDevice(wstrID);
}

void PrintAudioPlaybackDevice(LPWSTR wstrID, PROPVARIANT friendlyName)
{
	printf("Audio Device: %ws\n", friendlyName.pwszVal);
}

int main(int argc, char* argv[])
{
	EnumerateAudioPlaybackDevices(&PrintAudioPlaybackDevice);
	EnumerateAudioPlaybackDevices(&SetAudioPlaybackDeviceDefault);

	return 0;
}