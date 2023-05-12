/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

// GP2040 Libraries
#include "gamepad.h"
#include "storagemanager.h"

#include "FlashPROM.h"
#include "CRC32.h"

// MUST BE DEFINED for mpgs
uint32_t getMillis() {
	return to_ms_since_boot(get_absolute_time());
}

uint64_t getMicro() {
	return to_us_since_boot(get_absolute_time());
}


static HIDReport hidReport
{
	.square_btn = 0, .cross_btn = 0, .circle_btn = 0, .triangle_btn = 0,
	.l1_btn = 0, .r1_btn = 0, .l2_btn = 0, .r2_btn = 0,
	.select_btn = 0, .start_btn = 0, .l3_btn = 0, .r3_btn = 0, .ps_btn = 0, .tp_btn = 0,
	.direction = 0x08,
	.l_x_axis = 0x80, .l_y_axis = 0x80, .r_x_axis = 0x80, .r_y_axis = 0x80,
	.right_axis = 0x00, .left_axis = 0x00, .up_axis = 0x00, .down_axis = 0x00,
	.triangle_axis = 0x00, .circle_axis = 0x00, .cross_axis = 0x00, .square_axis = 0x00,
	.l1_axis = 0x00, .r1_axis = 0x00, .l2_axis = 0x00, .r2_axis = 0x00
};

static PS4Report ps4Report
{
	.report_id = 0x01,
	.left_stick_x = 0x80, .left_stick_y = 0x80, .right_stick_x = 0x80, .right_stick_y = 0x80,
	.dpad = 0x08,
	.button_west = 0, .button_south = 0, .button_east = 0, .button_north = 0,
	.button_l1 = 0, .button_r1 = 0, .button_l2 = 0, .button_r2 = 0,
	.button_select = 0, .button_start = 0, .button_l3 = 0, .button_r3 = 0, .button_home = 0,	
	.padding = 0,
	.mystery = { },
	.touchpad_data = TouchpadData(),
	.mystery_2 = { }
};

static SwitchReport switchReport
{
	.buttons = 0,
	.hat = SWITCH_HAT_NOTHING,
	.lx = SWITCH_JOYSTICK_MID,
	.ly = SWITCH_JOYSTICK_MID,
	.rx = SWITCH_JOYSTICK_MID,
	.ry = SWITCH_JOYSTICK_MID,
	.vendor = 0,
};

static XInputReport xinputReport
{
	.report_id = 0,
	.report_size = XINPUT_ENDPOINT_SIZE,
	.buttons1 = 0,
	.buttons2 = 0,
	.lt = 0,
	.rt = 0,
	.lx = GAMEPAD_JOYSTICK_MID,
	.ly = GAMEPAD_JOYSTICK_MID,
	.rx = GAMEPAD_JOYSTICK_MID,
	.ry = GAMEPAD_JOYSTICK_MID,
	._reserved = { },
};

static TouchpadData touchpadData;
static uint8_t last_report_counter = 0;


static KeyboardReport keyboardReport
{
	.keycode = { 0 }
};

