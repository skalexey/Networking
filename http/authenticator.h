// authenticator.h

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <common/common.h>
#include "http_client.h"

namespace anp
{
	struct credentials
	{
		std::string user;
		std::string token;
		query_t query() const;
	};

	class authenticator : public http_client
	{
		using base = http_client;

	public:
		enum erc : int
		{
			auth_error = http_client::erc::count,
		};

		int auth(const endpoint_t& ep, const std::string& path, const credentials& credentials);
		void auth_async(const endpoint_t& ep, const std::string& path, const credentials& credentials, const anp::result_cb& on_result);

	protected:
		void on_notify(int ec) override {};
		void on_reset() override {}

	private:
		std::string m_user;
		std::string m_token;
	};
	using authenticator_ptr = std::shared_ptr<authenticator>;
}