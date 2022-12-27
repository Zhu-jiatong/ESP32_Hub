#include "hardware.hpp"

namespace cst
{
	SDFS &SD_vspi = SD, SD_hspi(FSImplPtr(new VFSImpl()));
	SPIClass hspi(HSPI);
	SPIClass &vspi = SPI;

	std::array<SD_info_t, 2> disks{{{SS, MOSI, MISO, SCK, "/sdv", SD_vspi}, {15, 27, 26, 25, "/sdh", SD_hspi}}};
	SD_info_t &SD_vspi_cfg = disks[0], &SD_hspi_cfg = disks[1];

	void begin_SD()
	{
		SD_vspi.begin(SD_vspi_cfg.CS, vspi, 80000000U, SD_vspi_cfg.mountpoint);
		hspi.begin(SD_hspi_cfg.SCK, SD_hspi_cfg.MISO, SD_hspi_cfg.MOSI, SD_hspi_cfg.CS);
		SD_hspi.begin(SD_hspi_cfg.CS, hspi, 80000000U, SD_hspi_cfg.mountpoint);
	}

	void load_cfg() {}

	get_instance_ret get_instance(const char *mountpoint)
	{
		get_instance_ret temp{};
		auto found_instance = std::find_if(disks.begin(), disks.end(), [&](SD_info_t this_disk)
										   { return strcmp(mountpoint, this_disk.mountpoint) == 0; });
		if (found_instance != disks.end())
			return {true, &found_instance->SD_instance};
		return {false, nullptr};
	}
} // namespace cst