void Gamepad::setup()
{
	//load(); // MPGS loads
	options = mpgStorage->getGamepadOptions();

	// Configure pin mapping
	f2Mask = (GAMEPAD_MASK_A1 | GAMEPAD_MASK_S2);
	const BoardOptions& boardOptions = Storage::getInstance().getBoardOptions();

	mapP1DpadUp    = new GamepadButtonMapping(boardOptions.pinP1DpadUp,    GAMEPAD_MASK_UP);
	mapP1DpadDown  = new GamepadButtonMapping(boardOptions.pinP1DpadDown,  GAMEPAD_MASK_DOWN);
	mapP1DpadLeft  = new GamepadButtonMapping(boardOptions.pinP1DpadLeft,  GAMEPAD_MASK_LEFT);
	mapP1DpadRight = new GamepadButtonMapping(boardOptions.pinP1DpadRight, GAMEPAD_MASK_RIGHT);
	mapP1ButtonB1  = new GamepadButtonMapping(boardOptions.pinP1ButtonB1,  GAMEPAD_MASK_B1);
	mapP1ButtonB2  = new GamepadButtonMapping(boardOptions.pinP1ButtonB2,  GAMEPAD_MASK_B2);
	mapP1ButtonB3  = new GamepadButtonMapping(boardOptions.pinP1ButtonB3,  GAMEPAD_MASK_B3);
	mapP1ButtonB4  = new GamepadButtonMapping(boardOptions.pinP1ButtonB4,  GAMEPAD_MASK_B4);
	mapP1ButtonL1  = new GamepadButtonMapping(boardOptions.pinP1ButtonL1,  GAMEPAD_MASK_L1);
	mapP1ButtonR1  = new GamepadButtonMapping(boardOptions.pinP1ButtonR1,  GAMEPAD_MASK_R1);
	mapP1ButtonL2  = new GamepadButtonMapping(boardOptions.pinP1ButtonL2,  GAMEPAD_MASK_L2);
	mapP1ButtonR2  = new GamepadButtonMapping(boardOptions.pinP1ButtonR2,  GAMEPAD_MASK_R2);
	mapP1ButtonS1  = new GamepadButtonMapping(boardOptions.pinP1ButtonS1,  GAMEPAD_MASK_S1);
	mapP1ButtonS2  = new GamepadButtonMapping(boardOptions.pinP1ButtonS2,  GAMEPAD_MASK_S2);
	mapP1ButtonL3  = new GamepadButtonMapping(boardOptions.pinP1ButtonL3,  GAMEPAD_MASK_L3);
	mapP1ButtonR3  = new GamepadButtonMapping(boardOptions.pinP1ButtonR3,  GAMEPAD_MASK_R3);
	mapP1ButtonA1  = new GamepadButtonMapping(boardOptions.pinP1ButtonA1,  GAMEPAD_MASK_A1);
	mapP1ButtonA2  = new GamepadButtonMapping(boardOptions.pinP1ButtonA2,  GAMEPAD_MASK_A2);

	mapP2DpadUp    = new GamepadButtonMapping(boardOptions.pinP2DpadUp,    GAMEPAD_MASK_UP);
	mapP2DpadDown  = new GamepadButtonMapping(boardOptions.pinP2DpadDown,  GAMEPAD_MASK_DOWN);
	mapP2DpadLeft  = new GamepadButtonMapping(boardOptions.pinP2DpadLeft,  GAMEPAD_MASK_LEFT);
	mapP2DpadRight = new GamepadButtonMapping(boardOptions.pinP2DpadRight, GAMEPAD_MASK_RIGHT);
	mapP2ButtonB1  = new GamepadButtonMapping(boardOptions.pinP2ButtonB1,  GAMEPAD_MASK_B1);
	mapP2ButtonB2  = new GamepadButtonMapping(boardOptions.pinP2ButtonB2,  GAMEPAD_MASK_B2);
	mapP2ButtonB3  = new GamepadButtonMapping(boardOptions.pinP2ButtonB3,  GAMEPAD_MASK_B3);
	mapP2ButtonB4  = new GamepadButtonMapping(boardOptions.pinP2ButtonB4,  GAMEPAD_MASK_B4);
	mapP2ButtonL1  = new GamepadButtonMapping(boardOptions.pinP2ButtonL1,  GAMEPAD_MASK_L1);
	mapP2ButtonR1  = new GamepadButtonMapping(boardOptions.pinP2ButtonR1,  GAMEPAD_MASK_R1);
	mapP2ButtonL2  = new GamepadButtonMapping(boardOptions.pinP2ButtonL2,  GAMEPAD_MASK_L2);
	mapP2ButtonR2  = new GamepadButtonMapping(boardOptions.pinP2ButtonR2,  GAMEPAD_MASK_R2);
	mapP2ButtonS1  = new GamepadButtonMapping(boardOptions.pinP2ButtonS1,  GAMEPAD_MASK_S1);
	mapP2ButtonS2  = new GamepadButtonMapping(boardOptions.pinP2ButtonS2,  GAMEPAD_MASK_S2);
	mapP2ButtonL3  = new GamepadButtonMapping(boardOptions.pinP2ButtonL3,  GAMEPAD_MASK_L3);
	mapP2ButtonR3  = new GamepadButtonMapping(boardOptions.pinP2ButtonR3,  GAMEPAD_MASK_R3);
	mapP2ButtonA1  = new GamepadButtonMapping(boardOptions.pinP2ButtonA1,  GAMEPAD_MASK_A1);
	mapP2ButtonA2  = new GamepadButtonMapping(boardOptions.pinP2ButtonA2,  GAMEPAD_MASK_A2);

	gamepadMappings = new GamepadButtonMapping *[GAMEPAD_DIGITAL_INPUT_COUNT]
	{
		mapP1DpadUp,   mapP1DpadDown, mapP1DpadLeft, mapP1DpadRight,
		mapP1ButtonB1, mapP1ButtonB2, mapP1ButtonB3, mapP1ButtonB4,
		mapP1ButtonL1, mapP1ButtonR1, mapP1ButtonL2, mapP1ButtonR2,
		mapP1ButtonS1, mapP1ButtonS2, mapP1ButtonL3, mapP1ButtonR3,
		mapP1ButtonA1, mapP1ButtonA2, mapP2DpadUp, mapP2DpadDown,
		mapP2DpadLeft, mapP2DpadRight, mapP2ButtonB1, mapP2ButtonB2,
		mapP2ButtonB3, mapP2ButtonB4,  mapP2ButtonL1, mapP2ButtonR1,
		mapP2ButtonL2, mapP2ButtonR2, mapP2ButtonS1, mapP2ButtonS2,
		mapP2ButtonL3, mapP2ButtonR3, mapP2ButtonA1, mapP2ButtonA2
	};

	for (int i = 0; i < GAMEPAD_DIGITAL_INPUT_COUNT; i++)
	{
		if (gamepadMappings[i]->isAssigned())
		{
			gpio_init(gamepadMappings[i]->pin);             // Initialize pin
			gpio_set_dir(gamepadMappings[i]->pin, GPIO_IN); // Set as INPUT
			gpio_pull_up(gamepadMappings[i]->pin);          // Set as PULLUP
		}
	}

	#ifdef PIN_SETTINGS
		gpio_init(PIN_SETTINGS);             // Initialize pin
		gpio_set_dir(PIN_SETTINGS, GPIO_IN); // Set as INPUT
		gpio_pull_up(PIN_SETTINGS);          // Set as PULLUP
	#endif

	hotkeyF1Up    =	options.hotkeyF1Up;
	hotkeyF1Down  =	options.hotkeyF1Down;
	hotkeyF1Left  =	options.hotkeyF1Left;
	hotkeyF1Right =	options.hotkeyF1Right;
	hotkeyF2Up 	  =	options.hotkeyF2Up;
	hotkeyF2Down  =	options.hotkeyF2Down;
	hotkeyF2Left  =	options.hotkeyF2Left;
	hotkeyF2Right =	options.hotkeyF2Right;
}

