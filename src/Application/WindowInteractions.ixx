export module Application.WindowsInteractions;

import Framework;

import <Windows.h>;
import <shlobj.h>;
import std;

namespace Windows {
    export std::optional<std::string> OpenFileDialog(std::string_view filter = "All Files\0*.*\0") {
        char filename[MAX_PATH] = "";

        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFilter = filter.data();
        ofn.lpstrFile = filename;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameA(&ofn)) {
            return std::string(filename);
        }
        return {};
    }

    export std::optional<std::string> OpenDirectoryDialog() {
        char path[MAX_PATH] = "";

        BROWSEINFOA bi = {0};
        bi.lpszTitle = "Select a folder";
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);

        if (pidl && SHGetPathFromIDListA(pidl, path)) {
            CoTaskMemFree(pidl);
            return std::string(path);
        }
        if (pidl) CoTaskMemFree(pidl);
        return {};
    }

    export void ShowErrorMessage(const char *message, const char *title = "Error") {
        MessageBoxA(NULL, message, title, MB_OK | MB_ICONERROR);
    }

    export struct ProcessOutput {
        std::string stdout_str;
        std::string stderr_str;
    };

    export std::optional<ProcessOutput> RunProcessWithOutput(const std::string &cmd) {
        HANDLE outRead, outWrite, errRead, errWrite;
        SECURITY_ATTRIBUTES sa{sizeof(sa), NULL, TRUE};

        if (!CreatePipe(&outRead, &outWrite, &sa, 0)) return std::nullopt;
        if (!CreatePipe(&errRead, &errWrite, &sa, 0)) {
            CloseHandle(outRead);
            CloseHandle(outWrite);
            return std::nullopt;
        }

        STARTUPINFOA si = {sizeof(si)};
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = outWrite;
        si.hStdError = errWrite;
        si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

        PROCESS_INFORMATION pi{};
        BOOL success = CreateProcessA(
            NULL, (LPSTR) cmd.c_str(), NULL, NULL, TRUE,
            CREATE_NO_WINDOW, NULL, NULL, &si, &pi
        );

        CloseHandle(outWrite);
        CloseHandle(errWrite);

        if (!success) {
            CloseHandle(outRead);
            CloseHandle(errRead);
            ShowErrorMessage("Failed to create process, please make sure the compiler and sources are correct.");
            return std::nullopt;
        }

        std::string outStr, errStr;
        char buf[4096];
        DWORD read;

        while (ReadFile(outRead, buf, sizeof(buf), &read, NULL) && read)
            outStr.append(buf, read);

        while (ReadFile(errRead, buf, sizeof(buf), &read, NULL) && read)
            errStr.append(buf, read);

        CloseHandle(outRead);
        CloseHandle(errRead);

        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD exitCode = 0;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (exitCode != 0) {
            ShowErrorMessage(("Process exited abnormally. Exit code: " + std::to_string(exitCode) + ". Please report this issue to the compiler vendor with the source it compiled.").c_str());
        }

        return ProcessOutput{std::move(outStr), std::move(errStr)};
    }
}
