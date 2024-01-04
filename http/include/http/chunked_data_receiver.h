// chunked_data_receiver.h
//
// This receiver allows to receive data in chunks.
// It is parameterized by a type of any other receiver that follows data_receiver_base interface.
// so that it can be used as a wrapper for any other receiver, that allows to store the chunked data
// into a file or memory depending on needs.
//
// 4␍␊            (chunk size is four octets)
// Wiki           (four octets of data)
// ␍␊             (end of chunk)

// 7␍␊            (chunk size is seven octets)
// pedia i        (seven octets of data)
// ␍␊             (end of chunk)

// B␍␊            (chunk size is eleven octets)
// n ␍␊chunks.    (eleven octets of data)
// ␍␊             (end of chunk)

// 0␍␊            (chunk size is zero octets, no more chunks)
// ␍␊             (end of final chunk with zero data octets)
//
//
// Info from https://en.wikipedia.org/wiki/Chunked_transfer_encoding

#pragma once

#include <exception>
#include <algorithm>
#include <utility>
#include <optional>
#include <vector>
#include <cstddef>
#include <stdio.h>
#include <string.h>
#include <utils/data_receiver_memory.h>
#include <utils/macros.h>
#include <utils/parameters.h>

namespace anp
{
	namespace http
	{
		// --- receiver_memory ---
		template <typename Final_receiver_t>
		class chunked_data_receiver : public utils::data::receiver_base<typename Final_receiver_t::data_element_t>
		{
		public:
			using base = utils::data::receiver_base<typename Final_receiver_t::data_element_t>;
			using data_t = typename Final_receiver_t::data_t;
			using data_element_t = typename Final_receiver_t::data_element_t;
			enum chunked_data_receiver_error : int
			{
				control_block_partial_receive = Final_receiver_t::count,
				count,
			};
		public:
			chunked_data_receiver() {
				reset(0);
			}
			template <typename... Args>
			chunked_data_receiver(Args&&... args)
				: m_final_receiver(std::forward<Args>(args)...)
			{
				reset(utils::get_n_pack_arg<0, Args...>(std::forward<Args>(args)...));
			}

			std::size_t receive_impl(const data_element_t* data, const std::size_t& size) override {
				assert(m_chunk_size != 0);
				const data_element_t* cur = data;
				const data_element_t* end = data + size;
				while (cur < data + size)
				{
					if (m_final_receiver.full())
					{
						// Receiveing the chunk size
						if (!m_chunk_size.has_value())
						{
							const data_element_t* cycle_start_pos = cur;
							cur = std::find(cur, end, '\r');
							auto _ = cycle_start_pos;
							if (!receive_control_data(_, cur - cycle_start_pos))
								return cur - data;
							if (cur != end)
							{
								// The chunk size received
								int cs = 0;
								std::string fmt = "%" + std::to_string(m_control_data_receiver.size()) + "x";
								auto scanf_result = sscanf(m_control_data_receiver.data(), fmt.c_str(), &cs);
								assert(scanf_result > 0);
								m_chunk_size = cs;
								m_control_data_receiver.reset(2); // For the size terminator
							}
							else // The data ended on \r. Wait for the next receive call
								return cur - data;
						}
						// Receiving the chunk header terminator
						if (!receive_control_data(cur, 2))
							return cur - data;
						if (!m_control_data_receiver.empty() && strncmp(m_control_data_receiver.data(), "\r\n", 2) == 0)
						{
							m_control_data_receiver.reset(2); // For the chunk terminator
							m_final_receiver.grow(m_chunk_size.value());
						}
					}
					// Receiveing the data
					auto data_amount_before = this->size();
					m_final_receiver.receive(cur, size - (cur - data)); // It will receive no more than the current chunk
					if (base::error_code() != 0)
						return cur - data;
					auto received_data_amount = this->size() - data_amount_before;
					assert(received_data_amount >= 0);
					cur += received_data_amount;
					if (m_final_receiver.full())
					{
						// Receiving the chunk data terminator
						if (!receive_control_data(cur, 2))
							return cur - data;
						if (m_chunk_size > 0)
							if (!m_control_data_receiver.empty() && strncmp(m_control_data_receiver.data(), "\r\n", 2) == 0)
							{
								m_chunk_size.reset();
								m_control_data_receiver.reset();
								// The chunk received
							}
					}
				}
				return cur - data;
			}
			bool full() const override {
				return m_chunk_size == 0 && m_final_receiver.full();
			}
			const data_element_t* data() const override {
				return m_final_receiver.data();
			}
			size_t size() const override {
				return m_final_receiver.size();
			}
			std::size_t target_size() const override {
				return m_final_receiver.target_size();
			}
			void reset(const std::size_t& size = 0) override {
				base::reset(size);
				m_final_receiver.reset(size);
				m_control_data_receiver.reset();
				m_chunk_size.reset();
			}
			void grow(const std::size_t& size) override {
				m_final_receiver.grow(size);
			}

		protected:
			bool receive_control_data(const data_element_t*& cur, const std::size_t& size) {
				auto received = m_control_data_receiver.receive(cur, size);
				cur += received;
				return base::error_code() == 0 && (m_control_data_receiver.full() || m_control_data_receiver.is_unlimited());
			}

		private:
			Final_receiver_t m_final_receiver;
			utils::data::receiver_memory<data_element_t> m_control_data_receiver;
			std::optional<std::size_t> m_chunk_size;
		};
	}
}