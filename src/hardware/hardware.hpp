#pragma once

#include <SD.h>
#include "vfs_api.h"
#include <array>

struct SD_info_t
{
	uint8_t CS, MOSI, MISO, SCK;
	const char *mountpoint;
	SDFS &SD_instance;
};

struct get_instance_ret
{
	bool success;
	SDFS *instance;
};

namespace cst
{
	extern SDFS &SD_vspi, SD_hspi, &system_disk;
	extern SD_info_t &SD_vspi_cfg, &SD_hspi_cfg;

	void begin_SD();
	void load_cfg();
	get_instance_ret get_instance(const char *mountpoint);
} // namespace cst
