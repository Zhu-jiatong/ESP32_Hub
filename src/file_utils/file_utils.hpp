#pragma once

#include <SD.h>
#include <functional>
#include <Arduino_JSON.h>

using for_each_file_cb = std::function<void(File &)>;

namespace cst
{
	struct path_info_t
	{
		String disk, dir;
	};

	size_t forward_fs_iterator(File root, const for_each_file_cb &cb, bool recursive = false);
	size_t reverse_fs_iterator(File root, const for_each_file_cb &cb, bool recursive = false);
	size_t recursive_delete(SDFS &disk, File root);
	JSONVar parse_json_file(File path);
	path_info_t parse_path(const String &path);
} // namespace cst
