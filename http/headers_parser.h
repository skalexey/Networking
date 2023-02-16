﻿// headers_parser.h

#pragma once

#include <cstddef>
#include <string>
#include <optional>
#include <string_view>
#include "headers.h"

namespace anp
{
	// --- headers_parser ---
	class headers_parser
	{
	public:
		enum erc : int
		{
			initial = -2,
			in_progress = -1,
			done = 0,
			receive_size_error,
			status_parse_error,
			no_content_length,
			parse_size_error,
			count
		};

	public:
		int parse(const std::string_view& s);
		int error_code() const { return m_error_code; }
		int status() const { return m_status; }
		std::size_t content_length() const { return m_content_length.has_value() ? m_content_length.value() : 0; }
		std::size_t cursor() const { return m_cursor; }
		const headers_t& headers() const { return m_headers; }
		void reset();

	private:
		int m_error_code = 0;
		headers_t m_headers;
		std::optional<std::size_t> m_content_length;
		int m_status = -1;
		std::string m_last_data;
		std::size_t m_cursor = 0;
	};
}