void Gamepad::process()
{
	memcpy(&rawState, &p1State, sizeof(GamepadState));

	p1State.dpad = runSOCDCleaner(resolveSOCDMode(options), p1State.dpad);
	p2State.dpad = runSOCDCleaner(resolveSOCDMode(options), p2State.dpad);

	switch (options.dpadMode)
	{
		case DpadMode::DPAD_MODE_LEFT_ANALOG:
			if (!hasRightAnalogStick) {
				p1State.rx = GAMEPAD_JOYSTICK_MID;
				p1State.ry = GAMEPAD_JOYSTICK_MID;
				p2State.rx = GAMEPAD_JOYSTICK_MID;
				p2State.ry = GAMEPAD_JOYSTICK_MID;
			}
			p1State.lx = dpadToAnalogX(p1State.dpad);
			p1State.ly = dpadToAnalogY(p1State.dpad);
			p1State.dpad = 0;
			p2State.lx = dpadToAnalogX(p2State.dpad);
			p2State.ly = dpadToAnalogY(p2State.dpad);
			p2State.dpad = 0;
			break;

		case DpadMode::DPAD_MODE_RIGHT_ANALOG:
			if (!hasLeftAnalogStick) {
				p1State.lx = GAMEPAD_JOYSTICK_MID;
				p1State.ly = GAMEPAD_JOYSTICK_MID;
				p2State.lx = GAMEPAD_JOYSTICK_MID;
				p2State.ly = GAMEPAD_JOYSTICK_MID;
			}
			p1State.rx = dpadToAnalogX(p1State.dpad);
			p1State.ry = dpadToAnalogY(p1State.dpad);
			p1State.dpad = 0;
			p2State.rx = dpadToAnalogX(p2State.dpad);
			p2State.ry = dpadToAnalogY(p2State.dpad);
			p2State.dpad = 0;
			break;

		default:
			if (!hasLeftAnalogStick) {
				p1State.lx = GAMEPAD_JOYSTICK_MID;
				p1State.ly = GAMEPAD_JOYSTICK_MID;
				p2State.lx = GAMEPAD_JOYSTICK_MID;
				p2State.ly = GAMEPAD_JOYSTICK_MID;
			}
			if (!hasRightAnalogStick) {
				p1State.rx = GAMEPAD_JOYSTICK_MID;
				p1State.ry = GAMEPAD_JOYSTICK_MID;
				p2State.rx = GAMEPAD_JOYSTICK_MID;
				p2State.ry = GAMEPAD_JOYSTICK_MID;
			}
			break;
	}
}

