// headers.h

#pragma once

#include <string>
#include <string_view>
#include <utils/ordered_map.h>

namespace anp
{
	struct header {
		std::string name;
		std::string value;
		header(const std::string_view& name, const std::string_view& value) : name(name), value(value) {}
		std::string to_string() const;
		operator std::string() const;
	};

	struct headers_t
	{
		using data_t = utils::ordered_map<std::string, header>;
		headers_t() = default;
		headers_t(const data_t& data) : m_data(data) {}
		std::string to_string() const;
		operator std::string() const;
		void add(const header& h);
		const std::string& get(const std::string& name) const;
		const data_t& data() const { return m_data; }
		void clear() { m_data.clear(); }

	private:
		data_t m_data;
	};
}