#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>
#include <Windows.h>
#include <chrono>
#include <thread>

int mouse_movement_interval = 5;

int hold_direction = -1;
bool hold = false;

void move_mouse(int direction) {
	POINT mouse_position;

	GetCursorPos(&mouse_position);

	//0 = up
	//1 = down
	//2 = left
	//3 = right

	switch (direction) {
		case 0:
			std::cout << "Mouse moving up";
			SetCursorPos(mouse_position.x, mouse_position.y - mouse_movement_interval);
			break;
		case 1:
			std::cout << "Mouse moving down";
			SetCursorPos(mouse_position.x, mouse_position.y + mouse_movement_interval);
			break;
		case 2:
			std::cout << "Mouse moving left";
			SetCursorPos(mouse_position.x - mouse_movement_interval, mouse_position.y);
			break;
		case 3:
			std::cout << "Mouse moving right";
			SetCursorPos(mouse_position.x + mouse_movement_interval, mouse_position.y);
			break;
	}

	std::cout << " from (X: " << mouse_position.x << ", Y: " << mouse_position.y << ")";
	GetCursorPos(&mouse_position);
	std::cout << " to (X: " << mouse_position.x << ", Y: " << mouse_position.y << ")" << std::endl;
}

void move_mouse_hold(int direction) {
	hold_direction = direction;
	hold = true;
}

void release() {
	hold = false;
	hold_direction = -1;
}

void click_mouse(bool left_or_right, bool down_or_up) {
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.dwExtraInfo = NULL;
	input.mi.time = 0;

	//LEFT = TRUE
	//RIGHT = FALSE
	//DOWN = TRUE
	//UP = FALSE

	if (left_or_right) {
		if (down_or_up)
			input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		else 
			input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	}
	else {
		if (down_or_up)
			input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		else
			input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	}

	SendInput(1, &input, sizeof(INPUT));
}

void handle_button_press(SDL_Event event) {
	switch (event.jbutton.button) {
		case 0:
			move_mouse_hold(0);
			break;
		case 1:
			move_mouse_hold(3);
			break;
		case 2:
			move_mouse_hold(1);
			break;
		case 3:
			move_mouse_hold(2);
			break;
		case 4:
			std::cout << "Left click down" << std::endl;
			click_mouse(true, true);
			break;
		case 5:
			std::cout << "Right click down" << std::endl;
			click_mouse(false, true);
			break;
		case 9:
			if (mouse_movement_interval < 20) {
				mouse_movement_interval += 1;
				std::cout << "Increased mouse movement interval to " << mouse_movement_interval << std::endl;
			}
			else std::cout << "Mouse movement interval is already at maximum of 20!" << std::endl;
			break;
		case 8:
			if (mouse_movement_interval > 1) {
				mouse_movement_interval -= 1;
				std::cout << "Decreased mouse movement interval to " << mouse_movement_interval << std::endl;
			}
			else std::cout << "Mouse movement interval is already at minimum of 1!" << std::endl;
			break;
	}
}

void handle_button_release(SDL_Event event) {
	switch (event.jbutton.button) {
	case 0:
		release();
		break;
	case 1:
		release();
		break;
	case 2:
		release();
		break;
	case 3:
		release();
		break;
	case 4:
		std::cout << "Left click up" << std::endl;
		click_mouse(true, false);
		break;
	case 5:
		std::cout << "Right click up" << std::endl;
		click_mouse(false, false);
		break;
	}
}

void handle_mouse_movement(SDL_Event event) {
	if (event.jaxis.axis == 1) {
		switch (event.jaxis.value) {
			case -32768:
				move_mouse(0);
				break;
			case 32767:
				move_mouse(1);
				break;
		}
	}
	else {
		switch (event.jaxis.value) {
		case -32768:
			move_mouse(2);
			break;
		case 32767:
			move_mouse(3);
			break;
		}
	}
}

int main()
{
	system("title SNESMouse USB SNES Controller to Windows Mouse controls Emulator VM Simulator 2022 Edition");
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
		exit(2);
	}
	SDL_Event event;

	if (SDL_NumJoysticks() == 0) {
		std::cout << "No joysticks found!" << std::endl;
		exit(0);
	}

	for (int i = 0; i < SDL_NumJoysticks(); i++)
		printf("\t%d %s\n", i, SDL_JoystickName(SDL_JoystickOpen(i)));

	while (true) {
		if (hold) {
			move_mouse(hold_direction);
			std::this_thread::sleep_for(std::chrono::milliseconds(15));
		}

		while (SDL_PollEvent(&event))
		{
			switch (event.type) {
				case SDL_JOYBUTTONDOWN:
					handle_button_press(event);
					break;
				case SDL_JOYBUTTONUP:
					handle_button_release(event);
					break;
				case SDL_JOYAXISMOTION:
					handle_mouse_movement(event);
					break;
			}
		}
	}
}