void Gamepad::read()
{
	// Need to invert since we're using pullups
	uint32_t values = ~gpio_get_all();

	#ifdef PIN_SETTINGS
	p1State.aux = 0
		| ((values & (1 << PIN_SETTINGS)) ? (1 << 0) : 0)
	;
	p2State.aux = 0
		| ((values & (1 << PIN_SETTINGS)) ? (1 << 0) : 0)
	;
	#endif

	p1State.dpad = 0
		| ((values & mapP1DpadUp->pinMask)    ? (options.invertYAxis ? mapP1DpadDown->buttonMask : mapP1DpadUp->buttonMask) : 0)
		| ((values & mapP1DpadDown->pinMask)  ? (options.invertYAxis ? mapP1DpadUp->buttonMask : mapP1DpadDown->buttonMask) : 0)
		| ((values & mapP1DpadLeft->pinMask)  ? mapP1DpadLeft->buttonMask  : 0)
		| ((values & mapP1DpadRight->pinMask) ? mapP1DpadRight->buttonMask : 0)
	;

	p2State.dpad = 0
		| ((values & mapP2DpadUp->pinMask)    ? (options.invertYAxis ? mapP2DpadDown->buttonMask : mapP2DpadUp->buttonMask) : 0)
		| ((values & mapP2DpadDown->pinMask)  ? (options.invertYAxis ? mapP2DpadUp->buttonMask : mapP2DpadDown->buttonMask) : 0)
		| ((values & mapP2DpadLeft->pinMask)  ? mapP2DpadLeft->buttonMask  : 0)
		| ((values & mapP2DpadRight->pinMask) ? mapP2DpadRight->buttonMask : 0)
	;

	p1State.buttons = 0
		| ((values & mapP1ButtonB1->pinMask)  ? mapP1ButtonB1->buttonMask  : 0)
		| ((values & mapP1ButtonB2->pinMask)  ? mapP1ButtonB2->buttonMask  : 0)
		| ((values & mapP1ButtonB3->pinMask)  ? mapP1ButtonB3->buttonMask  : 0)
		| ((values & mapP1ButtonB4->pinMask)  ? mapP1ButtonB4->buttonMask  : 0)
		| ((values & mapP1ButtonL1->pinMask)  ? mapP1ButtonL1->buttonMask  : 0)
		| ((values & mapP1ButtonR1->pinMask)  ? mapP1ButtonR1->buttonMask  : 0)
		| ((values & mapP1ButtonL2->pinMask)  ? mapP1ButtonL2->buttonMask  : 0)
		| ((values & mapP1ButtonR2->pinMask)  ? mapP1ButtonR2->buttonMask  : 0)
		| ((values & mapP1ButtonS1->pinMask)  ? mapP1ButtonS1->buttonMask  : 0)
		| ((values & mapP1ButtonS2->pinMask)  ? mapP1ButtonS2->buttonMask  : 0)
		| ((values & mapP1ButtonL3->pinMask)  ? mapP1ButtonL3->buttonMask  : 0)
		| ((values & mapP1ButtonR3->pinMask)  ? mapP1ButtonR3->buttonMask  : 0)
		| ((values & mapP1ButtonA1->pinMask)  ? mapP1ButtonA1->buttonMask  : 0)
		| ((values & mapP1ButtonA2->pinMask)  ? mapP1ButtonA2->buttonMask  : 0)
	;

	p2State.buttons = 0
		| ((values & mapP2ButtonB1->pinMask)  ? mapP2ButtonB1->buttonMask  : 0)
		| ((values & mapP2ButtonB2->pinMask)  ? mapP2ButtonB2->buttonMask  : 0)
		| ((values & mapP2ButtonB3->pinMask)  ? mapP2ButtonB3->buttonMask  : 0)
		| ((values & mapP2ButtonB4->pinMask)  ? mapP2ButtonB4->buttonMask  : 0)
		| ((values & mapP2ButtonL1->pinMask)  ? mapP2ButtonL1->buttonMask  : 0)
		| ((values & mapP2ButtonR1->pinMask)  ? mapP2ButtonR1->buttonMask  : 0)
		| ((values & mapP2ButtonL2->pinMask)  ? mapP2ButtonL2->buttonMask  : 0)
		| ((values & mapP2ButtonR2->pinMask)  ? mapP2ButtonR2->buttonMask  : 0)
		| ((values & mapP2ButtonS1->pinMask)  ? mapP2ButtonS1->buttonMask  : 0)
		| ((values & mapP2ButtonS2->pinMask)  ? mapP2ButtonS2->buttonMask  : 0)
		| ((values & mapP2ButtonL3->pinMask)  ? mapP2ButtonL3->buttonMask  : 0)
		| ((values & mapP2ButtonR3->pinMask)  ? mapP2ButtonR3->buttonMask  : 0)
		| ((values & mapP2ButtonA1->pinMask)  ? mapP2ButtonA1->buttonMask  : 0)
		| ((values & mapP2ButtonA2->pinMask)  ? mapP2ButtonA2->buttonMask  : 0)
	;

	p1State.lx = GAMEPAD_JOYSTICK_MID;
	p1State.ly = GAMEPAD_JOYSTICK_MID;
	p1State.rx = GAMEPAD_JOYSTICK_MID;
	p1State.ry = GAMEPAD_JOYSTICK_MID;
	p1State.lt = 0;
	p1State.rt = 0;

	p2State.lx = GAMEPAD_JOYSTICK_MID;
	p2State.ly = GAMEPAD_JOYSTICK_MID;
	p2State.rx = GAMEPAD_JOYSTICK_MID;
	p2State.ry = GAMEPAD_JOYSTICK_MID;
	p2State.lt = 0;
	p2State.rt = 0;
}

void Gamepad::debounce() {
	debouncer.debounce(&p1State);
	debouncer.debounce(&p2State);
}

void Gamepad::save()
{
	bool dirty = false;
	GamepadOptions savedOptions = mpgStorage->getGamepadOptions();
	if (memcmp(&savedOptions, &options, sizeof(GamepadOptions)))
	{
		mpgStorage->setGamepadOptions(options);
		dirty = true;
	}

	if (dirty)
		mpgStorage->save();
}

GamepadHotkey Gamepad::hotkey()
{
	static GamepadHotkey lastAction = HOTKEY_NONE;
	GamepadHotkey action = HOTKEY_NONE;
	if (pressedF1())
	{
		if (p1State.dpad == hotkeyF1Up   .dpadMask) action = hotkeyF1Up   .action;
		if (p1State.dpad == hotkeyF1Down .dpadMask) action = hotkeyF1Down .action;
		if (p1State.dpad == hotkeyF1Left .dpadMask) action = hotkeyF1Left .action;
		if (p1State.dpad == hotkeyF1Right.dpadMask) action = hotkeyF1Right.action;
		if (action != HOTKEY_NONE) {
			p1State.dpad = 0;
			p1State.buttons &= ~(f1Mask);
		}
	} else if (pressedF2()) {
		if (p1State.dpad == hotkeyF2Up   .dpadMask) action = hotkeyF2Up   .action;
		if (p1State.dpad == hotkeyF2Down .dpadMask) action = hotkeyF2Down .action;
		if (p1State.dpad == hotkeyF2Left .dpadMask) action = hotkeyF2Left .action;
		if (p1State.dpad == hotkeyF2Right.dpadMask) action = hotkeyF2Right.action;
		if (action != HOTKEY_NONE) {
			p1State.dpad = 0;
			p1State.buttons &= ~(f2Mask);
		}
	}

	switch (action) {
		case HOTKEY_NONE              : return action;
		case HOTKEY_DPAD_DIGITAL      : options.dpadMode = DPAD_MODE_DIGITAL; break;
		case HOTKEY_DPAD_LEFT_ANALOG  : options.dpadMode = DPAD_MODE_LEFT_ANALOG; break;
		case HOTKEY_DPAD_RIGHT_ANALOG : options.dpadMode = DPAD_MODE_RIGHT_ANALOG; break;
		case HOTKEY_HOME_BUTTON       : p1State.buttons |= GAMEPAD_MASK_A1; break; // Press the Home button
		case HOTKEY_CAPTURE_BUTTON    :
			break;
		case HOTKEY_SOCD_UP_PRIORITY  : options.socdMode = SOCD_MODE_UP_PRIORITY; break;
		case HOTKEY_SOCD_NEUTRAL      : options.socdMode = SOCD_MODE_NEUTRAL; break;
		case HOTKEY_SOCD_LAST_INPUT   : options.socdMode = SOCD_MODE_SECOND_INPUT_PRIORITY; break;
		case HOTKEY_SOCD_FIRST_INPUT  : options.socdMode = SOCD_MODE_FIRST_INPUT_PRIORITY; break;
		case HOTKEY_SOCD_BYPASS       : options.socdMode = SOCD_MODE_BYPASS; break;
		case HOTKEY_INVERT_X_AXIS     : break;
		case HOTKEY_INVERT_Y_AXIS     :
			if (lastAction != HOTKEY_INVERT_Y_AXIS)
				options.invertYAxis = !options.invertYAxis;
			break;
	}

	GamepadHotkey hotkey = action;
	if (hotkey != GamepadHotkey::HOTKEY_NONE)
		save();

	return hotkey;
}


