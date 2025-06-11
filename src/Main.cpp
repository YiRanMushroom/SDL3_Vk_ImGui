export module Main;

import Framework;
import Application.ImGuiWindows;
import Atomic;
import std;
import Application.WindowsInteractions;

Atomic<std::string> g_SourceFilePath;
Atomic<std::string> g_TargetOutputPath;
Atomic<std::string> g_compilerOutput;
Atomic<std::string> g_compilerErrorOutput;
bool g_CompileWindowOpen = true;
bool g_CompilerOutputWindowOpen = true;
bool g_CompilerErrorOutputWindowOpen = true;

std::filesystem::path currentPath = std::filesystem::current_path();

void HandleCompile();

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
                if (auto result = Windows::OpenFileDialog("PicoBlaze Source Files\0*.psm\0All Files\0*.*\0")) {
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
    std::string sourceFilePath = g_SourceFilePath.GetProxy().Get();
    std::string targetOutputPath = g_TargetOutputPath.GetProxy().Get();
    if (sourceFilePath.empty()) {
        Windows::ShowErrorMessage("Please select a source file.");
        return;
    }

    std::filesystem::path picoBlazePath = currentPath / "PicoBlaze";

    std::string command = "EasyASM -l " + picoBlazePath.string() + " -i " + sourceFilePath + ' ';

    if (!targetOutputPath.empty()) {
        command += "-o " + targetOutputPath + " ";
    }

    std::cout << "Running command: " << command << std::endl;

    auto output = Windows::RunProcessWithOutput(command);
    if (!output) {
        return;
    }
    auto [stdout, stderr] = std::move(output.value());
    if (!stdout.empty()) {
        g_compilerOutput.GetProxy().Set(std::move(stdout));
        std::cout << "Compiler Output: " << g_compilerOutput.GetProxy().Get() << std::endl;
    }
    if (!stderr.empty()) {
        g_compilerErrorOutput.GetProxy().Set(std::move(stderr));
        std::cerr << "Compiler Error Output: " << g_compilerErrorOutput.GetProxy().Get() << std::endl;
    }
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

export int main(int, char **) {
    Program program{ProgramSpec{
        .WindowTitle = "EasyASM-PicoBlaze Compiler"
    }};
    // Our state

    auto &io = ImGui::GetIO(); // (void)io; // You can also access ImGuiIO directly if you need to

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
