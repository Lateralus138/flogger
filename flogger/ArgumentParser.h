#pragma once
#include <vector>
#include <string>
class ArgumentParser {
	// TODO Possible change internal storage to std::string_view to avoid copies
	// TODO Possible change to unordered_map for switches for faster lookup
	// TODO Possible add system to allow multiple switches with with one '-'
public:
	const static bool HASPARAM = true;
	const static bool NOPARAM = false;
	const static bool THROWIFNOTSET = true;
	const static bool NOTHROWIFNOTSET = false;
	ArgumentParser(int argc, char* argv[]);
	~ArgumentParser() = default;
	std::string getExecutableName() const;
	void setSwitchPair(std::string_view shortName, std::string_view longName);
	void addSwitch(std::string_view name, std::string_view description);
	void addSwitch(std::string_view name, std::string_view description, bool hasParam);
	void addSwitch(std::string_view name, std::string_view description, bool hasParam, bool throwIfNotSet);
	void addSwitchPair(std::string_view nameA, std::string_view nameB, std::string_view description, bool hasParam, bool throwIfNotSet);
	int parse();
	int getErrorCode() const;
	bool isSwitchSet(std::string_view name) const;
	// TODO Possible change printHelp header to std::string_view
	void printHelp(const std::string& header, bool hasNonSwitchArguments) const;
	void printHelpW(const std::wstring& header, bool hasNonSwitchArguments) const;
	std::string getSwitchValue(std::string_view name) const;
	std::string getArgvValue(size_t index) const;
	size_t getArgumentCount() const;
	const std::vector<std::string>& getArguments() const;

private:
	std::string executableName;
	int argc;
	int errorCode = 0;
	struct AddedSwitch {
		std::string name;
		std::string description;
		std::string prefix;
		std::string value;
		std::string pair;
		bool hasParam = false;
		bool isSet = false;
		bool throwIfNotSet = false;
		AddedSwitch(std::string_view name, std::string_view description, std::string_view prefix);
		AddedSwitch(std::string_view name, std::string_view description, std::string_view prefix, bool hasParam);
		AddedSwitch(std::string_view name, std::string_view description, std::string_view prefix, bool hasParam, bool throwIfNotSet);
	};
	std::vector<std::string> argv;
	std::vector<std::string> arguments;
	std::vector<AddedSwitch> switches;
	std::string formatSwitchList() const;
	std::string formatSwitchLine(bool hasNonSwitchArguments) const;

	// Wide-string versions for printHelpW
	std::wstring formatSwitchListW() const;
	std::wstring formatSwitchLineW(bool hasNonSwitchArguments) const;
};