void * Gamepad::getReport()
{
	switch (options.inputMode)
	{
		case INPUT_MODE_XINPUT:
			return getXInputReport();

		case INPUT_MODE_SWITCH:
			return getSwitchReport();

		case INPUT_MODE_PS4:
			return getPS4Report();

		case INPUT_MODE_KEYBOARD:
			return getKeyboardReport();

		default:
			return getHIDReport();
	}
}


uint16_t Gamepad::getReportSize()
{
	switch (options.inputMode)
	{
		case INPUT_MODE_XINPUT:
			return sizeof(XInputReport);

		case INPUT_MODE_SWITCH:
			return sizeof(SwitchReport);

		case INPUT_MODE_PS4:
			return sizeof(PS4Report);

		case INPUT_MODE_KEYBOARD:
			return sizeof(KeyboardReport);

		default:
			return sizeof(HIDReport);
	}
}


HIDReport *Gamepad::getHIDReport()
{
	switch (p1State.dpad & GAMEPAD_MASK_DPAD)
	{
		case GAMEPAD_MASK_UP:                        hidReport.direction = HID_HAT_UP;        break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   hidReport.direction = HID_HAT_UPRIGHT;   break;
		case GAMEPAD_MASK_RIGHT:                     hidReport.direction = HID_HAT_RIGHT;     break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: hidReport.direction = HID_HAT_DOWNRIGHT; break;
		case GAMEPAD_MASK_DOWN:                      hidReport.direction = HID_HAT_DOWN;      break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  hidReport.direction = HID_HAT_DOWNLEFT;  break;
		case GAMEPAD_MASK_LEFT:                      hidReport.direction = HID_HAT_LEFT;      break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    hidReport.direction = HID_HAT_UPLEFT;    break;
		default:                                     hidReport.direction = HID_HAT_NOTHING;   break;
	}

	hidReport.cross_btn    = pressedP1B1();
	hidReport.circle_btn   = pressedP1B2();
	hidReport.square_btn   = pressedP1B3();
	hidReport.triangle_btn = pressedP1B4();
	hidReport.l1_btn       = pressedP1L1();
	hidReport.r1_btn       = pressedP1R1();
	hidReport.l2_btn       = pressedP1L2();
	hidReport.r2_btn       = pressedP1R2();
	hidReport.select_btn   = pressedP1S1();
	hidReport.start_btn    = pressedP1S2();
	hidReport.l3_btn       = pressedP1L3();
	hidReport.r3_btn       = pressedP1R3();
	hidReport.ps_btn       = pressedP1A1();
	hidReport.tp_btn       = pressedP1A2();

	hidReport.l_x_axis = static_cast<uint8_t>(p1State.lx >> 8);
	hidReport.l_y_axis = static_cast<uint8_t>(p1State.ly >> 8);
	hidReport.r_x_axis = static_cast<uint8_t>(p1State.rx >> 8);
	hidReport.r_y_axis = static_cast<uint8_t>(p1State.ry >> 8);

	return &hidReport;
}


