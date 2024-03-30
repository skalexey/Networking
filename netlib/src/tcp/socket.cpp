#include <tcp/socket.h>
#include <utils/log.h>

LOG_TITLE("tcp/socket")

namespace anp
{
	namespace tcp
	{
		void socket::close()
		{
			LOG_VERBOSE("socket::close()");
			LOG_VERBOSE("	Shutting down...");
			m_soc.shutdown(asio::ip::tcp::socket::shutdown_both);
			LOG_VERBOSE("	Closing...");
			m_soc.close();
			LOG_VERBOSE("socket::close() done");
		}
	}
}