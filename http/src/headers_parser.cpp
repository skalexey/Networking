// headers_parser.cpp

#include <cassert>
#include <utils/Log.h>
#include <utils/string_utils.h>
#include <http/headers_parser.h>

LOG_PREFIX("[headers_parser]: ");
LOG_POSTFIX("\n");

namespace anp
{
	int headers_parser::parse(const std::string_view& s)
	{
		auto on_headers_received = [&] {
			auto& sc = m_headers.get("Content-Length");
			if (!sc.empty())
			{
				try
				{
					m_content_length = std::atoi(sc.c_str());
				}
				catch (...)
				{
					LOG_ERROR("Can't parse content length '" << sc << "'");
					return erc::parse_size_error;
				}

				if (!m_content_length.has_value())
					return erc::receive_size_error;
			}
			else
			{
				LOG_WARNING("No content length received from the server");
				//return erc::no_content_length;
			}
			auto& te = m_headers.get("Transfer-Encoding");
			if (!te.empty())
			{
				if (te == "chunked")
					m_transfer_encoding = transfer_encoding_type::chunked;
			}
			return erc::done;
		};

		m_cursor = 0; // Cursor

		do
		{
			// Parse headers
			auto p = s.find("\r\n", m_cursor); // Header end position
			if (p != std::string::npos)
			{
				if (p == m_cursor && m_last_data.empty())
				{
					// Headers block finished
					if (auto r = on_headers_received())
					{
						LOG_DEBUG("Headers parsing error: " << r);
						return m_error_code = r;
					}
					m_cursor = p + 2;
					m_last_data.clear();
					return erc::done;
				}
				else
				{
					// Header
					std::string_view h(&*(s.begin() + m_cursor), p - m_cursor);
					auto colon_p = h.find_first_of(":");
					if (colon_p != std::string::npos)
					{
						auto n = h.substr(0, colon_p); // Header name
						auto v = h.substr(colon_p + 2);
						m_headers.add({ utils::str_tolower(n), v });
					}
					else
					{
						// Parse HTTP status
						auto hp = h.find("HTTP/1");
						if (hp != std::string::npos)
						{
							auto sp = h.find_first_of(' '); // Begin of status
							if (sp != std::string::npos)
							{
								auto spe = h.find_first_of(' ', sp + 1);// End of status
								if (spe != std::string::npos)
								{
									try
									{
										m_status = std::atoi(h.substr(sp + 1, spe - sp).data());
									}
									catch (const std::exception& ex)
									{
										LOG_ERROR("Can't parse HTTP status");
										LOG_DEBUG("Exception: " << ex.what());
										return m_error_code = erc::status_parse_error;
									}
								}
							}
						}
					}
					m_cursor = p + 2;
				}
			}
			else
			{
				m_last_data = s.substr(m_cursor);
				return erc::in_progress;
			}
		} while (true);

		assert(false && "Should not be here");
		return erc::erc_count;
	}

	void headers_parser::reset()
	{
		m_error_code = erc::initial;
		m_cursor = 0;
		m_status = -1;
		m_content_length.reset();
		m_headers.clear();
		m_last_data.clear();
	}
}