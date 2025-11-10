#pragma once
#include <iostream>
#include <map>
#include <filesystem>
#include <fstream>
#if defined(_WIN32)
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif
#include "ArgumentParser.h"
#include "FileInfo.h"
