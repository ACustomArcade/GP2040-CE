#include "addons/i2cgpio_expander.h"
#include "storagemanager.h"
#include "hardware/gpio.h"


void I2CGPIOExpander::initI2C() {
    // Configure I2C
    i2c_init(i2c0, 400000);
    gpio_set_function(I2CGPIOExpanderSDAPin, GPIO_FUNC_I2C);
    gpio_set_function(I2CGPIOExpanderSCLPin, GPIO_FUNC_I2C);
    gpio_pull_up(I2CGPIOExpanderSDAPin);
    gpio_pull_up(I2CGPIOExpanderSCLPin);
}


void I2CGPIOExpander::getPins() {
    int ret;
    uint8_t buf[2];
    //i2c_write_blocking(I2C_GPIOEXPANDER_BLOCK, I2C_GPIOEXPANDER_ADDRESS, &addrRead, 1, true); // true to keep master control of bus
    //i2c_write_blocking(I2C_GPIOEXPANDER_BLOCK, I2C_GPIOEXPANDER_ADDRESS, &cmdConfigPort0, 1, true); // true to keep master control of bus
    //i2c_write_blocking(I2C_GPIOEXPANDER_BLOCK, I2C_GPIOEXPANDER_ADDRESS, &addrRead, 1, true); // true to keep master control of bus
    ret = i2c_write_blocking(i2c0, 0x20, &cmdInputPort0, 1, true);
    if (ret == PICO_ERROR_GENERIC) {
        printf("Problem writing to PCA9555: %d", ret);
    } else {
        i2c_read_blocking(i2c0, 0x20, buf, 2, false);
        printf("Read pins: %02x:%02x\n", buf[0], buf[1]);
    }
}


bool I2CGPIOExpander::available() {
    const BoardOptions& boardOptions = Storage::getInstance().getBoardOptions();
    const AddonOptions& addonOptions = Storage::getInstance().getAddonOptions();

    return ((boardOptions.I2CGPIOExpanderEnabled &&
             boardOptions.I2CGPIOExpanderBlock != (uint8_t)-1 &&
             boardOptions.I2CGPIOExpanderSDAPin != (uint8_t)-1 &&
             boardOptions.I2CGPIOExpanderSCLPin != (uint8_t)-1 &&
             boardOptions.I2CGPIOExpanderIntPin != (uint8_t)-1 &&
             boardOptions.I2CGPIOExpanderNumPins != (uint8_t)-1) ||
            (addonOptions.I2CGPIOExpanderEnabled &&
             addonOptions.I2CGPIOExpanderBlock != (uint8_t)-1 &&
             addonOptions.I2CGPIOExpanderSDAPin != (uint8_t)-1 &&
             addonOptions.I2CGPIOExpanderSCLPin != (uint8_t)-1 &&
             addonOptions.I2CGPIOExpanderIntPin != (uint8_t)-1 &&
             addonOptions.I2CGPIOExpanderNumPins != (uint8_t)-1));
}


void I2CGPIOExpander::setup() {
    const AddonOptions& addonOptions = Storage::getInstance().getAddonOptions();
    memset(&pins, 0, sizeof(GPIOEXP_PINS));

	I2CGPIOExpanderBlock = addonOptions.I2CGPIOExpanderBlock;
	I2CGPIOExpanderSDAPin = addonOptions.I2CGPIOExpanderSDAPin;
    I2CGPIOExpanderSCLPin = addonOptions.I2CGPIOExpanderSCLPin;
	I2CGPIOExpanderIntPin = addonOptions.I2CGPIOExpanderIntPin;
	I2CGPIOExpanderNumPins = addonOptions.I2CGPIOExpanderNumPins;

    initI2C();
    
    // Configure Interrupt pin
    gpio_init(I2CGPIOExpanderIntPin);             // Initialize pin
    gpio_set_dir(I2CGPIOExpanderIntPin, GPIO_IN); // Set as INPUT
    gpio_pull_up(I2CGPIOExpanderIntPin);          // Set as PULLUP
}


bool I2CGPIOExpander::checkInterrupt() {
    Gamepad * gamepad = Storage::getInstance().GetGamepad();
    bool hasInput;
    hasInput = gpio_get(I2CGPIOExpanderIntPin);
    if (!hasInput) {
        getPins();
        gamepad->p1State.buttons |= GAMEPAD_MASK_S2;
    }
    return false;
}


void I2CGPIOExpander::process() {
    checkInterrupt();
}