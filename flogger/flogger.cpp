#include "pch.h"
#include "flogger.h"
#include <ctime>
std::tm flogger::safe_localtime(const std::time_t &t) {
  std::tm result{};
#if defined(_WIN32)
  localtime_s(&result, &t);
#else
  localtime_r(&t, &result);
#endif
  return result;
}
std::string flogger::timestampA() {
  std::time_t now = std::time(nullptr);
  char buffer[20];
  std::tm tm = safe_localtime(now);
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
  return std::string(buffer);
}
std::wstring flogger::timestampW() {
  std::time_t now = std::time(nullptr);
  wchar_t buffer[20];
  std::tm tm = safe_localtime(now);
  std::wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S",
                &tm);
  return std::wstring(buffer);
}
static void buildArgumentParser(ArgumentParser &parser) {
  parser.addSwitchPair("h", "help", "Display this help message.",
                       ArgumentParser::NOPARAM,
                       ArgumentParser::NOTHROWIFNOTSET);
  parser.addSwitchPair("p", "path", "Log file path. Defaults to '.'.",
                       ArgumentParser::HASPARAM, ArgumentParser::THROWIFNOTSET);
  parser.addSwitchPair("n", "name",
                       "Log file name. Defaults to 'logger.<EXT>'.",
                       ArgumentParser::HASPARAM, ArgumentParser::THROWIFNOTSET);
  parser.addSwitchPair("e", "extension",
                       "Log file extension. Defaults to 'log'.",
                       ArgumentParser::HASPARAM, ArgumentParser::THROWIFNOTSET);
  parser.addSwitchPair(
      "o", "overwrite", "Overwrite existing log file. Defaults to append.",
      ArgumentParser::NOPARAM, ArgumentParser::NOTHROWIFNOTSET);
  parser.addSwitchPair(
      "t", "timestamp", "Prefix each log entry with a timestamp.",
      ArgumentParser::NOPARAM, ArgumentParser::NOTHROWIFNOTSET);
}
std::map<const int, std::wstring_view> errorMessages = {
    {0, L"No error."},
    {1, L"No arguments passed to the program."},
    {2, L"Failed to create log file."},
    {3, L"Failed to write to log file: "},
    {255, L"Error in parsing parameters. No parameter was provided."}};
int main(int argc, char *argv[]) {
#if defined(_WIN32)
  const int mode_result = _setmode(_fileno(stdout), _O_U16TEXT);
  if (mode_result == -1) {
    std::wstring errorMessage =
        L"Failed to set stdout mode to UTF-16. _setmode returned "
        "\nSome Unicode characters may fail.";
    std::wcerr << errorMessage << std::endl;
  }
#endif
  ArgumentParser argument_parser(argc, argv);
  buildArgumentParser(argument_parser);
  const int arg_exit_code = argument_parser.parse();
  const int errorCode = argument_parser.getErrorCode();
  if (errorCode > 0) {
    std::wcerr << L"Error: " << errorMessages[errorCode] << L'\n';
    return errorCode;
  }
  if (argument_parser.isSwitchSet("h")) {
    std::wstring wmessage = L"File Logger"
                            "\n  Log anything to a file with options.";
    argument_parser.printHelpW(wmessage, true);
    return 0;
  }
  auto &strings = argument_parser.getArguments();
  if (strings.size() == 0) {
    std::wcerr << errorMessages[1] << std::endl;
    return 1;
  }
  fileutils::FileInfo logFile(std::filesystem::path(
      std::filesystem::path(argument_parser.isSwitchSet("p")
                                ? argument_parser.getSwitchValue("p")
                                : ".") /
      std::filesystem::path(
          argument_parser.isSwitchSet("n")
              ? ((argument_parser.getSwitchValue("n") + '.') +
                 (argument_parser.isSwitchSet("e")
                      ? argument_parser.getSwitchValue("e")
                      : "log"))
              : ("logger." + (argument_parser.isSwitchSet("e")
                                  ? argument_parser.getSwitchValue("e")
                                  : "log")))));
  logFile.setAppendMode(!argument_parser.isSwitchSet("o"));
  if (!logFile.exists()) {
    const bool wasCreated = logFile.createFile();
    if (!wasCreated) {
      std::wcerr << errorMessages[2] << std::endl;
      return 2;
    }
  }
  bool writeStatus = false;
  for (auto &value : strings) {
    const std::wstring wvalue(value.begin(), value.end());
    if (argument_parser.isSwitchSet("t")) {
      const std::wstring timestamp = flogger::timestampW();
      writeStatus =
          logFile.writeFileW(L"[" + timestamp + L"] " + wvalue + L"\n");
      continue;
    }
    writeStatus = logFile.writeFileW(wvalue + L"\n");
    if (!writeStatus) {
      std::wcerr << errorMessages[3] << logFile.getFilePath().wstring()
                 << std::endl;
    }
  }
}
