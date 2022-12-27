#include "src/networking/networking.hpp"
#include "src/hardware/hardware.hpp"

void setup()
{
	Serial.begin(115200);
	cst::begin_SD();
	cst::load_cfg();
	cst::begin_network();
	cst::begin_server();
}

void loop() {}