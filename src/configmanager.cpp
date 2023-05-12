#include "configmanager.h"

#include "addonmanager.h"
#include "configs/webconfig.h"
#include "addons/neopicoleds.h"

void ConfigManager::setup(ConfigType config) {
	switch(config) {
		case CONFIG_TYPE_WEB:
			setupConfig(new WebConfig());
			break;
	}
    this->cType = config;
}

void ConfigManager::loop() {
    config->loop();
}

void ConfigManager::setupConfig(GPConfig * gpconfig) {
    gpconfig->setup();
    this->config = gpconfig;
}

void ConfigManager::setGamepadOptions(Gamepad* gamepad) {
	gamepad->save();
}

void ConfigManager::setLedOptions(LEDOptions ledOptions) {
	Storage::getInstance().setLEDOptions(ledOptions);
}

void ConfigManager::setBoardOptions(BoardOptions boardOptions) {
	Storage::getInstance().setBoardOptions(boardOptions);

	Gamepad * gamepad = Storage::getInstance().GetGamepad();
	gamepad->mapP1DpadUp->setPin(boardOptions.pinP1DpadUp);
	gamepad->mapP1DpadDown->setPin(boardOptions.pinP1DpadDown);
	gamepad->mapP1DpadLeft->setPin(boardOptions.pinP1DpadLeft);
	gamepad->mapP1DpadRight->setPin(boardOptions.pinP1DpadRight);
	gamepad->mapP1ButtonB1->setPin(boardOptions.pinP1ButtonB1);
	gamepad->mapP1ButtonB2->setPin(boardOptions.pinP1ButtonB2);
	gamepad->mapP1ButtonB3->setPin(boardOptions.pinP1ButtonB3);
	gamepad->mapP1ButtonB4->setPin(boardOptions.pinP1ButtonB4);
	gamepad->mapP1ButtonL1->setPin(boardOptions.pinP1ButtonL1);
	gamepad->mapP1ButtonR1->setPin(boardOptions.pinP1ButtonR1);
	gamepad->mapP1ButtonL2->setPin(boardOptions.pinP1ButtonL2);
	gamepad->mapP1ButtonR2->setPin(boardOptions.pinP1ButtonR2);
	gamepad->mapP1ButtonS1->setPin(boardOptions.pinP1ButtonS1);
	gamepad->mapP1ButtonS2->setPin(boardOptions.pinP1ButtonS2);
	gamepad->mapP1ButtonL3->setPin(boardOptions.pinP1ButtonL3);
	gamepad->mapP1ButtonR3->setPin(boardOptions.pinP1ButtonR3);
	gamepad->mapP1ButtonA1->setPin(boardOptions.pinP1ButtonA1);
	gamepad->mapP1ButtonA2->setPin(boardOptions.pinP1ButtonA2);

	gamepad->mapP2DpadUp->setPin(boardOptions.pinP2DpadUp);
	gamepad->mapP2DpadDown->setPin(boardOptions.pinP2DpadDown);
	gamepad->mapP2DpadLeft->setPin(boardOptions.pinP2DpadLeft);
	gamepad->mapP2DpadRight->setPin(boardOptions.pinP2DpadRight);
	gamepad->mapP2ButtonB1->setPin(boardOptions.pinP2ButtonB1);
	gamepad->mapP2ButtonB2->setPin(boardOptions.pinP2ButtonB2);
	gamepad->mapP2ButtonB3->setPin(boardOptions.pinP2ButtonB3);
	gamepad->mapP2ButtonB4->setPin(boardOptions.pinP2ButtonB4);
	gamepad->mapP2ButtonL1->setPin(boardOptions.pinP2ButtonL1);
	gamepad->mapP2ButtonR1->setPin(boardOptions.pinP2ButtonR1);
	gamepad->mapP2ButtonL2->setPin(boardOptions.pinP2ButtonL2);
	gamepad->mapP2ButtonR2->setPin(boardOptions.pinP2ButtonR2);
	gamepad->mapP2ButtonS1->setPin(boardOptions.pinP2ButtonS1);
	gamepad->mapP2ButtonS2->setPin(boardOptions.pinP2ButtonS2);
	gamepad->mapP2ButtonL3->setPin(boardOptions.pinP2ButtonL3);
	gamepad->mapP2ButtonR3->setPin(boardOptions.pinP2ButtonR3);
	gamepad->mapP2ButtonA1->setPin(boardOptions.pinP2ButtonA1);
	gamepad->mapP2ButtonA2->setPin(boardOptions.pinP2ButtonA2);

	GamepadStore.save();
}

void ConfigManager::setPreviewBoardOptions(BoardOptions boardOptions) {
	Storage::getInstance().setPreviewBoardOptions(boardOptions);
}

void ConfigManager::setSplashImage(const SplashImage& image) {
	Storage::getInstance().setSplashImage(image);
}
