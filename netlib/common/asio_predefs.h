#pragma once

#ifdef _WIN32
	#define _WIN32_WINNT 0x0A00
#endif

#ifndef ASIO_STANDALONE
	#define ASIO_STANDALONE
#endif