SwitchReport *Gamepad::getSwitchReport()
{
	switch (p1State.dpad & GAMEPAD_MASK_DPAD)
	{
		case GAMEPAD_MASK_UP:                        switchReport.hat = SWITCH_HAT_UP;        break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   switchReport.hat = SWITCH_HAT_UPRIGHT;   break;
		case GAMEPAD_MASK_RIGHT:                     switchReport.hat = SWITCH_HAT_RIGHT;     break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: switchReport.hat = SWITCH_HAT_DOWNRIGHT; break;
		case GAMEPAD_MASK_DOWN:                      switchReport.hat = SWITCH_HAT_DOWN;      break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  switchReport.hat = SWITCH_HAT_DOWNLEFT;  break;
		case GAMEPAD_MASK_LEFT:                      switchReport.hat = SWITCH_HAT_LEFT;      break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    switchReport.hat = SWITCH_HAT_UPLEFT;    break;
		default:                                     switchReport.hat = SWITCH_HAT_NOTHING;   break;
	}

	switchReport.buttons = 0
		| (pressedP1B1() ? SWITCH_MASK_B       : 0)
		| (pressedP1B2() ? SWITCH_MASK_A       : 0)
		| (pressedP1B3() ? SWITCH_MASK_Y       : 0)
		| (pressedP1B4() ? SWITCH_MASK_X       : 0)
		| (pressedP1L1() ? SWITCH_MASK_L       : 0)
		| (pressedP1R1() ? SWITCH_MASK_R       : 0)
		| (pressedP1L2() ? SWITCH_MASK_ZL      : 0)
		| (pressedP1R2() ? SWITCH_MASK_ZR      : 0)
		| (pressedP1S1() ? SWITCH_MASK_MINUS   : 0)
		| (pressedP1S2() ? SWITCH_MASK_PLUS    : 0)
		| (pressedP1L3() ? SWITCH_MASK_L3      : 0)
		| (pressedP1R3() ? SWITCH_MASK_R3      : 0)
		| (pressedP1A1() ? SWITCH_MASK_HOME    : 0)
		| (pressedP1A2() ? SWITCH_MASK_CAPTURE : 0)
	;

	switchReport.lx = static_cast<uint8_t>(p1State.lx >> 8);
	switchReport.ly = static_cast<uint8_t>(p1State.ly >> 8);
	switchReport.rx = static_cast<uint8_t>(p1State.rx >> 8);
	switchReport.ry = static_cast<uint8_t>(p1State.ry >> 8);

	return &switchReport;
}


XInputReport *Gamepad::getXInputReport()
{
	xinputReport.buttons1 = 0
		| (pressedP1Up()    ? XBOX_MASK_UP    : 0)
		| (pressedP1Down()  ? XBOX_MASK_DOWN  : 0)
		| (pressedP1Left()  ? XBOX_MASK_LEFT  : 0)
		| (pressedP1Right() ? XBOX_MASK_RIGHT : 0)
		| (pressedP1S2()    ? XBOX_MASK_START : 0)
		| (pressedP1S1()    ? XBOX_MASK_BACK  : 0)
		| (pressedP1L3()    ? XBOX_MASK_LS    : 0)
		| (pressedP1R3()    ? XBOX_MASK_RS    : 0)
	;

	xinputReport.buttons2 = 0
		| (pressedP1L1() ? XBOX_MASK_LB   : 0)
		| (pressedP1R1() ? XBOX_MASK_RB   : 0)
		| (pressedP1A1() ? XBOX_MASK_HOME : 0)
		| (pressedP1B1() ? XBOX_MASK_A    : 0)
		| (pressedP1B2() ? XBOX_MASK_B    : 0)
		| (pressedP1B3() ? XBOX_MASK_X    : 0)
		| (pressedP1B4() ? XBOX_MASK_Y    : 0)
	;

	xinputReport.lx = static_cast<int16_t>(p1State.lx) + INT16_MIN;
	xinputReport.ly = static_cast<int16_t>(~p1State.ly) + INT16_MIN;
	xinputReport.rx = static_cast<int16_t>(p1State.rx) + INT16_MIN;
	xinputReport.ry = static_cast<int16_t>(~p1State.ry) + INT16_MIN;

	if (hasAnalogTriggers)
	{
		xinputReport.lt = p1State.lt;
		xinputReport.rt = p1State.rt;
	}
	else
	{
		xinputReport.lt = pressedP1L2() ? 0xFF : 0;
		xinputReport.rt = pressedP1R2() ? 0xFF : 0;
	}

	return &xinputReport;
}


PS4Report *Gamepad::getPS4Report()
{
	switch (p1State.dpad & GAMEPAD_MASK_DPAD)
	{
		case GAMEPAD_MASK_UP:                        ps4Report.dpad = HID_HAT_UP;        break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   ps4Report.dpad = HID_HAT_UPRIGHT;   break;
		case GAMEPAD_MASK_RIGHT:                     ps4Report.dpad = HID_HAT_RIGHT;     break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: ps4Report.dpad = HID_HAT_DOWNRIGHT; break;
		case GAMEPAD_MASK_DOWN:                      ps4Report.dpad = HID_HAT_DOWN;      break;
		case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  ps4Report.dpad = HID_HAT_DOWNLEFT;  break;
		case GAMEPAD_MASK_LEFT:                      ps4Report.dpad = HID_HAT_LEFT;      break;
		case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    ps4Report.dpad = HID_HAT_UPLEFT;    break;
		default:                                     ps4Report.dpad = PS4_HAT_NOTHING;   break;
	}

	ps4Report.button_south    = pressedP1B1();
	ps4Report.button_east     = pressedP1B2();
	ps4Report.button_west     = pressedP1B3();
	ps4Report.button_north    = pressedP1B4();
	ps4Report.button_l1       = pressedP1L1();
	ps4Report.button_r1       = pressedP1R1();
	ps4Report.button_l2       = pressedP1L2();
	ps4Report.button_r2       = pressedP1R2();
	ps4Report.button_select   = pressedP1S1();
	ps4Report.button_start    = pressedP1S2();
	ps4Report.button_l3       = pressedP1L3();
	ps4Report.button_r3       = pressedP1R3();
	ps4Report.button_home     = pressedP1A1();
	ps4Report.button_touchpad = pressedP1A2();

	// report counter is 6 bits, but we circle 0-255
	ps4Report.report_counter = last_report_counter++;

	ps4Report.left_stick_x = static_cast<uint8_t>(p1State.lx >> 8);
	ps4Report.left_stick_y = static_cast<uint8_t>(p1State.ly >> 8);
	ps4Report.right_stick_x = static_cast<uint8_t>(p1State.rx >> 8);
	ps4Report.right_stick_y = static_cast<uint8_t>(p1State.ry >> 8);

	ps4Report.left_trigger = 0;
	ps4Report.right_trigger = 0;

	// set touchpad to nothing
	touchpadData.p1.unpressed = 1;
	touchpadData.p2.unpressed = 1;
	ps4Report.touchpad_data = touchpadData;

	return &ps4Report;
}

