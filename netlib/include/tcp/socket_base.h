﻿#pragma once

#include <cstddef>
#include <anp/common/asio_predefs.h>
#include <asio.hpp>
#include <socket_interface.h>

namespace anp
{
	namespace tcp
	{
		using namespace boost;

		template <typename Buffer_t, typename Soc_t>
		class socket_base : public socket_interface<Buffer_t>
		{
		public:
			using base = socket_interface<Buffer_t>;
			using buffer_value_type = typename base::buffer_t::value_type;
			void async_read_some(typename base::buffer_t& buf, const typename base::response_cb_t& cb) override {
				soc().async_read_some(asio::buffer(buf.data(), buf.size()), cb);
			}
			void async_write(const buffer_value_type* begin, std::size_t size, const typename base::response_cb_t& cb) override {
				asio::async_write(soc(), asio::buffer(begin, size), cb);
			}
			bool is_open() const override {
				return get_soc().is_open();
			}
			virtual const Soc_t& get_soc() const = 0;
			Soc_t& soc() {
				return const_cast<Soc_t&>(get_soc());
			}
		};
	}
}
