// query.h

#pragma once

#include <string>
#include <vector>
#include <utility>
#include <utils/ordered_map.h>
#include "headers.h"

namespace anp
{
	struct values_t
	{
		using data_t = utils::ordered_map<std::string, std::string>;
		std::string to_string() const;
		bool empty() const { return m_data.empty(); }
		void add(const std::string& key, const std::string& value);
		const std::string& get(const std::string& key) const;
		data_t& data() { return m_data; }
		const data_t& get_data() const { return m_data; }

	private:
		data_t m_data;
	};

	struct query_t
	{	
		std::string path;
		values_t values;
		std::string method;
		http::headers_t headers;
		std::string body;
		
		std::string uri() const;
	};
}