uint8_t Gamepad::getModifier(uint8_t code) {
	switch (code) {
		case HID_KEY_CONTROL_LEFT : return KEYBOARD_MODIFIER_LEFTCTRL  ;
		case HID_KEY_SHIFT_LEFT   : return KEYBOARD_MODIFIER_LEFTSHIFT ;
		case HID_KEY_ALT_LEFT     : return KEYBOARD_MODIFIER_LEFTALT   ;
		case HID_KEY_GUI_LEFT     : return KEYBOARD_MODIFIER_LEFTGUI   ;
		case HID_KEY_CONTROL_RIGHT: return KEYBOARD_MODIFIER_RIGHTCTRL ;
		case HID_KEY_SHIFT_RIGHT  : return KEYBOARD_MODIFIER_RIGHTSHIFT;
		case HID_KEY_ALT_RIGHT    : return KEYBOARD_MODIFIER_RIGHTALT  ;
		case HID_KEY_GUI_RIGHT    : return KEYBOARD_MODIFIER_RIGHTGUI  ;
	}

	return 0;
}

void Gamepad::pressKey(uint8_t code) {
	if (code >= HID_KEY_CONTROL_LEFT) {
		keyboardReport.keycode[0] |= getModifier(code);
	} else if ((code >> 3) < KEY_COUNT - 2) {
		keyboardReport.keycode[(code >> 3) + 1] |= 1 << (code & 7);
	}
}

void Gamepad::releaseAllKeys(void) {
	for (uint8_t i = 0; i < (sizeof(keyboardReport.keycode) / sizeof(keyboardReport.keycode[0])); i++) {
		keyboardReport.keycode[i] = 0;
	}
}

KeyboardReport *Gamepad::getKeyboardReport()
{
	releaseAllKeys();
	if(pressedP1Up())     { pressKey(options.keyP1DpadUp); }
	if(pressedP1Down())   { pressKey(options.keyP1DpadDown); }
	if(pressedP1Left())	  { pressKey(options.keyP1DpadLeft); }
	if(pressedP1Right())  { pressKey(options.keyP1DpadRight); }
	if(pressedP1B1()) 	  { pressKey(options.keyP1ButtonB1); }
	if(pressedP1B2()) 	  { pressKey(options.keyP1ButtonB2); }
	if(pressedP1B3()) 	  { pressKey(options.keyP1ButtonB3); }
	if(pressedP1B4()) 	  { pressKey(options.keyP1ButtonB4); }
	if(pressedP1L1()) 	  { pressKey(options.keyP1ButtonL1); }
	if(pressedP1R1()) 	  { pressKey(options.keyP1ButtonR1); }
	if(pressedP1L2()) 	  { pressKey(options.keyP1ButtonL2); }
	if(pressedP1R2()) 	  { pressKey(options.keyP1ButtonR2); }
	if(pressedP1S1()) 	  { pressKey(options.keyP1ButtonS1); }
	if(pressedP1S2()) 	  { pressKey(options.keyP1ButtonS2); }
	if(pressedP1L3()) 	  { pressKey(options.keyP1ButtonL3); }
	if(pressedP1R3()) 	  { pressKey(options.keyP1ButtonR3); }
	if(pressedP1A1()) 	  { pressKey(options.keyP1ButtonA1); }
	if(pressedP1A2()) 	  { pressKey(options.keyP1ButtonA2); }
	if(pressedP2Up())     { pressKey(options.keyP2DpadUp); }
	if(pressedP2Down())   { pressKey(options.keyP2DpadDown); }
	if(pressedP2Left())	  { pressKey(options.keyP2DpadLeft); }
	if(pressedP2Right())  { pressKey(options.keyP2DpadRight); }
	if(pressedP2B1()) 	  { pressKey(options.keyP2ButtonB1); }
	if(pressedP2B2()) 	  { pressKey(options.keyP2ButtonB2); }
	if(pressedP2B3()) 	  { pressKey(options.keyP2ButtonB3); }
	if(pressedP2B4()) 	  { pressKey(options.keyP2ButtonB4); }
	if(pressedP2L1()) 	  { pressKey(options.keyP2ButtonL1); }
	if(pressedP2R1()) 	  { pressKey(options.keyP2ButtonR1); }
	if(pressedP2L2()) 	  { pressKey(options.keyP2ButtonL2); }
	if(pressedP2R2()) 	  { pressKey(options.keyP2ButtonR2); }
	if(pressedP2S1()) 	  { pressKey(options.keyP2ButtonS1); }
	if(pressedP2S2()) 	  { pressKey(options.keyP2ButtonS2); }
	if(pressedP2L3()) 	  { pressKey(options.keyP2ButtonL3); }
	if(pressedP2R3()) 	  { pressKey(options.keyP2ButtonR3); }
	if(pressedP2A1()) 	  { pressKey(options.keyP2ButtonA1); }
	if(pressedP2A2()) 	  { pressKey(options.keyP2ButtonA2); }
	return &keyboardReport;
}


/* Gamepad stuffs */
void GamepadStorage::start()
{
	//EEPROM.start();
}

