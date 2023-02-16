// authenticator.h

#pragma once

#include <chrono>
#include <memory>
#include <string>
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

	protected:
		void on_notify(int ec) override;
		void on_reset() override;

	private:
		std::string m_user;
		std::string m_token;
	};
}