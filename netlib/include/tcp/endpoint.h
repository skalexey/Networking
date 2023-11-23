// endpoint.h

#pragma once

#include <string>

namespace anp
{
	namespace tcp
	{
		struct endpoint_t
		{
			std::string host;
			int port;
		};
	}
}