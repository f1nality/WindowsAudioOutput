#include "WindowsAudioOutput.h"
#include <iostream>
#include <cstring>

void PrintAudioPlaybackDevice(LPWSTR id, LPWSTR name, BOOL default)
{
	printf("%d %ws\n", default, name);
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	char *action;

	if (argc < 2)
	{
		action = "list";
	}
	else
	{
		action = argv[1];
	}

	WindowsAudioOutput *windowsAudioOutput = new WindowsAudioOutput();

	if (strcmp(action, "list") == 0)
	{
		windowsAudioOutput->EnumerateAudioPlaybackDevices(&PrintAudioPlaybackDevice);
	}
	else if (strcmp(action, "setdefault") == 0)
	{
		if (argc < 3)
		{
			std::cout << "ERROR:" << "No device index specified";

			return 0;
		}

		int index = atoi(argv[2]);
		
		windowsAudioOutput->SetDefaultAudioPlaybackDeviceByIndex(index);
	}
	else
	{
		std::cout << "ERROR:" << "Unknown command" << action;
	}

	return 0;
}