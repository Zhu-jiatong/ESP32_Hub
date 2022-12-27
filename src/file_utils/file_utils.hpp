#pragma once

#include <FS.h>
#include <functional>

using for_each_file_cb = std::function<void(File &)>;

namespace cst
{
	size_t for_each_file(File root, const for_each_file_cb &cb, bool inverse_recursion = false, const int &max_recursion_depth = -1);
} // namespace cst
