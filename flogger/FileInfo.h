#pragma once
namespace fileutils {
	class FileInfo
	{
	public:
		FileInfo(const std::filesystem::path& filepath);
		~FileInfo() = default;
		std::filesystem::path getFilePath() const;
		std::wstring getFileNameW() const;
		std::string getFileNameA() const;
		std::wstring getFileExtensionW() const;
		std::string getFileExtensionA() const;
		std::filesystem::path getDirectoryPath() const;
		bool exists() const;
		void setAppendMode(bool appendMode);
		bool createFile();
		bool writeFileW(const std::wstring& content) const;
		bool writeFileA(const std::string& content) const;
		bool isDirectory() const;
		bool isRegularFile() const;
		bool isUnknownType() const;
		bool operator==(const FileInfo& other) const;
	private:
		std::filesystem::path filePath;
		bool fileExists;
		bool append;
		enum class FileType {
			Unknown,
			Directory,
			RegularFile
		} fileType;
	};
}

