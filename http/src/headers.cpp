// headers.cpp

#pragma once

#include <utils/string_utils.h>
#include "headers.h"

namespace
{
	const std::string empty_string;
}

namespace anp
{
	// Begin of headers_t
	std::string headers_t::to_string() const
	{
		std::string r;
		for (auto&& h : m_data)
			r += h.second.to_string();
		return r;
	}

	headers_t::operator std::string() const
	{
		return to_string();
	}

	void headers_t::add(const header& h)
	{
		m_data.add(h.name, h);
	}

	const std::string& headers_t::get(const std::string& name) const
	{
		if (auto it = m_data.find(name))
			return (*it).second.value;
		return empty_string;
	}
	// End of headers_t

	// Begin of header
	std::string header::to_string() const
	{
		return utils::format_str("%s: %s\r\n", name.c_str(), value.c_str());
	}

	header::operator std::string() const
	{
		return to_string();
	}
	// End of header
}