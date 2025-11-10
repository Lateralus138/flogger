#include "pch.h"
#include "ArgumentParser.h"
#include <iostream>
ArgumentParser::ArgumentParser(int argc, char* argv[]) {
	// TODO Possible change internal storage to std::string_view to avoid copies
	// TODO Possible change to unordered_map for switches for faster lookup
	// TODO Possible add system to allow multiple switches with with one '-'
	std::string fullPath = std::string(argv[0]);
	size_t lastSlashPos = fullPath.find_last_of("/\\");
	if (lastSlashPos != std::string::npos) {
		this->executableName = fullPath.substr(lastSlashPos + 1);
	}
	else {
		this->executableName = fullPath;
	}
	this->argc = (argc - 1);
	for (int index = 1; index < argc; ++index) {
		this->argv.emplace_back(std::string(argv[index]));
	}
}
std::string ArgumentParser::getExecutableName() const {
	return this->executableName;
}
void ArgumentParser::setSwitchPair(std::string_view shortName, std::string_view longName) {
	for (auto& sw : this->switches) {
		if (sw.name == shortName) {
			sw.pair = longName;
		}
		if (sw.name == longName) {
			sw.pair = shortName;
		}
	}
}
void ArgumentParser::addSwitch(std::string_view name, std::string_view description) {
	this->switches.emplace_back(
		AddedSwitch(name, description, (name.length() > 1) ? "--" : "-")
	);
}
void ArgumentParser::addSwitch(std::string_view name, std::string_view description, bool hasParam) {
	this->switches.emplace_back(
		AddedSwitch(name, description, (name.length() > 1) ? "--" : "-", hasParam)
	);
}
void ArgumentParser::addSwitch(std::string_view name, std::string_view description, bool hasParam, bool throwIfNotSet) {
	this->switches.emplace_back(
		AddedSwitch(name, description, (name.length() > 1) ? "--" : "-", hasParam, throwIfNotSet)
	);
}
void ArgumentParser::addSwitchPair(std::string_view nameA, std::string_view nameB, std::string_view description, bool hasParam, bool throwIfNotSet) {
	this->addSwitch(nameA, description, hasParam, throwIfNotSet);
	this->addSwitch(nameB, description, hasParam, throwIfNotSet);
	this->setSwitchPair(nameA, nameB);
}
int ArgumentParser::parse() {
	// Optimized: Single pass over argv, O(N) time, no nested loops/ifs
	int possibleError = 256;
	this->arguments.clear();
	for (size_t index = 0; index < this->argv.size(); ++index) {
		std::string_view arg = this->argv[index];
		bool matchedSwitch = false;
		AddedSwitch* matched = nullptr;

		// Find switch in a single pass (linear search, but no nested loops)
		for (auto& sw : this->switches) {
			if (arg == sw.prefix + sw.name) {
				matchedSwitch = true;
				matched = &sw;
				break;
			}
			// Also match paired switch, if any
			if (!sw.pair.empty() && arg == ((sw.pair.length() > 1) ? "--" : "-") + sw.pair) {
				matchedSwitch = true;
				matched = &sw;
				break;
			}
		}

		if (matchedSwitch && matched) {
			matched->isSet = true;
			if (matched->hasParam) {
				possibleError--;
				bool paramMissing = ((index + 1) >= this->argv.size()) ||
					(this->argv[index + 1].rfind("-", 0) == 0) ||
					(this->argv[index + 1].rfind("--", 0) == 0) ||
					(this->argv[index + 1].empty());
				if (paramMissing) {
					if (matched->throwIfNotSet) {
						this->errorCode = possibleError;
						return static_cast<int>(index + 1);
					} else {
						matched->value = "";
					}
				} else {
					matched->value = this->argv[index + 1];
					++index;
				}
			}
		} else {
			this->arguments.emplace_back(arg);
		}
	}
	return this->errorCode;
}
int ArgumentParser::getErrorCode() const {
	return this->errorCode;
}
bool ArgumentParser::isSwitchSet(std::string_view name) const {
	for (const auto& sw : this->switches) {
		if (sw.name == name) {
			return sw.isSet;
		}
	}
	return false;
}
// TODO Possible change printHelp header to std::string_view
void ArgumentParser::printHelp(const std::string& header, bool hasNonSwitchArguments) const {
	std::string message;
	message += '\n';
	message += header;
	message += "\n\nUsage:\n  ";
	message += this->executableName;
	message += ' ';
	message += this->formatSwitchLine(hasNonSwitchArguments);
	message += "\n\nSwitches:\n";
	message += this->formatSwitchList();
	std::cout << message << '\n';
}
void ArgumentParser::printHelpW(const std::wstring& header, bool hasNonSwitchArguments) const {
	const std::wstring executableNameW(this->executableName.begin(), this->executableName.end());

	// Build wstring message piecewise to avoid mismatched type concatenation.
	std::wstring message;
	message += L"\n";
	message += header;
	message += L"\n\nUsage:\n  ";
	message += executableNameW;
	message += L' ';
	message += this->formatSwitchLineW(hasNonSwitchArguments);
	message += L"\n\nSwitches:\n";
	message += this->formatSwitchListW();
	std::wcout << message << L'\n';
}
ArgumentParser::AddedSwitch::AddedSwitch(std::string_view name, std::string_view description, std::string_view prefix) {
	this->name = name;
	this->description = description;
	this->prefix = prefix;
}
ArgumentParser::AddedSwitch::AddedSwitch(std::string_view name, std::string_view description, std::string_view prefix, bool hasParam) {
	this->name = name;
	this->description = description;
	this->prefix = prefix;
	this->hasParam = hasParam;
}
ArgumentParser::AddedSwitch::AddedSwitch(std::string_view name, std::string_view description, std::string_view prefix, bool hasParam, bool throwIfNotSet) {
	this->name = name;
	this->description = description;
	this->prefix = prefix;
	this->hasParam = hasParam;
	this->throwIfNotSet = throwIfNotSet;
}
std::string ArgumentParser::formatSwitchList() const {
	std::string result = "";
	for (const auto& sw : this->switches) {
		if (!sw.pair.empty() && sw.name > sw.pair) {
			continue;
		}
		result += "  " + sw.prefix + sw.name;
		if (!sw.pair.empty()) {
			result += ", " + std::string((sw.pair.length() > 1) ? "--" : "-") + sw.pair;
		}
		if (sw.hasParam) {
			result += " <parameter>";
		}
		result += "\n    " + sw.description + "\n";
	}
	return result;
}
std::string ArgumentParser::formatSwitchLine(bool hasNonSwitchArguments) const {
	const size_t newLineSpace = (this->getExecutableName().length() + 3);
	const std::string newLinePadding(newLineSpace, ' ');
	std::string result = "";
	for (size_t index = 0; index < this->switches.size(); ++index) {
		const auto& sw = this->switches[index];
		if (!sw.pair.empty() && sw.name > sw.pair) {
			continue;
		}
		result += "[";
		result += sw.prefix + sw.name;
		if (!sw.pair.empty()) {
			result += " | " + std::string((sw.pair.length() > 1) ? "--" : "-") + sw.pair;
		}
		if (sw.hasParam) {
			result += " <parameter>";
		}
		result += "]";
		if (index < (this->switches.size() - 2)) {
			result += " |\n" + newLinePadding;
		}
	}
	if (hasNonSwitchArguments) {
		if (!result.empty()) {
			result += "\n" + newLinePadding + "<ARGUMENTS>";
		} else {
			result += "<ARGUMENTS>";
		}
	}
	return result;
}
std::string ArgumentParser::getSwitchValue(std::string_view name) const {
	for (const auto& sw : this->switches) {
		if (sw.name == name) {
			return sw.value;
		}
	}
	return "";
}
std::string ArgumentParser::getArgvValue(size_t index) const {
	if (index < this->argv.size()) {
		return this->argv[index];
	}
	return "";
}
size_t ArgumentParser::getArgumentCount() const {
	return this->arguments.size();
}
std::vector<std::string> const& ArgumentParser::getArguments() const {
	return this->arguments;
}

