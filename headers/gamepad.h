#ifndef _GAMEPAD_H_
#define _GAMEPAD_H_

#include "BoardConfig.h"
#include <string.h>

#include "gamepad/GamepadDebouncer.h"
#include "gamepad/GamepadOptions.h"
#include "gamepad/GamepadState.h"
#include "gamepad/GamepadStorage.h"
#include "gamepad/descriptors/HIDDescriptors.h"
#include "gamepad/descriptors/SwitchDescriptors.h"
#include "gamepad/descriptors/XInputDescriptors.h"
#include "gamepad/descriptors/KeyboardDescriptors.h"
#include "gamepad/descriptors/PS4Descriptors.h"

#include "pico/stdlib.h"

// MUST BE DEFINED FOR MPG
extern uint32_t getMillis();
extern uint64_t getMicro();

#define GAMEPAD_POLL_MS 1
#define GAMEPAD_POLL_MICRO 100

#define GAMEPAD_FEATURE_REPORT_SIZE 32

struct GamepadButtonMapping
{
	GamepadButtonMapping(uint8_t p, uint16_t bm) : 
		pin(p < NUM_BANK0_GPIOS ? p : 0xff),
		pinMask(p < NUM_BANK0_GPIOS? (1 << p) : 0),
		buttonMask(bm)
	{}

	uint8_t pin;
	uint32_t pinMask;
	const uint16_t buttonMask;

	inline void setPin(uint8_t p)
	{
		if (p < NUM_BANK0_GPIOS)
		{
			pin = p;
			pinMask = 1 << p;
		}
		else
		{
			pin = 0xff;
			pinMask = 0;
		}
	}

	bool isAssigned() const { return pin != 0xff; }
};

#define GAMEPAD_DIGITAL_INPUT_COUNT 36 // Total number of buttons, including D-pad

class Gamepad {
public:
	Gamepad(int debounceMS = 5, GamepadStorage *storage = &GamepadStore) :
			debounceMS(debounceMS)
			, f1Mask((GAMEPAD_MASK_S1 | GAMEPAD_MASK_S2))
			, f2Mask((GAMEPAD_MASK_L3 | GAMEPAD_MASK_R3))
			, debouncer(debounceMS)
			, mpgStorage(storage)
	{}

	void setup();
	void process();
	void read();
	void save();
	void debounce();
	
	GamepadHotkey hotkey();

	/**
	 * @brief Flag to indicate analog trigger support.
	 */
	bool hasAnalogTriggers {false};

	/**
	 * @brief Flag to indicate Left analog stick support.
	 */
	bool hasLeftAnalogStick {false};

	/**
	 * @brief Flag to indicate Right analog stick support.
	 */
	bool hasRightAnalogStick {false};

	void *getReport();
	uint16_t getReportSize();
	HIDReport *getHIDReport();
	SwitchReport *getSwitchReport();
	XInputReport *getXInputReport();
	KeyboardReport *getKeyboardReport();
	PS4Report *getPS4Report();

	/**
	 * @brief Check for a button press. Used by `pressedP1[Button]` helper methods.
	 */
	inline bool __attribute__((always_inline)) pressedP1Button(const uint16_t mask) {
		return (p1State.buttons & mask) == mask;
	}

	/**
	 * @brief Check for a button press. Used by `pressedP2[Button]` helper methods.
	 */
	inline bool __attribute__((always_inline)) pressedP2Button(const uint16_t mask) {
		return (p2State.buttons & mask) == mask;
	}

	/**
	 * @brief Check for a dpad press. Used by `pressedP1[Dpad]` helper methods.
	 */
	inline bool __attribute__((always_inline)) pressedP1Dpad(const uint8_t mask) { return (p1State.dpad & mask) == mask; }

	/**
	 * @brief Check for a dpad press. Used by `pressedP2[Dpad]` helper methods.
	 */
	inline bool __attribute__((always_inline)) pressedP2Dpad(const uint8_t mask) { return (p2State.dpad & mask) == mask; }

