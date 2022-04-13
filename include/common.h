#pragma once

#include <functional>
#include <string>

namespace anp
{
	typedef std::function<void(const std::string&)> on_data_cb;
}