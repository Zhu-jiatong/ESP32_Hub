#include "networking.hpp"
namespace cst
{
	AsyncWebServer server(80);

	void handle_root(AsyncWebServerRequest *request) { request->send(200); }

	void handle_dir(AsyncWebServerRequest *request)
	{
		auto mountpoint = request->getParam("disk")->value().c_str();
		auto disk_info = cst::get_instance(mountpoint);
		if (!disk_info.success)
			return request->send(404, "text/plain", "404: invalid directory");
		const auto &dir_path = request->getParam("path")->value();
		JSONVar files_JSON;
		size_t counter = 0;
		auto JSON_fn = [&](File &f)
		{
			files_JSON[counter]["name"] = f.name();
			files_JSON[counter]["path"] = f.path();
			files_JSON[counter]["size"] = f.size();
			files_JSON[counter]["dir"] = f.isDirectory();
			++counter;
		};
		for_each_file(disk_info.instance->open(dir_path), JSON_fn, false, 0);
		request->send(200, "application/json", JSON.stringify(files_JSON));
	}

	void begin_network()
	{
		WiFi.softAP("Tony ESP32");
		delay(1000);
		Serial.println(WiFi.softAPIP());
	}

	void begin_server()
	{
		server.on("/", HTTP_GET, handle_root);
		server.on("/dir", HTTP_GET, handle_dir);
		server.begin();
	}
} // namespace cst
