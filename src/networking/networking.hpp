#pragma once

#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include "../../src/file_utils/file_utils.hpp"
#include "../../src/ESP32SessionManager/ESPSessionManager.hpp"
#include "../../src/hardware/hardware.hpp"

namespace cst
{
	extern AsyncWebServer server;
	void begin_network();
	void begin_server();
} // namespace cst
