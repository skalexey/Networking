// uploader.h

#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include "authenticator.h"

namespace anp
{
	class uploader : public authenticator
	{
		using base = http_client;
		using http_client::endpoint_t;

	public:
		enum erc : int
		{
			file_not_exists = http_client::erc::count,
			transfer_error,
			auth_error
		};

		int upload_file(
			const endpoint_t& ep,
			const std::filesystem::path& target_path,
			const credentials& credentials,
			const std::string& url_path = ""
		);

	protected:
		void on_notify(int ec) override;
		void on_reset() override;
	};
}