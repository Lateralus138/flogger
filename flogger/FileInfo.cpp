#include "pch.h"
#include "FileInfo.h"
namespace fileutils {
	FileInfo::FileInfo(const std::filesystem::path& filepath)
		: filePath(filepath), fileExists(false), append(false) {
		fileExists = std::filesystem::exists(filePath);
		if (fileExists) {
			if (std::filesystem::is_directory(filePath)) {
				fileType = FileType::Directory;
			}
			else if (std::filesystem::is_regular_file(filePath)) {
				fileType = FileType::RegularFile;
			}
			else {
				fileType = FileType::Unknown;
			}
		}
		else {
			fileType = FileType::Unknown;
		}
	}
	std::filesystem::path FileInfo::getFilePath() const {
		return filePath;
	}
	std::wstring FileInfo::getFileNameW() const {
		return filePath.filename().wstring();
	}
	std::string FileInfo::getFileNameA() const {
		return filePath.filename().string();
	}
	std::wstring FileInfo::getFileExtensionW() const {
		return filePath.extension().wstring();
	}
	std::string FileInfo::getFileExtensionA() const {
		return filePath.extension().string();
	}
	std::filesystem::path FileInfo::getDirectoryPath() const {
		return filePath.parent_path();
	}
	bool FileInfo::exists() const {
		return fileExists;
	}
	void FileInfo::setAppendMode(bool appendMode) {
		append = appendMode;
	}
	bool FileInfo::createFile() {
		if (!fileExists) {
			std::ofstream ofs(filePath);
			ofs.close();
			fileExists = std::filesystem::exists(filePath);
			if (fileExists) {
				fileType = FileType::RegularFile;
			}
		}
		return fileExists;
	}
	bool FileInfo::writeFileW(const std::wstring& content) const {
		if (fileType != FileType::RegularFile) {
			return false;
		}
		std::wofstream wofs;
		if (append) {
			wofs.open(filePath, std::ios::app);
		}
		else {
			wofs.open(filePath, std::ios::trunc);
		}
		if (!wofs.is_open()) {
			return false;
		}
		wofs << content;
		wofs.close();
		return true;
	}
	bool FileInfo::writeFileA(const std::string& content) const {
		if (fileType != FileType::RegularFile) {
			return false;
		}
		std::ofstream ofs;
		if (append) {
			ofs.open(filePath, std::ios::app);
		}
		else {
			ofs.open(filePath, std::ios::trunc);
		}
		if (!ofs.is_open()) {
			return false;
		}
		ofs << content;
		ofs.close();
		return true;
	}
	bool FileInfo::isDirectory() const {
		return fileType == FileType::Directory;
	}
	bool FileInfo::isRegularFile() const {
		return fileType == FileType::RegularFile;
	}
	bool FileInfo::isUnknownType() const {
		return fileType == FileType::Unknown;
	}
	bool FileInfo::operator==(const FileInfo& other) const {
		return filePath == other.filePath;
	}
}