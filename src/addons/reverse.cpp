#include "addons/reverse.h"
#include "storagemanager.h"
#include "GamepadEnums.h"

bool ReverseInput::available() {
    const AddonOptions& options = Storage::getInstance().getAddonOptions();
    pinButtonReverse = options.pinButtonReverse;
	return (options.ReverseInputEnabled &&
        pinButtonReverse != (uint8_t)-1);
}

void ReverseInput::setup()
{
    // Setup Reverse Input Button
    gpio_init(pinButtonReverse);             // Initialize pin
    gpio_set_dir(pinButtonReverse, GPIO_IN); // Set as INPUT
    gpio_pull_up(pinButtonReverse);          // Set as PULLUP

    // Setup Reverse LED if available
    const AddonOptions& options = Storage::getInstance().getAddonOptions();
    pinLED = options.pinReverseLED;
    if (pinLED != (uint8_t)-1) {
        gpio_init(options.pinReverseLED);
        gpio_set_dir(options.pinReverseLED, GPIO_OUT);
        gpio_put(options.pinReverseLED, 1);
    }

    actionUp = options.reverseActionUp;
    actionDown = options.reverseActionDown;
    actionLeft = options.reverseActionLeft;
    actionRight = options.reverseActionDown;

    Gamepad * gamepad = Storage::getInstance().GetGamepad();
	mapP1DpadUp    = gamepad->mapP1DpadUp;
	mapP1DpadDown  = gamepad->mapP1DpadDown;
	mapP1DpadLeft  = gamepad->mapP1DpadLeft;
	mapP1DpadRight = gamepad->mapP1DpadRight;
    mapP2DpadUp    = gamepad->mapP2DpadUp;
	mapP2DpadDown  = gamepad->mapP2DpadDown;
	mapP2DpadLeft  = gamepad->mapP2DpadLeft;
	mapP2DpadRight = gamepad->mapP2DpadRight;

    invertXAxis = gamepad->options.invertXAxis;
    invertYAxis = gamepad->options.invertYAxis;

    state = false;
}

void ReverseInput::update() {
    state = !gpio_get(pinButtonReverse);
}

uint8_t ReverseInput::input(uint8_t valueMask, uint16_t buttonMask, uint16_t buttonMaskReverse, uint8_t action, bool invertAxis) {
    if (state && action == 2) {
        return 0;
    }
    bool invert = (state && action == 1) ? !invertAxis : invertAxis;
    return (valueMask ? (invert ? buttonMaskReverse : buttonMask) : 0);
}

void ReverseInput::process()
{
    // Update Reverse State
    update();

    uint32_t values = ~gpio_get_all();
    Gamepad * gamepad = Storage::getInstance().GetGamepad();

    gamepad->p1State.dpad = 0
        | input(values & mapP1DpadUp->pinMask,    mapP1DpadUp->buttonMask,      mapP1DpadDown->buttonMask,    actionUp,       invertYAxis)
        | input(values & mapP1DpadDown->pinMask,  mapP1DpadDown->buttonMask,    mapP1DpadUp->buttonMask,      actionDown,     invertYAxis)
        | input(values & mapP1DpadLeft->pinMask,  mapP1DpadLeft->buttonMask,    mapP1DpadRight->buttonMask,   actionLeft,     invertXAxis)
        | input(values & mapP1DpadRight->pinMask, mapP1DpadRight->buttonMask,   mapP1DpadLeft->buttonMask,    actionRight,    invertXAxis)
    ;

    gamepad->p2State.dpad = 0
        | input(values & mapP2DpadUp->pinMask,    mapP2DpadUp->buttonMask,      mapP2DpadDown->buttonMask,    actionUp,       invertYAxis)
        | input(values & mapP2DpadDown->pinMask,  mapP2DpadDown->buttonMask,    mapP2DpadUp->buttonMask,      actionDown,     invertYAxis)
        | input(values & mapP2DpadLeft->pinMask,  mapP2DpadLeft->buttonMask,    mapP2DpadRight->buttonMask,   actionLeft,     invertXAxis)
        | input(values & mapP2DpadRight->pinMask, mapP2DpadRight->buttonMask,   mapP2DpadLeft->buttonMask,    actionRight,    invertXAxis)
    ;

    if (pinLED != (uint8_t)-1) {
        gpio_put(pinLED, !state);
    }
}
