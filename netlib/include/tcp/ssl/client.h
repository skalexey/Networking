// client.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <tcp/client_base.h>
#include <tcp/ssl/connection.h>

namespace anp
{
	namespace tcp
	{
		namespace ssl
		{
			class client : public tcp::client_base
			{
			protected:
				std::unique_ptr<anp::tcp::connection_base> make_connection() override {
					return std::make_unique<anp::tcp::ssl::connection>(io_context());
				}
			};
		}
	}
}
