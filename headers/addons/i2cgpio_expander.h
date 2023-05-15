#ifndef _I2CGPIOExpander_H
#define _I2CGPIOExpander_H

#include <hardware/i2c.h>
#include "BoardConfig.h"
#include "gpaddon.h"
#include "gamepad.h"

#include "GamepadEnums.h"

#ifndef I2C_GPIOEXPANDER_ENABLED
#define I2C_GPIOEXPANDER_ENABLED 0
#endif

#ifndef I2C_GPIOEXPANDER_SDA_PIN
#define I2C_GPIOEXPANDER_SDA_PIN    -1
#endif

#ifndef I2C_GPIOEXPANDER_SCL_PIN
#define I2C_GPIOEXPANDER_SCL_PIN    -1
#endif

#ifndef I2C_GPIOEXPANDER_INT_PIN
#define I2C_GPIOEXPANDER_INT_PIN    -1
#endif

#ifndef I2C_GPIOEXPANDER_NUM_PINS
#define I2C_GPIOEXPANDER_NUM_PINS   16
#endif

#ifndef I2C_GPIOEXPANDER_BLOCK
#define I2C_GPIOEXPANDER_BLOCK i2c0
#endif

#ifndef I2C_GPIOEXPANDER_SPEED
#define I2C_GPIOEXPANDER_SPEED 400000
#endif

#ifndef I2C_GPIOEXPANDER_ADDRESS
#define I2C_GPIOEXPANDER_ADDRESS 0x20
#endif

// Analog Module Name
#define I2CGPIOExpanderName "I2CGPIOExpander"

typedef struct {
	float PINS[I2C_GPIOEXPANDER_NUM_PINS];
} GPIOEXP_PINS;

class I2CGPIOExpander : public GPAddon {
public:
	virtual bool available();
	virtual void setup();       // GPIOExpander Setup
	virtual void preprocess() {}
	virtual void process();     // GPIOExpander Process
    virtual std::string name() { return I2CGPIOExpanderName; }
private:
	bool checkInterrupt();
	void getPins();
	void initI2C();
	GPIOEXP_PINS pins;
	int I2CGPIOExpanderBlock;
	uint8_t I2CGPIOExpanderSDAPin;
    uint8_t I2CGPIOExpanderSCLPin;
	uint8_t I2CGPIOExpanderIntPin;
	uint8_t I2CGPIOExpanderNumPins;

	uint8_t cmdInputPort0 = 0x00;
	uint8_t cmdInputPort1 = 0x01;
	uint8_t cmdOutputPort0 = 0x02;
	uint8_t cmdOutputPort1 = 0x03;
	uint8_t cmdPolarityPort0 = 0x04;
	uint8_t cmdPolarityPort1 = 0x05;
	uint8_t cmdConfigPort0 = 0x06;
	uint8_t cmdConfigPort1 = 0x07;
	
	uint8_t addrRead = I2C_GPIOEXPANDER_ADDRESS & 0xFE;
	uint8_t addrWrite = I2C_GPIOEXPANDER_ADDRESS | 0x01;


};

#endif  // _I2CGPIOExpander_H
