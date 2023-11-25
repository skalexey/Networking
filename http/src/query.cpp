// query.cpp

#pragma once

#include <http/query.h>

namespace
{
	const std::string empty_string;
}

namespace anp
{
	std::string query_t::uri() const
	{
		std::string uri = path;
		if (!values.empty())
		{
			uri += "?";
			uri += values.to_string();
		}
		return uri;
	}

	std::string values_t::to_string() const
	{
		std::string s;
		for (auto&& h : m_data)
		{
			if (!s.empty())
				s += "&";
			s += h.first + "=" + h.second;
		}
		return s;
	}

	void values_t::add(const std::string& key, const std::string &value)
	{
		m_data.add(key, value);
	}

	const std::string &values_t::get(const std::string& key) const
	{
		if (auto it = m_data.find(key))
			return (*it).second;
		return empty_string;
	}
}