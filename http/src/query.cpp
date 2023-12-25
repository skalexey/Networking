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
		if (!m_values.empty())
		{
			uri += "?";
			uri += m_values.to_string();
		}
		else if(path.empty())
		{
			uri = "/";
		}
		return uri;
	}

	void query_t::add_value(const std::string& key, const std::string& value)
	{
		// auto value_encoded = utils::url_encode(value);
		m_values.add(key, value);
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