#include "Input.h"
#include <SDL_syswm.h>
#include "Error.h"
#include <iostream>
#include <Winuser.h>
#include "Game.h"

#ifdef _WIN64	// for some reason qword isn't defined for x64 targets
	#define QWORD uint64_t
#endif

namespace RustyEngine
{
	Vec2 Input::mouse_pos;		// Mouse position
	Vec2 Input::touch_pos;		// Touch position (on touchscreen devices)
	float Input::mouse_left;	// Is left mouse button clicked (how much? acceleration can be used for non-analog inputs)
	float Input::mouse_right;	// Is right mouse button clicked (how much?)
	float Input::mouse_middle;	// Is middle mouse button clicked (how much?)
	float Input::touch;			// Has anyone touched screen? (how much? acceleration can be used for non-analog inputs)

	RAWINPUTDEVICE Input::Rid[1];
	long Input::xes = 0;
	long Input::yes = 0;
	bool Input::rawInputInitialized = false;

	// Return how much mouse button is clicked (for single button only)
	float Input::getMouseDown(Mousebutton button)
	{
		switch (button)
		{
			case left: return mouse_left;
			case middle: return mouse_middle;
			case right: return mouse_right;
		}
		return 0.0f;
	}


	// Get absolute mouse position (read-only)
	const Vec2 & Input::getMousePos()
	{
		return mouse_pos;
	}


	// Retrun if anyone has touched the screen (and how much)
	float Input::getTouch()
	{
		return touch;
	}


	// Get absolute touch position (read-only)
	const Vec2 & Input::getTouchPos()
	{
		return touch_pos;
	}


