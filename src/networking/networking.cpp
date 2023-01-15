#include "networking.hpp"

namespace cst
{
	AsyncWebServer server(80);
	const String system_dir = "/system",
				 credentials_dir = system_dir + "/users/accounts",
				 uni_config_dir = system_dir + "/config";

	bool login_auth(const char *id, const char *pwd)
	{
		auto auth_file = system_disk.open(credentials_dir + "/" + id);
		return auth_file && auth_file.readString() == pwd;
	}

	void handle_login(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
	{
		auto rcv_data = JSON.parse(reinterpret_cast<const char *>(data));
		auto rcv_id = static_cast<const char *>(rcv_data["id"]),
			 rcv_pwd = static_cast<const char *>(rcv_data["pw"]);
		if (!login_auth(rcv_id, rcv_pwd))
			return request->send(200, "text/plain", "Authentication failed: incorrect username/password.");
		auto &new_session = session_manager.newSession(request->client()->localIP(), request->client()->getRemoteAddress(), rcv_id);
		request->send(200, "text/plain", "Login success: hello, " + new_session._userID);
	}

	void get_login_info(AsyncWebServerRequest *request)
	{
		auto this_session = session_manager.getSessionInfo(request->client()->localIP(), request->client()->getRemoteAddress());
		if (this_session == session_manager.emptySession)
			return request->send(200, "text/plain", "Not logged in.");

		request->send(200, "application/json", JSON.stringify(this_session.toJSON()));
	}

	void handle_logoff(AsyncWebServerRequest *request)
	{
		session_manager.removeSession(request->client()->localIP(), request->client()->getRemoteAddress());
		return request->send(200);
	}

	void handle_register(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
	{
		auto reg_info = JSON.parse(reinterpret_cast<const char *>(data));
		auto reg_path = credentials_dir + static_cast<const char *>(reg_info["id"]);
		if (system_disk.exists(reg_path))
			return request->send(200, "text/plain", "Registration falied: username taken");
		system_disk.open(reg_path, FILE_WRITE, true).print(static_cast<const char *>(reg_info["pw"]));
		request->send(200, "text/plain", "Registration success");
	}

	void handle_delete_account(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
	{
		auto del_info = JSON.parse(reinterpret_cast<const char *>(data));
		auto del_file = system_disk.open(credentials_dir + static_cast<const char *>(del_info["id"]));
		if (!del_file && del_file.readString() != static_cast<const char *>(del_info["pw"]))
			return request->send(200, "text/plain", "Deletion falied: user not exist");
		if (!system_disk.remove(del_file.path()))
			return request->send(200, "text/plain", "Deletion falied: incorrect password");
		request->send(200, "text/plain", "Deletion success: logging off");
		handle_logoff(request);
	}

	void handle_dir(AsyncWebServerRequest *request)
	{
		auto mountpoint = request->getParam("disk")->value().c_str();
		auto disk_info = get_instance(mountpoint);
		if (!disk_info.success)
			return request->send(404, "text/plain", "404: invalid disk");
		const auto &dir_path = request->getParam("path")->value();
		if (!disk_info.instance->exists(dir_path))
			return request->send(404, "text/plain", "404: invalid path");

		JSONVar files_JSON;
		size_t counter = 0;
		auto JSON_fn = [&](File &f)
		{
			files_JSON[counter]["name"] = f.name();
			files_JSON[counter]["path"] = f.path();
			files_JSON[counter]["size"] = f.size();
			files_JSON[counter]["disk"] = mountpoint;
			files_JSON[counter]["dir"] = f.isDirectory();
			files_JSON[counter]["mod"] = f.getLastWrite();
			++counter;
		};
		forward_fs_iterator(disk_info.instance->open(dir_path), JSON_fn);
		request->send(200, "application/json", JSON.stringify(files_JSON));
	}

	const char *get_file_type(const String &fname)
	{
		return parse_json_file(system_disk.open(uni_config_dir + "/types.json"))[fname.substring(fname.lastIndexOf("."))];
	}

	void handle_file(AsyncWebServerRequest *request)
	{
		auto mountpoint = request->getParam("disk")->value().c_str();
		auto disk_info = get_instance(mountpoint);

		if (!disk_info.success)
			return request->send(404, "text/plain", "404: invalid disk");
		const auto &dir_path = request->getParam("path")->value();
		if (!disk_info.instance->exists(dir_path))
			return request->send(404, "text/plain", "404: file not found");

		auto &action = request->getParam("act")->value();
		if (action == "view")
			return request->send(*disk_info.instance, dir_path, get_file_type(dir_path));
		if (action == "download")
			return request->send(*disk_info.instance, dir_path, emptyString, true);
		if (action == "delete")
			return request->send(200, "text/plain", "Deleted " + String(recursive_delete(*disk_info.instance, disk_info.instance->open(dir_path))) + " files");

		request->send(404, "text/plain", "404: invalid param");
	}

	void handle_not_found(AsyncWebServerRequest *request)
	{
		/* auto &disk_path = request->url();
		auto path_split_index = disk_path.indexOf("/", 1);
		auto disk = cst::get_instance(disk_path.substring(0, path_split_index).c_str());
		auto path = disk_path.substring(path_split_index); */
		auto path_info = parse_path(request->url());
		auto disk = get_instance(path_info.disk.c_str());
		if (disk.success && disk.instance->exists(path_info.dir))
			return request->send(*disk.instance, path_info.dir, get_file_type(path_info.dir));

		JSONVar ret;
		ret["uri"] = request->url();
		ret["method"] = request->methodToString();
		for (uint8_t i = 0; i < request->params(); ++i)
			ret["param"][request->getParam(i)->name()] = request->getParam(i)->value();
		request->send(404, "application/json", JSON.stringify(ret));
	}

	void handle_file_upload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
	{
		if (!index)
		{
			auto disk_info = get_instance(request->getParam("disk")->value().c_str());
			if (!disk_info.success)
				return request->send(404, "text/plain", "404: invalid disk");
			auto path = request->getParam("path")->value();
			if (!(request->_tempFile = disk_info.instance->open(path + "/" + filename, FILE_WRITE)))
				return request->send(404, "text/plain", "404: invalid path");
		}
		request->_tempFile.write(data, len);
		if (final)
		{
			request->_tempFile.close();
		}
	}

	void handle_post_request(AsyncWebServerRequest *request) { request->send(200); }

	void begin_network()
	{
		WiFi.softAP("Tony ESP32");
		// delay(1000);
		// Serial.println(WiFi.softAPIP());
		MDNS.begin("esphub");
		MDNS.addService("http", "tcp", 80);
	}

	void begin_server()
	{
		server.serveStatic("/", system_disk, (system_dir + "/webpage").c_str()).setDefaultFile("index.html");
		server.on("/dir", HTTP_GET, handle_dir);
		server.on("/file", HTTP_GET, handle_file);
		server.on("/file", HTTP_POST, handle_post_request, handle_file_upload);
		server.on("/login", HTTP_POST, nullptr, nullptr, handle_login);
		server.on("/logoff", HTTP_GET, handle_logoff);
		server.on("/register", HTTP_POST, nullptr, nullptr, handle_register);
		server.on("/delete", HTTP_POST, nullptr, nullptr, handle_delete_account);
		server.on("/logininfo", HTTP_GET, get_login_info);
		server.onNotFound(handle_not_found);
		server.begin();
	}
} // namespace cst