void GamepadStorage::save()
{
	EEPROM.commit();
}

GamepadOptions GamepadStorage::getGamepadOptions()
{
	GamepadOptions options;
	EEPROM.get(GAMEPAD_STORAGE_INDEX, options);

	uint32_t lastCRC = options.checksum;
	options.checksum = CHECKSUM_MAGIC;
	if (CRC32::calculate(&options) != lastCRC)
	{
		#ifdef DEFAULT_INPUT_MODE
		options.inputMode = DEFAULT_INPUT_MODE;
		#else
		options.inputMode = InputMode::INPUT_MODE_XINPUT; // Default?
		#endif
		#ifdef DEFAULT_DPAD_MODE
		options.dpadMode = DEFAULT_DPAD_MODE;
		#else
		options.dpadMode = DpadMode::DPAD_MODE_DIGITAL; // Default?
		#endif
		#ifdef DEFAULT_SOCD_MODE
		options.socdMode = DEFAULT_SOCD_MODE;
		#else
		options.socdMode = SOCD_MODE_NEUTRAL;
		#endif
		options.invertXAxis = false;
		options.invertYAxis = false;
		options.keyP1DpadUp    = KEY_P1_DPAD_UP;
		options.keyP1DpadDown  = KEY_P1_DPAD_DOWN;
		options.keyP1DpadRight = KEY_P1_DPAD_RIGHT;
		options.keyP1DpadLeft  = KEY_P1_DPAD_LEFT;
		options.keyP1ButtonB1  = KEY_P1_BUTTON_B1;
		options.keyP1ButtonB2  = KEY_P1_BUTTON_B2;
		options.keyP1ButtonR2  = KEY_P1_BUTTON_R2;
		options.keyP1ButtonL2  = KEY_P1_BUTTON_L2;
		options.keyP1ButtonB3  = KEY_P1_BUTTON_B3;
		options.keyP1ButtonB4  = KEY_P1_BUTTON_B4;
		options.keyP1ButtonR1  = KEY_P1_BUTTON_R1;
		options.keyP1ButtonL1  = KEY_P1_BUTTON_L1;
		options.keyP1ButtonS1  = KEY_P1_BUTTON_S1;
		options.keyP1ButtonS2  = KEY_P1_BUTTON_S2;
		options.keyP1ButtonL3  = KEY_P1_BUTTON_L3;
		options.keyP1ButtonR3  = KEY_P1_BUTTON_R3;
		options.keyP1ButtonA1  = KEY_P1_BUTTON_A1;
		options.keyP1ButtonA2  = KEY_P1_BUTTON_A2;
		options.keyP2DpadUp    = KEY_P2_DPAD_UP;
		options.keyP2DpadDown  = KEY_P2_DPAD_DOWN;
		options.keyP2DpadRight = KEY_P2_DPAD_RIGHT;
		options.keyP2DpadLeft  = KEY_P2_DPAD_LEFT;
		options.keyP2ButtonB1  = KEY_P2_BUTTON_B1;
		options.keyP2ButtonB2  = KEY_P2_BUTTON_B2;
		options.keyP2ButtonR2  = KEY_P2_BUTTON_R2;
		options.keyP2ButtonL2  = KEY_P2_BUTTON_L2;
		options.keyP2ButtonB3  = KEY_P2_BUTTON_B3;
		options.keyP2ButtonB4  = KEY_P2_BUTTON_B4;
		options.keyP2ButtonR1  = KEY_P2_BUTTON_R1;
		options.keyP2ButtonL1  = KEY_P2_BUTTON_L1;
		options.keyP2ButtonS1  = KEY_P2_BUTTON_S1;
		options.keyP2ButtonS2  = KEY_P2_BUTTON_S2;
		options.keyP2ButtonL3  = KEY_P2_BUTTON_L3;
		options.keyP2ButtonR3  = KEY_P2_BUTTON_R3;
		options.keyP2ButtonA1  = KEY_P2_BUTTON_A1;
		options.keyP2ButtonA2  = KEY_P2_BUTTON_A2;

	    options.hotkeyF1Up = { HOTKEY_F1_UP_MASK, HOTKEY_F1_UP_ACTION };
	    options.hotkeyF1Down = { HOTKEY_F1_DOWN_MASK, HOTKEY_F1_DOWN_ACTION };
	    options.hotkeyF1Left = { HOTKEY_F1_LEFT_MASK, HOTKEY_F1_LEFT_ACTION };
	    options.hotkeyF1Right = { HOTKEY_F1_RIGHT_MASK, HOTKEY_F1_RIGHT_ACTION };
	    options.hotkeyF2Up = { HOTKEY_F2_UP_MASK, HOTKEY_F2_UP_ACTION };
	    options.hotkeyF2Down = { HOTKEY_F2_DOWN_MASK, HOTKEY_F2_DOWN_ACTION };
	    options.hotkeyF2Left = { HOTKEY_F2_LEFT_MASK, HOTKEY_F2_LEFT_ACTION };
	    options.hotkeyF2Right = { HOTKEY_F2_RIGHT_MASK, HOTKEY_F2_RIGHT_ACTION };
		setGamepadOptions(options);
	}

	return options;
}

void GamepadStorage::setGamepadOptions(GamepadOptions options)
{
	options.checksum = 0;
	options.checksum = CRC32::calculate(&options);
	EEPROM.set(GAMEPAD_STORAGE_INDEX, options);
}

