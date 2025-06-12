export module Main;

import Framework;
import Application.ImGuiWindows;
import Atomic;
import std;
import Application.WindowsInteractions;
import <windows.h>;

Atomic<std::string> g_SourceFilePath;
Atomic<std::string> g_TargetOutputPath;
Atomic<std::string> g_compilerOutput;
Atomic<std::string> g_compilerErrorOutput;
bool g_CompileWindowOpen = true;
bool g_CompilerOutputWindowOpen = true;
bool g_CompilerErrorOutputWindowOpen = true;

std::filesystem::path g_CurrentPath = std::filesystem::current_path();

void HandleCompile();

void AssertHasFile(const std::filesystem::path& resourcePath) {
    if (!std::filesystem::exists(resourcePath)) {
        Windows::ShowErrorMessage(
            ("File not found: " + resourcePath.string() + "\nThe compiler may not be able to run expectantly.\nPlease reinstall the newest version of EasyASM-PicoBlaze.").c_str(),
            "File Not Found"
        );
    }
}

void RenderCompileWindow() {
    ImGui::Begin("Compile", &g_CompileWindowOpen); {
        ImGui::Text("Source File: ");
        ImGui::SameLine();
        float availWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetNextItemWidth(availWidth - 50.0f); {
            auto sourceProxy = g_SourceFilePath.GetProxy();
            ImGui::InputText("##SourceFile", &sourceProxy.Get());
        }
        ImGui::SameLine();
        if (ImGui::Button("...##SourceFile")) {
            std::thread([&] {
                if (auto result = Windows::OpenFileDialog(L"PicoBlaze Source Files\0*.psm\0All Files\0*.*\0")) {
                    g_SourceFilePath.GetProxy().Set(std::move(result.value()));
                }
            }).detach();
        }

        ImGui::Text("Output Directory: ");
        ImGui::SameLine();
        availWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetNextItemWidth(availWidth - 50.0f); {
            auto targetProxy = g_TargetOutputPath.GetProxy();
            ImGui::InputText("##OutputDirectory", &targetProxy.Get());
        }
        ImGui::SameLine();
        if (ImGui::Button("...##OutputDirectory")) {
            std::thread([&] {
                if (auto result = Windows::OpenDirectoryDialog()) {
                    g_TargetOutputPath.GetProxy().Set(std::move(result.value()));
                }
            }).detach();
        }

        if (ImGui::Button("Compile")) {
            HandleCompile();
        }
    }
    ImGui::End();
}

void HandleCompile() {
    std::thread ([]() {
        std::string sourceFilePath = g_SourceFilePath.GetProxy().Get();
        std::string targetOutputPath = g_TargetOutputPath.GetProxy().Get();
        if (sourceFilePath.empty()) {
            Windows::ShowErrorMessage("Please select a source file.");
            return;
        }

        std::filesystem::path picoBlazePath = g_CurrentPath / "PicoBlaze";

        std::string command = std::string("EasyASM -l ") + "PicoBlaze" + " -i \"" + sourceFilePath + "\" ";

        if (!targetOutputPath.empty()) {
            command += "-o \"" + targetOutputPath + "\" ";
        }

        std::cout << "Running command: " << command << std::endl;

        auto output = Windows::RunProcessWithOutput(command);
        if (!output) {
            return;
        }
        auto [stdout, stderr] = std::move(output.value());
        if (!stdout.empty()) {
            g_compilerOutput.GetProxy().Set(std::move(stdout));
        } else {
            g_compilerOutput.GetProxy().Set("");
        }
        if (!stderr.empty()) {
            g_compilerErrorOutput.GetProxy().Set(std::move(stderr));
        } else {
            g_compilerErrorOutput.GetProxy().Set("");
        }
    }).detach();
}

void RenderCompilerOutputWindow() {
    ImGui::Begin("Compiler Output", &g_CompilerOutputWindowOpen);

    if (ImGui::Button("Clear##CompilerOutput")) {
        g_compilerOutput.GetProxy().Set("");
    }

    ImGui::TextWrapped("%s", g_compilerOutput.GetProxy().Get().c_str());

    ImGui::End();
}

void RenderCompilerErrorOutputWindow() {
    ImGui::Begin("Compiler Error Output", &g_CompilerErrorOutputWindowOpen);

    if (ImGui::Button("Clear##CompilerErrorOutput")) {
        g_compilerErrorOutput.GetProxy().Set("");
    }

    ImGui::TextWrapped("%s", g_compilerErrorOutput.GetProxy().Get().c_str());

    ImGui::End();
}

export int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    AssertHasFile("EasyASM.exe");
    AssertHasFile("PicoBlaze");
    AssertHasFile("OpenSans.ttf");
    AssertHasFile("NotoSansSC.ttf");
    Program program{
        ProgramSpec{
            .WindowTitle = "EasyASM-PicoBlaze Compiler"
        }
    };

    std::vector<std::pair<std::string, bool *>> windowNames{
        {"Compile", &g_CompileWindowOpen},
        {"Compiler Output", &g_CompilerOutputWindowOpen},
        {"Compiler Error Output", &g_CompilerErrorOutputWindowOpen}
    };

    program.ExecuteLoop(
        [&](SDL_Event) {
            RenderBackgroundSpace(windowNames);
            if (g_CompileWindowOpen) {
                RenderCompileWindow();
            }
            if (g_CompilerOutputWindowOpen) {
                RenderCompilerOutputWindow();
            }
            if (g_CompilerErrorOutputWindowOpen) {
                RenderCompilerErrorOutputWindow();
            }
        }
    );

    return 0;
}