	inline bool __attribute__((always_inline)) pressedP1Up()    { return pressedP1Dpad(GAMEPAD_MASK_UP); }
	inline bool __attribute__((always_inline)) pressedP1Down()  { return pressedP1Dpad(GAMEPAD_MASK_DOWN); }
	inline bool __attribute__((always_inline)) pressedP1Left()  { return pressedP1Dpad(GAMEPAD_MASK_LEFT); }
	inline bool __attribute__((always_inline)) pressedP1Right() { return pressedP1Dpad(GAMEPAD_MASK_RIGHT); }
	inline bool __attribute__((always_inline)) pressedP1B1()    { return pressedP1Button(GAMEPAD_MASK_B1); }
	inline bool __attribute__((always_inline)) pressedP1B2()    { return pressedP1Button(GAMEPAD_MASK_B2); }
	inline bool __attribute__((always_inline)) pressedP1B3()    { return pressedP1Button(GAMEPAD_MASK_B3); }
	inline bool __attribute__((always_inline)) pressedP1B4()    { return pressedP1Button(GAMEPAD_MASK_B4); }
	inline bool __attribute__((always_inline)) pressedP1L1()    { return pressedP1Button(GAMEPAD_MASK_L1); }
	inline bool __attribute__((always_inline)) pressedP1R1()    { return pressedP1Button(GAMEPAD_MASK_R1); }
	inline bool __attribute__((always_inline)) pressedP1L2()    { return pressedP1Button(GAMEPAD_MASK_L2); }
	inline bool __attribute__((always_inline)) pressedP1R2()    { return pressedP1Button(GAMEPAD_MASK_R2); }
	inline bool __attribute__((always_inline)) pressedP1S1()    { return pressedP1Button(GAMEPAD_MASK_S1); }
	inline bool __attribute__((always_inline)) pressedP1S2()    { return pressedP1Button(GAMEPAD_MASK_S2); }
	inline bool __attribute__((always_inline)) pressedP1L3()    { return pressedP1Button(GAMEPAD_MASK_L3); }
	inline bool __attribute__((always_inline)) pressedP1R3()    { return pressedP1Button(GAMEPAD_MASK_R3); }
	inline bool __attribute__((always_inline)) pressedP1A1()    { return pressedP1Button(GAMEPAD_MASK_A1); }
	inline bool __attribute__((always_inline)) pressedP1A2()    { return pressedP1Button(GAMEPAD_MASK_A2); }

	inline bool __attribute__((always_inline)) pressedP2Up()    { return pressedP2Dpad(GAMEPAD_MASK_UP); }
	inline bool __attribute__((always_inline)) pressedP2Down()  { return pressedP2Dpad(GAMEPAD_MASK_DOWN); }
	inline bool __attribute__((always_inline)) pressedP2Left()  { return pressedP2Dpad(GAMEPAD_MASK_LEFT); }
	inline bool __attribute__((always_inline)) pressedP2Right() { return pressedP2Dpad(GAMEPAD_MASK_RIGHT); }
	inline bool __attribute__((always_inline)) pressedP2B1()    { return pressedP2Button(GAMEPAD_MASK_B1); }
	inline bool __attribute__((always_inline)) pressedP2B2()    { return pressedP2Button(GAMEPAD_MASK_B2); }
	inline bool __attribute__((always_inline)) pressedP2B3()    { return pressedP2Button(GAMEPAD_MASK_B3); }
	inline bool __attribute__((always_inline)) pressedP2B4()    { return pressedP2Button(GAMEPAD_MASK_B4); }
	inline bool __attribute__((always_inline)) pressedP2L1()    { return pressedP2Button(GAMEPAD_MASK_L1); }
	inline bool __attribute__((always_inline)) pressedP2R1()    { return pressedP2Button(GAMEPAD_MASK_R1); }
	inline bool __attribute__((always_inline)) pressedP2L2()    { return pressedP2Button(GAMEPAD_MASK_L2); }
	inline bool __attribute__((always_inline)) pressedP2R2()    { return pressedP2Button(GAMEPAD_MASK_R2); }
	inline bool __attribute__((always_inline)) pressedP2S1()    { return pressedP2Button(GAMEPAD_MASK_S1); }
	inline bool __attribute__((always_inline)) pressedP2S2()    { return pressedP2Button(GAMEPAD_MASK_S2); }
	inline bool __attribute__((always_inline)) pressedP2L3()    { return pressedP2Button(GAMEPAD_MASK_L3); }
	inline bool __attribute__((always_inline)) pressedP2R3()    { return pressedP2Button(GAMEPAD_MASK_R3); }
	inline bool __attribute__((always_inline)) pressedP2A1()    { return pressedP2Button(GAMEPAD_MASK_A1); }
	inline bool __attribute__((always_inline)) pressedP2A2()    { return pressedP2Button(GAMEPAD_MASK_A2); }

