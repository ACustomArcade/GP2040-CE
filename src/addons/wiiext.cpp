#include "addons/wiiext.h"
#include "storagemanager.h"
#include "hardware/gpio.h"

bool WiiExtensionInput::available() {
    const BoardOptions& boardOptions = Storage::getInstance().getBoardOptions();
    AddonOptions options = Storage::getInstance().getAddonOptions();

    return (!boardOptions.hasI2CDisplay && (options.WiiExtensionAddonEnabled &&
        options.wiiExtensionSDAPin != (uint8_t)-1 &&
        options.wiiExtensionSCLPin != (uint8_t)-1));
}

void WiiExtensionInput::setup() {
    AddonOptions options = Storage::getInstance().getAddonOptions();
    nextTimer = getMillis();

#if WII_EXTENSION_DEBUG==true
    stdio_init_all();
#endif

    uIntervalMS = 0;
    
    wii = new WiiExtension(
        options.wiiExtensionSDAPin,
        options.wiiExtensionSCLPin,
        options.wiiExtensionBlock == 0 ? i2c0 : i2c1,
        options.wiiExtensionSpeed,
        WII_EXTENSION_I2C_ADDR);
    wii->begin();
    wii->start();
}

void WiiExtensionInput::process() {
    if (nextTimer < getMillis()) {
        wii->poll();
        
        if (wii->extensionType == WII_EXTENSION_NUNCHUCK) {
            buttonZ = wii->buttonZ;
            buttonC = wii->buttonC;

            leftX = map(wii->joy1X,0,1023,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
            leftY = map(wii->joy1Y,1023,0,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
            rightX = GAMEPAD_JOYSTICK_MID;
            rightY = GAMEPAD_JOYSTICK_MID;

            triggerLeft = 0;
            triggerRight = 0;
        } else if ((wii->extensionType == WII_EXTENSION_CLASSIC) || (wii->extensionType == WII_EXTENSION_CLASSIC_PRO)) {
            buttonA = wii->buttonA;
            buttonB = wii->buttonB;
            buttonX = wii->buttonX;
            buttonY = wii->buttonY;
            buttonL = wii->buttonZL;
            buttonZL = wii->buttonLT;
            buttonR = wii->buttonZR;
            buttonZR = wii->buttonRT;
            dpadUp = wii->directionUp;
            dpadDown = wii->directionDown;
            dpadLeft = wii->directionLeft;
            dpadRight = wii->directionRight;
            buttonSelect = wii->buttonMinus;
            buttonStart = wii->buttonPlus;
            buttonHome = wii->buttonHome;

            if (wii->extensionType == WII_EXTENSION_CLASSIC) {
                triggerLeft  = wii->triggerLeft;
                triggerRight = wii->triggerRight;
            }

            leftX = map(wii->joy1X,0,WII_ANALOG_PRECISION_3,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
            leftY = map(wii->joy1Y,WII_ANALOG_PRECISION_3,0,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
            rightX = map(wii->joy2X,0,WII_ANALOG_PRECISION_3,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
            rightY = map(wii->joy2Y,WII_ANALOG_PRECISION_3,0,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
        } else if (wii->extensionType == WII_EXTENSION_GUITAR) {
            buttonSelect = wii->buttonMinus;
            buttonStart = wii->buttonPlus;

            dpadUp = wii->directionUp;
            dpadDown = wii->directionDown;

            buttonB = wii->fretGreen;
            buttonA = wii->fretRed;
            buttonX = wii->fretYellow;
            buttonY = wii->fretBlue;
            buttonL = wii->fretOrange;

            // whammy currently maps to Joy2X in addition to the raw whammy value
            whammyBar = wii->whammyBar;

            leftX = map(wii->joy1X,0,WII_ANALOG_PRECISION_3,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
            leftY = map(wii->joy1Y,WII_ANALOG_PRECISION_3,0,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
            rightX = map(wii->joy2X,0,WII_ANALOG_PRECISION_3,GAMEPAD_JOYSTICK_MID,GAMEPAD_JOYSTICK_MAX);
            rightY = GAMEPAD_JOYSTICK_MID;

            triggerLeft = 0;
            triggerRight = 0;
        } else if (wii->extensionType == WII_EXTENSION_TAIKO) {
            buttonL = wii->rimLeft;
            buttonR = wii->rimRight;

            dpadRight = wii->drumLeft;
            buttonA = wii->drumRight;
        }
               
        nextTimer = getMillis() + uIntervalMS;
    }

    Gamepad * gamepad = Storage::getInstance().GetGamepad();

    gamepad->p1State.lx = leftX;
    gamepad->p1State.ly = leftY;
    gamepad->p1State.rx = rightX;
    gamepad->p1State.ry = rightY;

    if (wii->extensionType == WII_EXTENSION_CLASSIC) {
        gamepad->hasAnalogTriggers = true;
        gamepad->p1State.lt = triggerLeft;
        gamepad->p1State.rt = triggerRight;
    } else {
        gamepad->hasAnalogTriggers = false;
    }

    if (buttonC) gamepad->p1State.buttons |= GAMEPAD_MASK_B1;
    if (buttonZ) gamepad->p1State.buttons |= GAMEPAD_MASK_B2;

    if (buttonA) gamepad->p1State.buttons |= GAMEPAD_MASK_B2;
    if (buttonB) gamepad->p1State.buttons |= GAMEPAD_MASK_B1;
    if (buttonX) gamepad->p1State.buttons |= GAMEPAD_MASK_B4;
    if (buttonY) gamepad->p1State.buttons |= GAMEPAD_MASK_B3;
    if (buttonL) gamepad->p1State.buttons |= GAMEPAD_MASK_L1;
    if (buttonZL) gamepad->p1State.buttons |= GAMEPAD_MASK_L2;
    if (buttonR) gamepad->p1State.buttons |= GAMEPAD_MASK_R1;
    if (buttonZR) gamepad->p1State.buttons |= GAMEPAD_MASK_R2;
    if (buttonSelect) gamepad->p1State.buttons |= GAMEPAD_MASK_S1;
    if (buttonStart) gamepad->p1State.buttons |= GAMEPAD_MASK_S2;
    if (buttonHome) gamepad->p1State.buttons |= GAMEPAD_MASK_A1;
    if (dpadUp) gamepad->p1State.dpad |= GAMEPAD_MASK_UP;
    if (dpadDown) gamepad->p1State.dpad |= GAMEPAD_MASK_DOWN;
    if (dpadLeft) gamepad->p1State.dpad |= GAMEPAD_MASK_LEFT;
    if (dpadRight) gamepad->p1State.dpad |= GAMEPAD_MASK_RIGHT;
}

uint16_t WiiExtensionInput::map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
