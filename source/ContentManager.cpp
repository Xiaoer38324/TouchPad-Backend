#include "../header/ContentManager.h"
// ContentManager.cpp
#include "../header/WindowOperation.h"
#include "../header/UI_Event_Resources.h"
#include"../header/test_MouseClick.h"
#include <GLFW/glfw3.h>
#include <imgui_impl_opengl3.h>
#include "../header/Package_Loder.h"
#include"../header/Material.h"
#include <imgui_impl_glfw.h>
#include"../header/stb_image_write.h"
#include "gl/GL.h"
#include"../header/EasyUIContent.h"
GLFWwindow* content_window;
ImVec4 content_clear_color = ImVec4(0, 1, 0, 0.0f);
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
inline void Content_InitWindow(ImVec2 window_size) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    content_window = glfwCreateWindow(window_size.x, window_size.y, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (content_window == nullptr)
        return;
    glfwMakeContextCurrent(content_window);
    glfwSwapInterval(1); // Enable vsync
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(content_window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(content_window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

}
inline void Content_ClearWindow() {
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(content_window);
    glfwTerminate();
}
inline bool Content_PreWindow(ImVec2 size, ImVec2 pos) {
    glfwPollEvents();
    if (glfwGetWindowAttrib(content_window, GLFW_ICONIFIED) != 0)
    {
        ImGui_ImplGlfw_Sleep(10);
        return false;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // 半透明黑色背景
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); // 无圆角
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); // 无边框
    ImGui::Begin("Transparent Background Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize("Transparent Background Window", size);
    //ImGui::SetWindowPos("Transparent Background Window",pos);
    return true;
}
inline void Content_AfWindow() {
    ImGui::End();
    ImGui::PopStyleVar(2); // 恢复样式
    ImGui::PopStyleColor(); // 恢复颜色
    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(content_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(content_clear_color.x * content_clear_color.w, content_clear_color.y * content_clear_color.w, content_clear_color.z * content_clear_color.w, content_clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(content_window);
}
ContentManager& ContentManager::getInstance() {
    static ContentManager instance;
    return instance;
}
ContentManager::~ContentManager() {
    CleanupContents();
}
void ContentManager::Run(std::unique_ptr<InputInstance> input,
    const std::wstring& window_title, const std::filesystem::path& page,
    std::string initial_ui,const bool& covered)
{
    global_input_ = std::move(input);
    int width, height;
    // 加载初始内容
    this->package = page;
    auto& package_loader = Package_Loader::getInstance();
    // 加载资源包
    if (!package_loader.Loader(package)) {
        throw std::runtime_error("Failed to load package: " + package.string());
    }
    nlohmann::json Info = Package_Loader::getInstance().I();
    if (initial_ui == "") {
        if (!Info.contains("initial_ui")) {
            throw std::runtime_error("Failed to load package: No Enter Ui");
        };
        initial_ui = Info["initial_ui"].get<std::string>();
    }
    if (!Info.contains("width")) {
        throw std::runtime_error("Failed to load package: No Width of Window Info");
    }
    width = Info["width"].get<int>();
    if (!Info.contains("height")) {
        throw std::runtime_error("Failed to load package: No Height of Window Info");
    }
    height = Info["height"].get<int>();
    auto& window_op = WindowOperation::getInstance();
    window_op.SetSizeForInit(ImVec2(width, height));
    Content_InitWindow(WindowOperation::getInstance().GetSize());
    window_op.Init();
    window_op.SetTargetWindow(window_title);
    SwitchContent(initial_ui);
    if (covered) {
        WindowOperation::getInstance().AilgnWindow();
        WindowOperation::getInstance().SetCoveredWindow();
    }
    // 主循环
    while (!glfwWindowShouldClose(content_window)) {
        if (Content_PreWindow(window_op.GetSize(), window_op.GetPos())) {
            current_euc_->Update();
            Content_AfWindow();
        }
    }

    CleanupContents();
    Content_ClearWindow();
}
bool ContentManager::TestRun(std::unique_ptr<InputInstance>& input, const std::filesystem::path& page, std::vector<std::string> initial_ui,int test_frame)
{
    if (initial_ui.size() == 0) {
        return false;
    }

    global_input_ = std::move(input);
    // 加载初始内容
    this->package = page;
    auto& package_loader = Package_Loader::getInstance();
    int width, height;
    // 加载资源包
    if (!package_loader.Loader(package)) {
        throw std::runtime_error("Failed to load package: " + package.string());
    }
    nlohmann::json Info = Package_Loader::getInstance().I();
    if (!Info.contains("width")) {
        throw std::runtime_error("Failed to load package: No Width of Window Info");
    }
    width = Info["width"].get<int>();
    if (!Info.contains("height")) {
        throw std::runtime_error("Failed to load package: No Height of Window Info");
    }
    height = Info["height"].get<int>();
    auto& window_op = WindowOperation::getInstance();
    window_op.SetSizeForInit(ImVec2(width, height));
    Content_InitWindow(WindowOperation::getInstance().GetSize());
    
    int i = 0;
    SwitchContent(initial_ui[i]);
    std::cout << "Testing: " << initial_ui[i] << std::endl;
    int current_frame = test_frame;
    // 主循环
    while (!glfwWindowShouldClose(content_window)) {
        if (current_frame<0){
            if (i < initial_ui.size() - 1) {
                SwitchContent(initial_ui[++i]);
                std::cout << "Testing: " << initial_ui[i] << std::endl;
                current_frame = test_frame;
            }
            else {
                break;
            }
            
        }
        if (Content_PreWindow(window_op.GetSize(), window_op.GetPos())) {
            current_euc_->Update();
            Content_AfWindow();
        }
        current_frame--;
    }

    CleanupContents();
    return false;
}
void ContentManager::SwitchContent(const std::string& new_ui) {
    
    if (current_ui_ == new_ui) return;

    // 离开当前内容
    if (current_euc_) {
        current_euc_->Leave();
    }

    // 加载或切换内容
    if (contents_.find(new_ui) == contents_.end()) {
        LoadContent(new_ui, package.string());
    }

    // 进入新内容
    current_ui_ = new_ui;
    current_euc_ = contents_[new_ui].get();
    current_euc_->Enter();
}
void ContentManager::LoadContent(const std::string& ui_name,const std::string& package) {
    auto& package_loader = Package_Loader::getInstance();

    // 加载资源包
    if (!package_loader.Loader(package)) {
        throw std::runtime_error("Failed to load package: " + ui_name);
    }

    // 反序列化资源
    UI_Event_Resources::getInstance().Deserialization(package_loader.E());
    Material_Resources::getInstance().Deserialization(package_loader.T());

    // 创建内容实例
    auto& content = contents_[ui_name];
    content = std::make_unique<PlayingUIContent>(global_input_.get());
    int member_begin = WindowOperation::getInstance().GetMemberSize();//记录下当前UI监听器的位置
    content->Deserialization(package_loader.GetU(ui_name));
    // 初始化内容
    content->Init();
    package_loader.Done();
    WindowOperation::getInstance().SetOriginWindowPositionAndSize(member_begin);//从监听位置开始往后更新，就是新添加的
}
void ContentManager::CleanupContents() {
    for (auto& [name, content] : contents_) {
        content->Quit();
    }
    contents_.clear();
    current_euc_ = nullptr;
}
