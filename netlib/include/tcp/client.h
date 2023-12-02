#pragma once

#include <tcp/client_base.h>
#include <tcp/connection.h>

namespace anp
{
	namespace tcp
	{
		class client : public client_base {
			std::unique_ptr<anp::tcp::connection_base> make_connection() override {
				return std::make_unique<anp::tcp::connection>(io_context());
			}
		};
	}
}
