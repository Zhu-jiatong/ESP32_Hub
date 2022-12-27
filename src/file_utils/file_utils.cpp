#include "file_utils.hpp"

namespace cst
{
	void _for_each_file(File &root, size_t &recursion_depth, size_t &file_count, const for_each_file_cb &cb, bool inverse_recursion, const int &max_recursion_depth)
	{
		if (recursion_depth > max_recursion_depth)
			return;
		for (; auto next_file = root.openNextFile(); ++file_count)
		{
			if (!inverse_recursion)
				cb(next_file);
			if (next_file.isDirectory() || max_recursion_depth >= 0)
			{
				_for_each_file(next_file, ++recursion_depth, file_count, cb, inverse_recursion, max_recursion_depth);
				--recursion_depth;
			}
			if (inverse_recursion)
				cb(next_file);
		}
	}

	size_t for_each_file(File root, const for_each_file_cb &cb, bool inverse_recursion, const int &max_recursion_depth)
	{
		size_t init_recursion_depth = 0, init_file_count = 0;
		_for_each_file(root, init_recursion_depth, init_file_count, cb, inverse_recursion, max_recursion_depth);
		return init_file_count;
	}
} // namespace cst