	// TODO convert update to private method and access it via friend class
	// Handles input events (convert to only handle input events, instead of all events)
	void Input::update_old()
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))	// Grab input events
		{
			switch (event.type)
			{
				// Handle mouse button press
				case SDL_MOUSEBUTTONDOWN:
					mouse_pos.set(event.motion.x, event.motion.y);
					
					if (event.button.button & SDL_BUTTON_LMASK)
						mouse_left = 1.0f;
					if (event.button.button & SDL_BUTTON_MMASK)
						mouse_middle = 1.0f;
					if (event.button.button & SDL_BUTTON_RMASK)
						mouse_right = 1.0f;
					break;

				// Handle mouse button release
				case SDL_MOUSEBUTTONUP:
					mouse_pos.set(event.motion.x, event.motion.y);

					if (event.button.button & SDL_BUTTON_LMASK)
						mouse_left = 0.0f;
					if (event.button.button & SDL_BUTTON_MMASK)
						mouse_middle = 0.0f;
					if (event.button.button & SDL_BUTTON_RMASK)
						mouse_right = 0.0f;
					break;

				// Handle mouse movement
				case SDL_MOUSEMOTION:
					mouse_pos.set(event.motion.x, event.motion.y);
					break;

				// Handle finger press
				case SDL_FINGERDOWN:
					touch_pos.set(event.tfinger.x, event.tfinger.y);
					touch = 1.0f;
					break;

				// Handle finger release
				case SDL_FINGERUP:
					touch_pos.set(event.tfinger.x, event.tfinger.y);
					touch = 0.0f;
					break;

				// Handle finger movement
				case SDL_FINGERMOTION:
					touch_pos.set(event.tfinger.x, event.tfinger.y);
					break;


				// Special system events
				case SDL_QUIT:
					exit(0);
			}
		}
	}
	
	void Input::rawInputHook(void* userdata, void* hwnd, UINT msg, uint64_t wparam, Sint64 lparam)
	{
		if (msg != WM_INPUT)
			return;

		u_int dwSize;


		GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &dwSize,
			sizeof(RAWINPUTHEADER));

		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL)
			return;

		if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize,	sizeof(RAWINPUTHEADER)) != dwSize)
			OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			std::cout << raw->data.mouse.lLastX << " " << raw->data.mouse.lLastY << "\n";
		}

		DefWindowProc((HWND)hwnd, msg, wparam, lparam);
	}


	void Input::initRawInput()
	{
		
		Rid[0].usUsagePage = 0x01;
		Rid[0].usUsage = 0x02;
		Rid[0].dwFlags = RIDEV_NOLEGACY; // add HID mouse and ignore legacy mouse
		Rid[0].hwndTarget = 0;
														   
		SDL_SysWMinfo info;
		SDL_SysWMmsg msg;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(Game::world.main_window, &info);
		SDL_SetWindowsMessageHook(Input::rawInputHook, nullptr);

		Rid[0].hwndTarget = info.info.win.window;


		if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE)
		{
			std::cout << "INPUT | Registration of raw input device failed: " << GetLastError() << std::endl;
			exit(RAWINPUT_REGISTER_FAILED);
		}

		rawInputInitialized = true;
		/*
		uint32_t nDevices;
		PRAWINPUTDEVICELIST pRawInputDeviceList;
		GetRawInputDeviceList(nullptr, &nDevices, sizeof(RAWINPUTDEVICELIST));
		pRawInputDeviceList = (PRAWINPUTDEVICELIST)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices);
		GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST));
		std::cout << "Handle: " << pRawInputDeviceList->hDevice << " Type: " << pRawInputDeviceList->dwType << std::endl;

		free(pRawInputDeviceList);
		system("pause");
		exit(1);*/
	}

	// This is buffered update, could cause massive lag, more tests required
	// Mostly I still have to figure out how the hell does all this work
	/*
	void Input::update()
	{
		update_old();
	}*/


	void updateRawInput()
	{
		long xes, yes;
		u_int buffer_size1, buffer_size2;
		u_int count = u_int(-1);

		// Fetch minimum raw input buffer size
		if (GetRawInputBuffer(nullptr, &buffer_size1, sizeof(RAWINPUTHEADER)) == u_int(-1))
		{
			std::cout << "INPUT | Failed to fetch raw input buffer size: " << GetLastError() << std::endl;
			exit(RAWINPUT_GETINPUTBUFFER_FAILED);
		}
		//buffer_size2 = buffer_size1;
		buffer_size1 *= 8; // Alignment for wow64 systems

						   // Allocate memory for the said buffer
		PRAWINPUT raw_input = (PRAWINPUT)malloc(buffer_size1);

		if (raw_input == nullptr)
		{
			std::cout << "INPUT | Cannot allocate memory for raw input buffer (out of memory): " << GetLastError() << std::endl;
			exit(OUT_OF_MEMORY);
		}

		while (true)
		{
			buffer_size2 = buffer_size1;
			// Fetch raw input buffer
			count = GetRawInputBuffer(raw_input, &buffer_size2, sizeof(RAWINPUTHEADER));

			if (count == u_int(-1))
			{
				std::cout << "INPUT | Failed to fetch raw input buffer: " << GetLastError() << std::endl;
				exit(RAWINPUT_GETINPUTBUFFER_FAILED);
			}
			else if (count == 0)
				break;

			PRAWINPUT raw_start = raw_input;

			PRAWINPUT* raw_input2 = (PRAWINPUT*)malloc(sizeof(PRAWINPUT) * count);
			if (raw_input2 == nullptr)
				break;

			// Fill raw input buffer with data
			for (u_int i = 0; i < count; i++)
			{
				std::cout << "Raw input type: " << raw_start->header.dwType << std::endl;

				//xes = raw_start->data.mouse.lLastX;
				//yes = raw_start->data.mouse.lLastY;
				raw_input2[i] = raw_start;

				xes = raw_input2[i]->data.mouse.lLastX;
				yes = raw_input2[i]->data.mouse.lLastY;

				raw_start = NEXTRAWINPUTBLOCK(raw_start);
				std::cout << "All x: " << xes << "     All y: " << yes << std::endl;
				//std::cout << raw_input2[i]->data.mouse.ulButtons << " " << raw_input2[i]->data.mouse.ulRawButtons << " " << raw_input2[i]->data.mouse.usButtonData << " " << raw_input2[i]->data.mouse.usButtonFlags << " " << raw_input2[i]->data.mouse.usFlags << " " << raw_input2[i]->data.mouse.ulExtraInformation << " " << xes << " " << yes << std::endl;
			}

			// Clean raw input buffer (free memory)
			DefRawInputProc(raw_input2, count, sizeof(RAWINPUTHEADER));
			free(raw_input2);
		}
		free(raw_input);
		//system("pause");
		//exit(0);
	}


	void Input::update()
	{
		if (rawInputInitialized)
			SDL_PumpEvents();
		else
			update_old();
			//SDL_Event event;
		//while (SDL_PollEvent(&event)) 
		//{
			/* handle your event here */
		//}

	}	
	

	// Cannot construct static only class (well... you can, but is pointless)
	Input::Input()
	{
	}

	Input::~Input()
	{
	}
}