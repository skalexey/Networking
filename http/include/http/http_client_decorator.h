// http_client_decorator.h

#pragma once

#include <utils/filesystem.h>
#include "http_client.h"
#include "http_client_interface.h"

namespace anp
{
	class http_client_decorator : public virtual http_client_interface
	{
	public:
		http_client_decorator(http_client_interface* object) : m_object(object) {}

		int request(
			const tcp::endpoint_t& endpoint,
			const std::string& request,
			const http_response_cb& on_receive
		)
		{
			return m_object->request(endpoint, request, on_receive);
		}

		void request_async(
			const tcp::endpoint_t& endpoint,
			const std::string& request,
			const http_response_cb& on_receive
		)
		{
			m_object->request_async(endpoint, request, on_receive);
		}

		int errcode() override
		{
			return m_object->errcode();
		}

		void query_async(
			const tcp::endpoint_t& endpoint,
			const std::string& method,
			const std::string& query,
			const http_response_cb& on_receive = http_response_cb(),
			const http::headers_t& m_headers = http::headers_t(),
			const std::string& body = ""
		) override
		{
			m_object->query_async(endpoint, method, query, on_receive, m_headers, body);
		}
		
		void wait() override
		{
			m_object->wait();
		}

		int notify(int ec) override
		{
			on_before_notify(ec);
			auto result = m_object->notify(ec);
			on_notify(ec);
			return result;
		}

		void set_receive_file(const fs::path& file_path) override
		{
			m_object->set_receive_file(file_path);
		}

		void set_receive_mode(int mode) override
		{
			m_object->set_receive_mode(mode);
		}

		const fs::path& get_file_path() const override
		{
			return m_object->get_file_path();
		}

		bool remove_received_file() override
		{
			return m_object->remove_received_file();
		}

		void reset(client_type c = client_type::http) override {
			m_object->reset(c);
			on_reset();
		};

	protected:
		void on_before_notify(int ec) override {};
		void on_notify(int ec) override {};
		
	private:
		http_client_interface* m_object = nullptr;
	};
}