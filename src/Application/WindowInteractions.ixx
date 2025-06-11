export module Application.WindowInteractions;

import Framework;

import <Windows.h>;
import std;

namespace Windows {
    export std::optional<std::string> OpenFileDialog() {
        char filename[MAX_PATH] = "";

        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFilter = "All Files\0*.*\0Text Files\0*.txt\0";
        ofn.lpstrFile = filename;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameA(&ofn)) {
            return std::string(filename);
        }
        return "";
    }

    export void ShowErrorMessage(const char* message, const char* title = "Error") {
        MessageBoxA(NULL, message, title, MB_OK | MB_ICONERROR);
    }
}