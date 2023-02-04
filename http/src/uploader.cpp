// uploader.cpp

#ifdef __cpp_lib_format
#include <format>
#endif
#include <fstream>
#include <tcp/client.h>
#include <utils/Log.h>
#include <utils/string_utils.h>
#include <utils/file_utils.h>
#include <utils/datetime.h>
#include <tcp/client.h>
#include "uploader.h"

LOG_PREFIX("[uploader]: ");
LOG_POSTFIX("\n");
SET_LOG_DEBUG(true);

namespace fs = std::filesystem;

namespace anp
{
	int uploader::upload_file(
		const std::string& host,
		int port,
		const fs::path& local_path,
		const std::string& q
	)
	{
		std::string fname = local_path.filename().string();
		headers_t m_headers;
		std::ifstream f(local_path);
		if (!f.is_open())
			return notify(erc::file_not_exists);
		std::string file_data = utils::file::contents(local_path);
		m_headers.add({ "Content-Type", "multipart/form-data; boundary=dsfjiofadsio"});
		std::string body = utils::format_str(
			"--dsfjiofadsio\r\n"
			"Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"
			"Content-Type: text/plain\r\n\r\n"
			"%s\r\n"
			"--dsfjiofadsio--"
			, fname.c_str(), file_data.c_str()
		);
		m_headers.add({ "Content-Length", std::to_string(body.size()) });
		return query({ host, port }, "POST", q, [=, this](
			const headers_t& headers
			, const char* data
			, std::size_t sz
			, int status
			) -> bool
		{
			std::string s(data, data + sz);
			if (s.find("was uploaded successfully") != std::string::npos)
			{
				LOG_DEBUG("Upload has been completed");
				notify(http_client::erc::no_error);
				return true;
			}
			else if (s.find("Auth error") != std::string::npos)
			{
				notify(erc::auth_error);
				return false;
			}
			else
			{
				LOG_DEBUG("Upload failed");
				notify(erc::transfer_error);
				return true;
			}
			return false;
		}, m_headers, body);
	}

	void uploader::on_notify(int ec)
	{
	}

	void uploader::on_reset()
	{
	}
}