#include "addons/i2cioexpander.h"
#include "storagemanager.h"

bool I2CIOExpanderAddon::available() {
    BoardOptions boardOptions = Storage::getInstance().getBoardOptions();
	return (boardOptions.i2cIOExpanderSDAPin != (uint8_t)-1 &&
        boardOptions.i2cIOExpanderSCLPin != (uint8_t)-1 &&
        boardOptions.i2cIOExpanderINTPin != (uint8_t)-1);
}

void I2CIOExpanderAddon::setup() {
    BoardOptions boardOptions = Storage::getInstance().getBoardOptions();
    pca.attach(1,
        boardOptions.i2cIOExpanderSDAPin,
        boardOptions.i2cIOExpanderSCLPin,
        boardOptions.i2cIOExpanderBlock == 0 ? i2c0 : i2c1,
        boardOptions.i2cIOExpanderSpeed,
        boardOptions.i2cIOExpanderAddress);
    // pca.polarity(PCA95xx::Polarity::ORIGINAL_ALL);
    // pca.direction(PCA95xx::Direction::IN_ALL);

    gpio_init(boardOptions.i2cIOExpanderINTPin);             // Initialize pin
    gpio_set_dir(boardOptions.i2cIOExpanderINTPin, GPIO_IN); // Set as INPUT
    gpio_pull_up(boardOptions.i2cIOExpanderINTPin);          // Set as PULLUP

    pinMask = 0;
}

void I2CIOExpanderAddon::preprocess() {
    BoardOptions boardOptions = Storage::getInstance().getBoardOptions();
    Gamepad * gamepad = Storage::getInstance().GetGamepad();

    // Is our interrupt ready (Read for next frame (delay))
    //if ( !gpio_get(boardOptions.i2cIOExpanderINTPin)) {
    pinMask = pca.read();
    //}


    // If the I/O pin -> button is defined, read it from the mask and add to our gamepad state
    if ( pinMask != 0 ) {
        for(uint8_t i = 0; i < 7; i++) {
            if ( boardOptions.i2cIOExpanderPins[i] != 0 && pinMask & (1<<i)) {
                if ( boardOptions.i2cIOExpanderPins[i] & GAMEPAD_MASK_DPAD )
                    gamepad->state.dpad |= boardOptions.i2cIOExpanderPins[i];
                else
                    gamepad->state.buttons |= boardOptions.i2cIOExpanderPins[i];
            }
        }
    }
}

void I2CIOExpanderAddon::process() {
}
