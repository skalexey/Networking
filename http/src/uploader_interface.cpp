#include <http/uploader_interface.h>
#include <http/authenticator.h>
#include <utils/Log.h>

LOG_PREFIX("[uploader_interface]: ");
LOG_POSTFIX("\n");
SET_LOCAL_LOG_DEBUG(true);

namespace anp
{
	namespace http
	{
		int uploader_interface::parse_upload_result_code(const std::string& s)
		{
			int ec = 0;
			if (s.find("uploaded successfully") != std::string::npos)
			{
				LOG_DEBUG("Upload has been completed");
				ec = erc::no_error;
			}
			else if (s.find("Auth error") != std::string::npos)
			{
				ec = erc::auth_error;
			}
			else if (s.find("newer version is on the server"))
			{
				LOG_DEBUG("Newer version is on the server");
				ec = erc::newer_version_on_server;
			}
			else
			{
				LOG_DEBUG("Upload failed");
				ec = erc::transfer_error;
			}
			return ec;
		}
	}
}