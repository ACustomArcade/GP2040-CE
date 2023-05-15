#ifndef _i2cioexpander_H
#define _i2cioexpander_H

#include "gpaddon.h"

#include "PCA95xx.h"

#ifndef I2C_IOEXPANDER_SDA_PIN
#define I2C_IOEXPANDER_SDA_PIN    -1
#endif

#ifndef I2C_IOEXPANDER_SCL_PIN
#define I2C_IOEXPANDER_SCL_PIN    -1
#endif

#ifndef I2C_IOEXPANDER_INT_PIN
#define I2C_IOEXPANDER_INT_PIN -1
#endif

#ifndef I2C_IOEXPANDER_BLOCK
#define I2C_IOEXPANDER_BLOCK i2c0
#endif

#ifndef I2C_IOEXPANDER_SPEED
#define I2C_IOEXPANDER_SPEED 400000
#endif

#ifndef I2C_IOEXPANDER_ADDRESS
#define I2C_IOEXPANDER_ADDRESS 0x20
#endif

// PCA95x5 uses Pins 0-7, 10-17
#ifndef I2C_IOEXPANDER_PIN_0
#define I2C_IOEXPANDER_PIN_0 0
#endif

#ifndef I2C_IOEXPANDER_PIN_1
#define I2C_IOEXPANDER_PIN_1 0
#endif

#ifndef I2C_IOEXPANDER_PIN_2
#define I2C_IOEXPANDER_PIN_2 0
#endif

#ifndef I2C_IOEXPANDER_PIN_3
#define I2C_IOEXPANDER_PIN_3 0
#endif

#ifndef I2C_IOEXPANDER_PIN_4
#define I2C_IOEXPANDER_PIN_4 0
#endif

#ifndef I2C_IOEXPANDER_PIN_5
#define I2C_IOEXPANDER_PIN_5 0
#endif

#ifndef I2C_IOEXPANDER_PIN_6
#define I2C_IOEXPANDER_PIN_6 0
#endif

#ifndef I2C_IOEXPANDER_PIN_7
#define I2C_IOEXPANDER_PIN_7 0
#endif

// PCA95x5 uses Pins 0-7, 10-17
#ifndef I2C_IOEXPANDER_PIN_10
#define I2C_IOEXPANDER_PIN_10 0
#endif

#ifndef I2C_IOEXPANDER_PIN_11
#define I2C_IOEXPANDER_PIN_11 0
#endif

#ifndef I2C_IOEXPANDER_PIN_12
#define I2C_IOEXPANDER_PIN_12 0
#endif

#ifndef I2C_IOEXPANDER_PIN_13
#define I2C_IOEXPANDER_PIN_13 0
#endif

#ifndef I2C_IOEXPANDER_PIN_14
#define I2C_IOEXPANDER_PIN_14 0
#endif

#ifndef I2C_IOEXPANDER_PIN_15
#define I2C_IOEXPANDER_PIN_15 0
#endif

#ifndef I2C_IOEXPANDER_PIN_16
#define I2C_IOEXPANDER_PIN_16 0
#endif

#ifndef I2C_IOEXPANDER_PIN_17
#define I2C_IOEXPANDER_PIN_17 0
#endif

// Analog Module Name
#define I2CIOExpanderName "I2CIOExpander"

class I2CIOExpanderAddon : public GPAddon {
public:
	virtual bool available();   // GPAddon available
	virtual void setup();       // I/O Expander Setup
	virtual void process();     // I/O Expander Process
    virtual void preprocess();  // I/O Expander Pre-Process
    virtual std::string name() { return I2CIOExpanderName; }
private:
    PCA95xx::PCA95xx pca;   // PCA95x5 expander
    uint16_t pinMask;       // Pin Mask for I/O Expander
};

#endif  // _IOExpander_H