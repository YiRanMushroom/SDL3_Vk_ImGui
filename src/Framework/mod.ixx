export module Framework;

export import "CoreHeader.hpp";
export import std.compat;

export struct ProgramSpec {
    SDL_InitFlags SDLInitFlags = SDL_INIT_VIDEO | SDL_INIT_GAMEPAD;
    SDL_WindowFlags SDLWindowFlags =
            SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    const char *WindowTitle = "Dear ImGui SDL3+Vulkan example";
    int WindowWidth = 1280;
    int WindowHeight = 720;

    void (*ImGuiSetStyleCallBack)() = ImGui::StyleColorHazel;

    std::function<void(SDL_Event)> AdditionalEventCallback = nullptr;
};

export class Program {
public:
    Program(ProgramSpec spec);

    ~Program();

    void ExecuteLoop(const std::function<void(SDL_Event)> &callBack);

    SDL_Window *GetSDLWindow() const;

private:
    SDL_Window *m_SDLWindow = nullptr;

public:
    bool IsDone = false;
};

