// nothing much the files will open on default open defined in the system // could include special as open in nvim and
// other ....
#include "file_opener.hpp"

#include <cstdlib>
#include <filesystem>
#include <string>

#ifdef _WIN32 // to make it cross platform only windows need it cause linux and mac will work on default open and
              // xdg-open
#include <shellapi.h>
#include <windows.h>
#endif

bool openfile(std::filesystem::path path) {
#ifdef _WIN32
    return (INT_PTR)ShellExecuteW(nullptr, L"open", path.wstring(), c_str(), nullptr, nullptr, SW_SHOWNORMAL) >
           32; // apprantly this windows api call return >32 if it was success but in format HINSTANCE
#elif defined(__APPLE__)
    return std::system(("open " + path.string()).c_str()) == 0;
#else
    return std::system(("xdg-open " + path.string() + " 2> /dev/null").c_str()) == 0;
#endif
}
