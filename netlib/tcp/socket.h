#pragma once

#include <vector>
#include <asio/ts/internet.hpp>
#include <tcp/socket_base.h>

namespace anp
{
	namespace tcp
	{
		using tcp_socket_soc_t = asio::ip::tcp::socket;
		using tcp_socket_base = tcp::socket_base<std::vector<char>, asio::ip::tcp::socket>;
		class socket : public tcp_socket_base
		{
		public:
			using base = tcp_socket_base;
			using soc_t = tcp_socket_soc_t;
			socket(soc_t&& soc) : m_soc(std::move(soc)) {}
			socket(asio::io_context& ctx) : m_soc(ctx) {}
			const soc_t& get_soc() const override { return m_soc; }

		private:
			soc_t m_soc;
		};
	}
}
