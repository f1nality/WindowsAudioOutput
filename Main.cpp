#
#include "WindowsAudioOutput.h"

void PrintAudioPlaybackDevice(LPWSTR id, LPWSTR name)
{
	printf("Audio Device: %ws\n", name);
}

int main(int argc, char* argv[])
{
	WindowsAudioOutput *windowsAudioOutput = new WindowsAudioOutput();

	windowsAudioOutput->EnumerateAudioPlaybackDevices(&PrintAudioPlaybackDevice);
	windowsAudioOutput->SetDefaultAudioPlaybackDeviceByIndex(0);

	std::vector<WindowsAudioPlaybackDevice> devices = windowsAudioOutput->GetAudioPlaybackDevices();

	printf("First device: %ls\n", devices.front().name.c_str());

	return 0;
}