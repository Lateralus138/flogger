#pragma once
namespace flogger {
	std::tm safe_localtime(const std::time_t& t);
	std::string timestampA();
	std::wstring timestampW();
}