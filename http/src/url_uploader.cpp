// url_uploader.cpp

#include <http/uploader.h>
#include <http/url_uploader.h>
#include <utils/string_utils.h>
#include <utils/file_utils.h>
#include <utils/datetime.h>
#include <utils/base64.h>
#include <utils/zip.h>
#include <utils/Log.h>

LOG_TITLE("url_uploader");
SET_LOCAL_LOG_LEVEL(debug);

namespace anp
{
	namespace http
	{
		void url_uploader::upload_file_async(
			const tcp::endpoint_t& ep,
			const fs::path& local_path,
			const query_t& query,
			const utils::void_int_cb& cb
		)
		{
			LOG_DEBUG("Uploading file: " << local_path.string() << " to " << query.uri());

			std::string fname = local_path.filename().string();
			query_t q = query;
			std::ifstream f(local_path);
			if (!f.is_open())
			{
				if (cb)
					cb(errcode());
				notify(erc::file_not_exists);
				return;
			}
			std::string file_data = utils::file::contents(local_path);
			q.add_value("fname", fname);
			utils::zip::data_t compressed_data;
			utils::zip::compress(file_data, compressed_data);
			std::string file_data_prepared = base64(compressed_data);
			q.headers.add({ "Content-Length", std::to_string(compressed_data.size())});
			q.headers.add({ "Content-Type", "application/zip" });
			//q.add_value("data", file_data_prepared);
			q.body = compressed_data;
			q.method = "POST";
			this->query_async(ep, q, [self = this, cb](
				const http::headers_t& headers
				, const char* data
				, std::size_t sz
				, int status
				) -> bool
			{
				std::string s(data, data + sz);
				int ec = self->parse_upload_result_code(s);
				if (cb)
					cb(ec);
				self->notify(ec);
				return self->errcode() == erc::no_error;
			});
		}
	}
}