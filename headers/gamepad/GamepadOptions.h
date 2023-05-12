/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#pragma once

#include <stdint.h>
#include "GamepadEnums.h"

typedef struct
{
	uint8_t dpadMask;
	GamepadHotkey action;
} GamepadHotkeyEntry;

struct GamepadOptions
{
	InputMode inputMode {InputMode::INPUT_MODE_XINPUT}; 
	DpadMode dpadMode {DpadMode::DPAD_MODE_DIGITAL};
	SOCDMode socdMode {SOCDMode::SOCD_MODE_NEUTRAL};
	bool invertXAxis;
	bool invertYAxis;

	// Player 1
	uint8_t keyP1DpadUp;
	uint8_t keyP1DpadDown;
	uint8_t keyP1DpadLeft;
	uint8_t keyP1DpadRight;
	uint8_t keyP1ButtonB1;
	uint8_t keyP1ButtonB2;
	uint8_t keyP1ButtonB3;
	uint8_t keyP1ButtonB4;
	uint8_t keyP1ButtonL1;
	uint8_t keyP1ButtonR1;
	uint8_t keyP1ButtonL2;
	uint8_t keyP1ButtonR2;
	uint8_t keyP1ButtonS1;
	uint8_t keyP1ButtonS2;
	uint8_t keyP1ButtonL3;
	uint8_t keyP1ButtonR3;
	uint8_t keyP1ButtonA1;
	uint8_t keyP1ButtonA2;

	// Player 2
	uint8_t keyP2DpadUp;
	uint8_t keyP2DpadDown;
	uint8_t keyP2DpadLeft;
	uint8_t keyP2DpadRight;
	uint8_t keyP2ButtonB1;
	uint8_t keyP2ButtonB2;
	uint8_t keyP2ButtonB3;
	uint8_t keyP2ButtonB4;
	uint8_t keyP2ButtonL1;
	uint8_t keyP2ButtonR1;
	uint8_t keyP2ButtonL2;
	uint8_t keyP2ButtonR2;
	uint8_t keyP2ButtonS1;
	uint8_t keyP2ButtonS2;
	uint8_t keyP2ButtonL3;
	uint8_t keyP2ButtonR3;
	uint8_t keyP2ButtonA1;
	uint8_t keyP2ButtonA2;

	GamepadHotkeyEntry hotkeyF1Up;
	GamepadHotkeyEntry hotkeyF1Down;
	GamepadHotkeyEntry hotkeyF1Left;
	GamepadHotkeyEntry hotkeyF1Right;
	GamepadHotkeyEntry hotkeyF2Up;
	GamepadHotkeyEntry hotkeyF2Down;
	GamepadHotkeyEntry hotkeyF2Left;
	GamepadHotkeyEntry hotkeyF2Right;

	uint32_t checksum;
};
