//
// Created by Administrator on 6/13/2025.
//

export module Framework.Program;
export import "FrameworkCoreHeader.hpp";

export struct ProgramSpec {
    // SDL_InitFlags SDLInitFlags = SDL_INIT_VIDEO | SDL_INIT_GAMEPAD;
    // SDL_WindowFlags SDLWindowFlags =
    //         SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    const char *WindowTitle = "Dear ImGui glfw Vulkan example";
    int WindowWidth = 1280;
    int WindowHeight = 720;


    // void (*ImGuiSetStyleCallBack)() = StyleColorHazel;

    // std::function<void(SDL_Event)> AdditionalEventCallback = nullptr;
};

export class Program {
public:
    Program(ProgramSpec spec);

    ~Program();

    template<typename T>
    void ExecuteLoop(const T& callBack);

    GLFWwindow* GetGLFWWindow() const {
        return m_GLFWWindow;
    }

    GLFWwindow* m_GLFWWindow = nullptr;
    ImGui_ImplVulkanH_Window* m_Wd;
public:
    // clear color
    ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

template<typename T>
void Program::ExecuteLoop(const T &callBack) {
    // Main loop
    while (!glfwWindowShouldClose(m_GLFWWindow)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Resize swap chain?
        int fb_width, fb_height;
        glfwGetFramebufferSize(m_GLFWWindow, &fb_width, &fb_height);
        if (fb_width > 0 && fb_height > 0 && (g_SwapChainRebuild || g_MainWindowData.Width != fb_width || g_MainWindowData.Height != fb_height))
        {
            ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
            ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData, g_QueueFamily, g_Allocator, fb_width, fb_height, g_MinImageCount);
            g_MainWindowData.FrameIndex = 0;
            g_SwapChainRebuild = false;
        }
        if (glfwGetWindowAttrib(m_GLFWWindow, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        callBack();

        ImGui::Render();
        auto& io = ImGui::GetIO();
        ImDrawData* main_draw_data = ImGui::GetDrawData();
        const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
        m_Wd->ClearValue.color.float32[0] = ClearColor.x * ClearColor.w;
        m_Wd->ClearValue.color.float32[1] = ClearColor.y * ClearColor.w;
        m_Wd->ClearValue.color.float32[2] = ClearColor.z * ClearColor.w;
        m_Wd->ClearValue.color.float32[3] = ClearColor.w;
        if (!main_is_minimized)
            FrameRender(m_Wd, main_draw_data);

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // Present Main Platform Window
        if (!main_is_minimized)
            FramePresent(m_Wd);
    }
}
