#import "libid:F1AA5209-5217-4B82-BA7E-A68198999AFA"
#include "serial/serial.h"
#include <iostream>
#include "json/json.h"

//#define SHOW_FPS
#define SHOW_INFO

#ifdef SHOW_FPS
#include <chrono>
#include <ctime>
#endif

int main(int argc, char** argv)
{
	//////////
	// User Configurable Section or through command line
	//////////
	std::string WLEDCOMPORT = "COM3";
	int WLEDBAUDRATE = 115200;
	//
	//// End of User Configurable Section
	///////////

	if (argc > 1) {
		WLEDCOMPORT = std::string(argv[1]);
		if (argc > 2) {
			WLEDBAUDRATE = std::stoi(argv[2]);
		}
	}

	serial::Serial wled_serial(WLEDCOMPORT, WLEDBAUDRATE);
	if (wled_serial.isOpen()) {
#ifdef SHOW_INFO
		std::cout << "Connected to WLED" << std::endl;
#endif
	}
	else {
		return 1;
	}

	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	std::string json_string;

	JSONCPP_STRING json_err;
	Json::Value json_value;

	HRESULT hr;
	// Initialize COM
	hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (SUCCEEDED(hr))
	{
		// Create SDK instance
		AuraServiceLib::IAuraSdkPtr sdk = nullptr;
		hr = sdk.CreateInstance(__uuidof(AuraServiceLib::AuraSdk), nullptr, CLSCTX_INPROC_SERVER);
		if (SUCCEEDED(hr))
		{
			// Acquire control
			sdk->SwitchMode();
			// Enumerate all devices
			AuraServiceLib::IAuraSyncDeviceCollectionPtr devices;
			devices = sdk->Enumerate(0); // 0 means ALL

#ifdef SHOW_INFO
			std::cout << "Found " + std::to_string(devices->Count) + " devices in Aura Sync" << std::endl;
			std::cout << std::endl;

			for (int i = 0; i < devices->Count; i++)
			{
				AuraServiceLib::IAuraSyncDevicePtr dev = devices->Item[i];
				AuraServiceLib::IAuraRgbLightCollectionPtr lights = dev->Lights;

				std::cout << dev->Name;
				std::cout << " : " + std::to_string(lights->Count) + " led(s)" << std::endl;
			}

			std::cout << std::endl;
			std::cout << "Starting Sync" << std::endl;
#endif

#ifdef SHOW_FPS
			int apply_count = 0;
			auto t_start = std::chrono::high_resolution_clock::now();
#endif
			while (1) {
				wled_serial.write("l");
				json_string = wled_serial.readline();
				if (json_string.length() < 5 || json_string[0] != '{' || json_string[json_string.length() - 3] != '}') {
					continue;
				}

				if (reader->parse(json_string.c_str(), json_string.c_str() + json_string.length(), &json_value, &json_err)) {
					json_value = json_value["leds"];
					int led_result_size = json_value.size();

					unsigned int led_index = 0;

					for (int i = 0; i < devices->Count; i++)
					{
						AuraServiceLib::IAuraSyncDevicePtr dev = devices->Item[i];
						AuraServiceLib::IAuraRgbLightCollectionPtr lights = dev->Lights;
						for (int j = 0; j < lights->Count; j++)
						{
							if (led_index >= json_value.size()) {
								break;
							}
							std::string color_value = json_value[led_index].asString();
							std::string bgr_value = "0x00" + color_value.substr(4, 2) + color_value.substr(2, 2) + color_value.substr(0, 2);

							AuraServiceLib::IAuraRgbLightPtr light = lights->Item[j];
							light->Color = (unsigned long)strtol(bgr_value.c_str(), NULL, 16);

							led_index++;
						}
						// Apply colors that we have just set
						dev->Apply();
#ifdef SHOW_FPS
						apply_count++;
						auto t_end = std::chrono::high_resolution_clock::now();
						if (std::chrono::duration<double, std::milli>(t_end - t_start).count() > 1000) {
							t_start = std::chrono::high_resolution_clock::now();
							std::cout << std::to_string(apply_count) + " fps" << std::endl;
							apply_count = 0;
						}
#endif
					}
				}
			}

		}
	}// Uninitialize COM
	::CoUninitialize();

	return 0;
}