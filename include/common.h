#pragma once

#include <cstddef>
#include <functional>
#include <vector>
#include <system_error>

namespace anp
{
	typedef std::function<void(const std::vector<char>&, std::size_t, int)> on_client_data_cb;
	typedef std::function<void(asio::error_code&) > asio_operation_cb;
}