/*
 * Wide-string helpers to support printHelpW
 */
std::wstring ArgumentParser::formatSwitchListW() const {
	std::wstring result;
	for (const auto& sw : this->switches) {
		if (!sw.pair.empty() && sw.name > sw.pair) {
			continue;
		}
		// prefix + name
		result += L"  ";
		result += std::wstring(sw.prefix.begin(), sw.prefix.end());
		result += std::wstring(sw.name.begin(), sw.name.end());

		// paired switch
		if (!sw.pair.empty()) {
			result += L", ";
			result += (sw.pair.length() > 1) ? std::wstring(L"--") : std::wstring(L"-");
			result += std::wstring(sw.pair.begin(), sw.pair.end());
		}

		// parameter indicator
		if (sw.hasParam) {
			result += L" <parameter>";
		}

		// description
		result += L"\n    ";
		result += std::wstring(sw.description.begin(), sw.description.end());
		result += L"\n";
	}
	return result;
}

std::wstring ArgumentParser::formatSwitchLineW(bool hasNonSwitchArguments) const {
	const std::wstring executableNameW(this->executableName.begin(), this->executableName.end());
	const size_t newLineSpace = (executableNameW.length() + 3);
	const std::wstring newLinePadding(newLineSpace, L' ');
	std::wstring result;
	for (size_t index = 0; index < this->switches.size(); ++index) {
		const auto& sw = this->switches[index];
		if (!sw.pair.empty() && sw.name > sw.pair) {
			continue;
		}
		result += L"[";
		result += std::wstring(sw.prefix.begin(), sw.prefix.end());
		result += std::wstring(sw.name.begin(), sw.name.end());
		if (!sw.pair.empty()) {
			result += L" | ";
			result += (sw.pair.length() > 1) ? std::wstring(L"--") : std::wstring(L"-");
			result += std::wstring(sw.pair.begin(), sw.pair.end());
		}
		if (sw.hasParam) {
			result += L" <parameter>";
		}
		result += L"]";
		if (index < (this->switches.size() - 2)) {
			result += L" |\n";
			result += newLinePadding;
		}
	}
	if (hasNonSwitchArguments) {
		if (!result.empty()) {
			result += L"\n";
			result += newLinePadding;
			result += L"<ARGUMENTS>";
		} else {
			result += L"<ARGUMENTS>";
		}
	}
	return result;
}