	inline bool __attribute__((always_inline)) pressedF1()    { return pressedP1Button(f1Mask); }
	inline bool __attribute__((always_inline)) pressedF2()    { return pressedP1Button(f2Mask); }

	GamepadDebouncer debouncer;
	GamepadStorage *mpgStorage;
	const uint8_t debounceMS;
	uint16_t f1Mask;
	uint16_t f2Mask;
	GamepadOptions options;
	GamepadState rawState;
	GamepadState p1State;
	GamepadState p2State;
	GamepadButtonMapping *mapP1DpadUp;
	GamepadButtonMapping *mapP1DpadDown;
	GamepadButtonMapping *mapP1DpadLeft;
	GamepadButtonMapping *mapP1DpadRight;
	GamepadButtonMapping *mapP1ButtonB1;
	GamepadButtonMapping *mapP1ButtonB2;
	GamepadButtonMapping *mapP1ButtonB3;
	GamepadButtonMapping *mapP1ButtonB4;
	GamepadButtonMapping *mapP1ButtonL1;
	GamepadButtonMapping *mapP1ButtonR1;
	GamepadButtonMapping *mapP1ButtonL2;
	GamepadButtonMapping *mapP1ButtonR2;
	GamepadButtonMapping *mapP1ButtonS1;
	GamepadButtonMapping *mapP1ButtonS2;
	GamepadButtonMapping *mapP1ButtonL3;
	GamepadButtonMapping *mapP1ButtonR3;
	GamepadButtonMapping *mapP1ButtonA1;
	GamepadButtonMapping *mapP1ButtonA2;
	GamepadButtonMapping *mapP2DpadUp;
	GamepadButtonMapping *mapP2DpadDown;
	GamepadButtonMapping *mapP2DpadLeft;
	GamepadButtonMapping *mapP2DpadRight;
	GamepadButtonMapping *mapP2ButtonB1;
	GamepadButtonMapping *mapP2ButtonB2;
	GamepadButtonMapping *mapP2ButtonB3;
	GamepadButtonMapping *mapP2ButtonB4;
	GamepadButtonMapping *mapP2ButtonL1;
	GamepadButtonMapping *mapP2ButtonR1;
	GamepadButtonMapping *mapP2ButtonL2;
	GamepadButtonMapping *mapP2ButtonR2;
	GamepadButtonMapping *mapP2ButtonS1;
	GamepadButtonMapping *mapP2ButtonS2;
	GamepadButtonMapping *mapP2ButtonL3;
	GamepadButtonMapping *mapP2ButtonR3;
	GamepadButtonMapping *mapP2ButtonA1;
	GamepadButtonMapping *mapP2ButtonA2;
	GamepadButtonMapping **gamepadMappings;

	inline static const SOCDMode resolveSOCDMode(const GamepadOptions& options) {
		 return ((options.socdMode == SOCD_MODE_BYPASS) && 
		         (options.inputMode == INPUT_MODE_HID || options.inputMode == INPUT_MODE_SWITCH || options.inputMode == INPUT_MODE_PS4)) ?
			    SOCD_MODE_NEUTRAL : options.socdMode;
	};

private:
	void releaseAllKeys(void);
	void pressKey(uint8_t code);
	uint8_t getModifier(uint8_t code);

	GamepadHotkeyEntry hotkeyF1Up;
	GamepadHotkeyEntry hotkeyF1Down;
	GamepadHotkeyEntry hotkeyF1Left;
	GamepadHotkeyEntry hotkeyF1Right;
	GamepadHotkeyEntry hotkeyF2Up;
	GamepadHotkeyEntry hotkeyF2Down;
	GamepadHotkeyEntry hotkeyF2Left;
	GamepadHotkeyEntry hotkeyF2Right;
};

#endif
