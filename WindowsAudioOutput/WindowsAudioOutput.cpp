#include "WindowsAudioOutput.h"

WindowsAudioOutput::WindowsAudioOutput(void)
{
}

WindowsAudioOutput::~WindowsAudioOutput(void)
{
}

bool WindowsAudioOutput::SetDefaultAudioPlaybackDeviceById(std::wstring device_id)
{	
	IPolicyConfigVista *pPolicyConfig;
	ERole reserved = eConsole;

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);

	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetDefaultEndpoint(device_id.c_str(), reserved);
		pPolicyConfig->Release();
	}

	return SUCCEEDED(hr);
}

bool WindowsAudioOutput::SetDefaultAudioPlaybackDeviceByIndex(UINT device_index)
{
	HRESULT hr = CoInitialize(NULL);
	bool result = false;

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
					if (device_index >= 0 && device_index < count)
					{
						IMMDevice *pDevice;
						hr = pDevices->Item(device_index, &pDevice);

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
										SetDefaultAudioPlaybackDeviceById(wstrID);
										PropVariantClear(&friendlyName);

										result = true;
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

	return result;
}

void WindowsAudioOutput::EnumerateAudioPlaybackDevices(ProcessAudioPlaybackDeviceCallback enumerate_callback)
{
	HRESULT hr = CoInitialize(NULL);

	if (SUCCEEDED(hr))
	{
		IMMDeviceEnumerator *pEnum = NULL;
		
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnum);
		
		if (SUCCEEDED(hr))
		{
			LPWSTR defaultDeviceId = NULL;
			IMMDevice *pDefaultDevice;

			HRESULT hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice);

			if (SUCCEEDED(hr))
			{
				hr = pDefaultDevice->GetId(&defaultDeviceId);

				if (!SUCCEEDED(hr))
				{
					defaultDeviceId = NULL;
				}

				pDefaultDevice->Release();
			}

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
						HRESULT hr = pDevices->Item(i, &pDevice);

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
										BOOL isDefault = wcscmp(wstrID, defaultDeviceId) == 0;

										enumerate_callback(wstrID, friendlyName.pwszVal, isDefault);
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

std::vector<WindowsAudioPlaybackDevice> WindowsAudioOutput::GetAudioPlaybackDevices()
{
	HRESULT hr = CoInitialize(NULL);
	std::vector<WindowsAudioPlaybackDevice> devices;

	if (SUCCEEDED(hr))
	{
		IMMDeviceEnumerator *pEnum = NULL;
		
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnum);
		
		if (SUCCEEDED(hr))
		{
			LPWSTR defaultDeviceId = NULL;
			IMMDevice *pDefaultDevice;

			HRESULT hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice);

			if (SUCCEEDED(hr))
			{
				hr = pDefaultDevice->GetId(&defaultDeviceId);

				if (!SUCCEEDED(hr))
				{
					defaultDeviceId = NULL;
				}

				pDefaultDevice->Release();
			}

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
						HRESULT hr = pDevices->Item(i, &pDevice);

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
										WindowsAudioPlaybackDevice device;

										device.id = wstrID;
										device.name = friendlyName.pwszVal;
										device.default = wcscmp(wstrID, defaultDeviceId) == 0;
										
										devices.push_back(device);

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

	return devices;
}