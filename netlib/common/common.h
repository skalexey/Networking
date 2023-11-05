#pragma once

#include <cstddef>
#include <functional>
#include <vector>
#include <system_error>
#include <asio.hpp>
#include <utils/common.h>

namespace anp
{
	using data_cb = std::function<bool(
		const std::vector<char>&	// Buffer
		, std::size_t				// Size
		, int						// Id
	)>;

	using error_cb = std::function<void(const std::error_code&)>;
	using asio_operation_cb = error_cb;
	using result_cb = utils::void_int_cb;
}