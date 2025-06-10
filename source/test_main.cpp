#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include"../header/test_MouseClick.h";
#include "../header/EasyUIContent.h"
#include "../header/Script/Button.h"
#include <fstream>
GLFWwindow* window;
ImVec4 clear_color = ImVec4(0, 1, 0, 0.0f);
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
inline void InitWindow(ImVec2 window_size) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return ;
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
    
    window = glfwCreateWindow(window_size.x, window_size.y, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return ;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

}
inline void ClearWindow() {
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}
inline bool PreWindow(ImVec2 size,ImVec2 pos) {
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
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
    ImGui::SetWindowSize("Transparent Background Window",size);
    //ImGui::SetWindowPos("Transparent Background Window",pos);
    return true;
}
inline void AfWindow() {
    ImGui::End();
    ImGui::PopStyleVar(2); // 恢复样式
    ImGui::PopStyleColor(); // 恢复颜色
    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}
#include"../header/Component_Includer.h"
#include "../header/Script/Joy.h"
#include "../header/Event_Int_Interpreter.h"
#include "../header/UI_Event_Resources.h"
#include "../header/Package_Loder.h"
#include "../header/OptionsParser.h"
#include "../header/Package_Saver.h"
//#include"UIManager.h"
inline void AddOne(std::string ma, PlayingUIContent& euc, std::string name, float x, float y, float w, float h, unsigned short p = 0, bool accept = true, std::vector<bool> state = {false,false,false,false}) {
    ButtonWithMask::Params bp{
        ma,x,y,w,h,state
    };
    LogicParams* cm = Button::GetParam();
    Button::Params* cnm = dynamic_cast<Button::Params*>(cm);
    cnm->name = name;
    cnm->params = bp;
    cnm->priority = p;
    std::string fuck2("Button");
    UIManager::CreateLogic(&euc, fuck2, cnm);
}
inline void AddTC(PlayingUIContent& euc, std::string name, float x, float y, float w, float h, bool accept = true) {
    Button_Empty::Params bp{
        x,y,w,h
    };
    LogicParams* cm = clickThrough::GetParam();
    clickThrough::Params* cnm = dynamic_cast<clickThrough::Params*>(cm);
    cnm->name = name;
    cnm->params = bp;
    cnm->priority = 0;
    std::string fuck2("clickThrough");
    UIManager::CreateLogic(&euc, fuck2, cnm);
}
inline void AddJOyOne(std::string b, std::string c, PlayingUIContent& euc, std::string name, float x, float y, float w, float h,float r) {
    ButtonWithMask::Params bp{
        b,x,y,w,h
    };
    ButtonWithMask::Params bp2{
        c,x,y,w,h
    };
    LogicParams* cm = Joy::GetParam();
    Joy::Params* cnm = dynamic_cast<Joy::Params*>(cm);
    cnm->name = name;
    cnm->background = bp;
    cnm->point = bp2;
    cnm->circle_r = r;
    cnm->rate = 0.3f;
    cnm->priority = 1;
    std::string fuck2("Joy");
    UIManager::CreateLogic(&euc, fuck2, cm);
}
int main1(){
    //初始化管理器。
    //UIManager::RegisterUI<ButtonWithMask>("ButtonWithMask");
    //UIManager::RegisterLogic<CircleMove2>("CircleMove2");
    WindowOperation::getInstance().SetSizeForInit(ImVec2(860, 540));
    InitWindow(WindowOperation::getInstance().GetSize());
    //在这里准备UI数据。
    testMouseClick tmc;
    std::wstring title(L"Brotato");
    WindowOperation::getInstance().Init();
    WindowOperation::getInstance().SetTargetWindow(title);
    PlayingUIContent euc(&tmc);

    Package_Loader::getInstance().Loader("page.zip");
    //处理事件
    Event_Int_Interpreter* eii=new Event_Int_Interpreter();
    eii->AddOne(0, EmptyConnector());
    eii->AddOne(1, ContentConnector("U2"));
    eii->AddOne(2, EmptyConnector());
    nlohmann::json test_json = eii->Serialization();
    delete eii;
    UI_Event_Resources::getInstance().Add("MF1", test_json);
     eii = new Event_Int_Interpreter();
    eii->AddOne(0, EmptyConnector());
    eii->AddOne(1, ContentConnector("U"));
    eii->AddOne(2, EmptyConnector());
    nlohmann::json test_json3 = eii->Serialization();
    delete eii;
    UI_Event_Resources::getInstance().Add("MFX", test_json3);
 /*   eii = new Event_Int_Interpreter();
    eii->AddOne(0, MouseKeyConnector());
    auto test_json2 = eii->Serialization();
    delete eii;
    UI_Event_Resources::getInstance().Add("TC", test_json2);*/
    //处理贴图
    std::string ma("jj.png"), y("yellow.png"), b("blue.png");
    MaterialInfo red(ma, MaskGenerType::ALPHA, 128);
    MaterialInfo blue(b, MaskGenerType::ALPHA, 128);
    MaterialInfo yellow(y, MaskGenerType::ALPHA, 128);
    Material_Resources::getInstance().Add(red);
    Material_Resources::getInstance().Add(yellow);
    Material_Resources::getInstance().Add(blue);
    //添加组件
   // AddOne(ma, euc, "MFX", 0, 0, 250, 250,0);
   // AddOne(y, euc, "MF2", 125, 0, 250, 250,1);
   //// AddTC(euc, "TC", 0, 0, 500, 500);
   //  AddOne(ma, euc, "MF2", 70, 90, 50, 50);
   // AddOne(ma, euc, "MF3", 110, 136, 50, 50);
   // AddOne(ma, euc, "MF4", 210, 440, 50, 50);
   // AddOne(ma, euc, "MF5", 250, 250, 50, 50);
   // AddOne(ma, euc, "MF6", 310, 60, 50, 50);
   // AddOne(ma, euc, "MF7", 225, 450, 50, 50);
   // AddOne(ma, euc, "MF8", 450, 315, 50, 50);
    //AddOne(ma, euc, "MF9", 450, 450, 50, 50,false);
    //AddJOyOne(b,y, euc, "J1", 250, 250, 200, 200);
    //std::string value = "";
    //std::ifstream file;
    //file.open("layout.txt", std::ios::out); // 以输出模式打开文件
    //file >>value ;
    //file.close();
    //nlohmann::json j= nlohmann::json::parse(value);
    //UI_Event_Resources::getInstance().Deserialization(j[1]);
    //Material_Resources::getInstance().Deserialization(j[2]);
    //euc.Deserialization(j[0]);
    /*Package_Loader::getInstance().Loader("page.zip");
    std::cout << Package_Loader::getInstance().E().dump() << std::endl;
    std::cout << Package_Loader::getInstance().T().dump() << std::endl;
    std::cout << Package_Loader::getInstance().U().dump() << std::endl;
    UI_Event_Resources::getInstance().Deserialization(Package_Loader::getInstance().E());
    Material_Resources::getInstance().Deserialization(Package_Loader::getInstance().T());
    euc.Deserialization(Package_Loader::getInstance().U());*/
    euc.Init();
    Package_Loader::getInstance().Done();
    euc.Enter();
    //ImageTest it= QuickAdd(db,op,message, input, NP, 400, 0, 100, 100, "./jj.png", "fuck1", 0);
    /*ImageTest it2=QuickAdd(message, input, NP, 200, 50, 100, 100, "./jj.png", "fuck2", 1);
    //ImageTest it3 = QuickAdd(message, input, NP, 250, 250, 200, 200, "./jj.png", "fuck3", 2);*/
    //ImageTest it4 = QuickAdd(db,op,message, input, NP, 0, 400, 100, 100, "./jj.png", "fuck4", 1);
    /*std::thread a([] {
        Sleep(1000);
        opp->SetWindowPositionAndSize(0, 0, 800, 800);
        window_size.x = 800;
        window_size.y = 800;
        });
    a.detach();*/
    //WindowOperation::getInstance().AilgnWindow();
    WindowOperation::getInstance().SetCoveredWindow();

    while (!glfwWindowShouldClose(window))
    {
        if (PreWindow(WindowOperation::getInstance().GetSize(),WindowOperation::getInstance().GetPos())) {
            //TODO输入处理.
            euc.Update();
            AfWindow();
       }
       
    }
    /*nlohmann::json j;
    j.push_back(euc.Serialization());
    j.push_back(UI_Event_Resources::getInstance().Serialization());
    j.push_back(Material_Resources::getInstance().Serialization());
    std::cout << j.dump();*/
    ClearWindow();
    //auto value = euc.Serialization();
    //std::ofstream file;
    //file.open("./page/U2", std::ios::out); // 以输出模式打开文件
    //file << value;
    //file.close();
    //
    //value = UI_Event_Resources::getInstance().Serialization();
    //std::ofstream file2;
    //file2.open("./page/E", std::ios::out); // 以输出模式打开文件
    //file2 << value;
    //file2.close();
    ////
    //value = Material_Resources::getInstance().Serialization();
    //std::ofstream file3;
    //file3.open("./page/T", std::ios::out); // 以输出模式打开文件
    //file3 << value;
    //file3.close();
    euc.Leave();
    euc.Quit();
    return 0;
}
int main2(int argc, char* argv[]) {
    std::unique_ptr<testMouseClick> tmc=std::make_unique<testMouseClick>();
    std::unique_ptr<InputInstance> ii=std::move(tmc);
    ContentManager::getInstance().Run(std::move(ii), L"Brotato","./page.zip");
    return 0;
}
#include <zip.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>
namespace fs = std::filesystem;
void add_folder_to_zip_and_close(const std::string& folder_path, const std::string& zip_path) {
    int err = 0;
    zip_t* zip = zip_open(zip_path.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
    if (!zip) {
        std::cerr << "无法创建zip文件: " << zip_path << std::endl;
        return;
    }

    fs::path folder(folder_path);
    std::vector<std::vector<char>*> buffer_ptrs;  // 局部变量：保存所有动态分配的 buffer

    for (const auto& entry : fs::recursive_directory_iterator(folder)) {
        if (fs::is_directory(entry.path())) continue;

        fs::path rel_path = fs::relative(entry.path(), folder);

        std::ifstream file(entry.path(), std::ios::binary);
        if (!file) {
            std::cerr << "无法打开文件: " << entry.path() << std::endl;
            continue;
        }

        // 动态分配 buffer
        auto* buffer = new std::vector<char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        const void* data_ptr = buffer->empty() ? "" : buffer->data();
        zip_source_t* source = zip_source_buffer(zip, data_ptr, buffer->size(), 0); // 我们自己托管
        if (!source) {
            std::cerr << "无法创建zip source: " << entry.path() << std::endl;
            delete buffer;
            continue;
        }

        if (zip_file_add(zip, rel_path.generic_string().c_str(), source,
            ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8) < 0) {
            std::cerr << "无法添加文件到zip: " << rel_path << std::endl;
            zip_source_free(source);
            delete buffer;
        }
        else {
            buffer_ptrs.push_back(buffer); // 成功才托管
        }
    }

    // 关闭 zip 文件
    if (zip_close(zip) < 0) {
        std::cerr << "zip_close 失败: " << zip_strerror(zip) << std::endl;
    }

    // 释放所有 buffer
    for (auto* ptr : buffer_ptrs) {
        delete ptr;
    }

    std::cout << "压缩完成：" << zip_path << std::endl;
}


int main() {
    WindowOperation::getInstance().SetSizeForInit(ImVec2(860, 540));
    InitWindow(WindowOperation::getInstance().GetSize());
    //在这里准备UI数据。
    testMouseClick tmc;
    std::wstring title(L"Brotato");
    WindowOperation::getInstance().Init();
    WindowOperation::getInstance().SetTargetWindow(title);
    PlayingUIContent euc(&tmc);
    std::string y("j.png"), b("b.png");
    MaterialInfo bg(b, MaskGenerType::ALPHA, 128);
    MaterialInfo jo(y, MaskGenerType::ALPHA, 128);
    Material_Resources::getInstance().Add(bg);
    Material_Resources::getInstance().Add(jo);
    nlohmann::json value = Material_Resources::getInstance().Serialization();
    std::ofstream file3;
    file3.open("./Brotato/T", std::ios::out); // 以输出模式打开文件
    file3 << value;
    file3.close();
    Event_Range_Interpreter eri;
    eri.setCyclicRange(0, 360);
    eri.Add(300,60,4,KeyConnector('D', false, false, false));
    eri.Add(30, 150,5, KeyConnector('S', false, false, false));
    eri.Add(120, 240,6, KeyConnector('A', false, false, false));
    eri.Add(210, 330,7, KeyConnector('W', false, false, false));
    UI_Event_Resources::getInstance().Add("move", eri.Serialization(), true);
    Event_Int_Interpreter eii;
    eii.AddOne(0, MouseKeyConnector());
    UI_Event_Resources::getInstance().Add("tc", eii.Serialization());
    value = UI_Event_Resources::getInstance().Serialization();
    file3.open("./Brotato/E", std::ios::out); // 以输出模式打开文件
    file3 << value;
    file3.close();
    add_folder_to_zip_and_close("./Brotato", "Brotato.zip");
    //优先度只在正常载入的时候用。
    AddTC(euc, "tc", 0, 0, 860, 540);
    AddJOyOne(b, y, euc, "move", 10, 340, 200, 200,80);
    Package_Loader::getInstance().Loader("Brotato.zip",true);//Loader得记得清空，及时释放。
    euc.Init();
    Package_Loader::getInstance().Done();
    euc.Enter();
    WindowOperation::getInstance().AilgnWindow();
    WindowOperation::getInstance().SetCoveredWindow();
    while (!glfwWindowShouldClose(window))
    {
        if (PreWindow(WindowOperation::getInstance().GetSize(), WindowOperation::getInstance().GetPos())) {
            //TODO输入处理.
            euc.Update();
            AfWindow();
        }

    }
    ClearWindow();
    euc.Leave();
    euc.Quit();
    return 0;
}

int main_limbo() {
    WindowOperation::getInstance().SetSizeForInit(ImVec2(860, 540));
    InitWindow(WindowOperation::getInstance().GetSize());
    //在这里准备UI数据。
    testMouseClick tmc;
    std::wstring title(L"Limbo");
    WindowOperation::getInstance().Init();
    WindowOperation::getInstance().SetTargetWindow(title);
    PlayingUIContent euc(&tmc);
    std::string l("l.png"), r("r.png"), down("down.png"),mask("mask.jpg"),up("up.png"),c("C.png");
    MaterialInfo left(l, MaskGenerType::NONE,128);
    MaterialInfo right(r, MaskGenerType::NONE, 128);
    MaterialInfo _down(down, MaskGenerType::ANOTHER,128,mask);
    MaterialInfo _up(up, MaskGenerType::ANOTHER, 128,mask);
    MaterialInfo grab(c, MaskGenerType::ANOTHER, 128,mask);
    Material_Resources::getInstance().Add(left);
    Material_Resources::getInstance().Add(right);
    Material_Resources::getInstance().Add(_down);
    Material_Resources::getInstance().Add(_up);
    Material_Resources::getInstance().Add(grab);
    nlohmann::json value = Material_Resources::getInstance().Serialization();
    std::ofstream file3;
    file3.open("./limbo/T", std::ios::out); // 以输出模式打开文件
    file3 << value;
    file3.close();

    KeyConnector go_right_down(VK_LEFT,false, false, false,ButtonState::Down);
    KeyConnector go_right_up(VK_LEFT, false, false, false, ButtonState::Up);

    KeyConnector go_left_down(VK_RIGHT, false, false, false, ButtonState::Down);
    KeyConnector go_left_up(VK_RIGHT, false, false, false, ButtonState::Up);

    KeyConnector go_up_up(VK_UP, false, false, false, ButtonState::Up);
    KeyConnector go_up_down(VK_UP, false, false, false, ButtonState::Down);

    KeyConnector go_down_down(VK_DOWN, false, false, false, ButtonState::Down);
    KeyConnector go_down_up(VK_DOWN, false, false, false, ButtonState::Up);

    KeyConnector go_ctrl_down(VK_CONTROL, false, false, false, ButtonState::Down);
    KeyConnector go_ctrl_up(VK_CONTROL, false, false, false, ButtonState::Up);

    Event_Int_Interpreter do_right;
    do_right.AddOne(1, go_right_down);
    do_right.AddOne(2, go_right_up);

    Event_Int_Interpreter do_left;
    do_left.AddOne(1, go_left_down);
    do_left.AddOne(2, go_left_up);

    Event_Int_Interpreter do_up;
    do_up.AddOne(0, go_up_down);
    do_up.AddOne(1, go_up_down);
    do_up.AddOne(2, go_up_up);
    do_up.AddOne(3, go_up_up);
    Event_Int_Interpreter do_down;
    do_down.AddOne(0, go_down_down);
    do_down.AddOne(1, go_down_down);
    do_down.AddOne(2, go_down_up);

    Event_Int_Interpreter do_ctrl;
    do_ctrl.AddOne(0, go_ctrl_down);
    do_ctrl.AddOne(1, go_ctrl_down);
    do_ctrl.AddOne(2, go_ctrl_up);

    UI_Event_Resources::getInstance().Add("left", do_left.Serialization(), true);
    UI_Event_Resources::getInstance().Add("right", do_right.Serialization(), true);
    UI_Event_Resources::getInstance().Add("down", do_down.Serialization(), true);
    UI_Event_Resources::getInstance().Add("up", do_up.Serialization(), true);
    UI_Event_Resources::getInstance().Add("ctrl", do_ctrl.Serialization(), true);

    Event_Int_Interpreter eii;
    eii.AddOne(0, MouseKeyConnector());
    UI_Event_Resources::getInstance().Add("tc", eii.Serialization());

    value = UI_Event_Resources::getInstance().Serialization();
    file3.open("./limbo/E", std::ios::out); // 以输出模式打开文件
    file3 << value;
    file3.close();

    add_folder_to_zip_and_close("./limbo", "limbo.zip");
    //优先度只在正常载入的时候用。
    AddTC(euc, "tc", 0, 0, 860, 540);
    AddOne(r, euc, "right", 50, 400, 80, 80, 1,true, {false,false,false,false});
    AddOne(l, euc, "left", 160, 400, 80, 80, 1,true, {false,false,false,false });
    AddOne(c, euc, "ctrl", 700, 400, 80, 80, 1, true, { true,false,false,false });
    AddOne(up, euc, "up", 750, 300, 80, 80, 1, true, { true,false,false,false });
    AddOne(down, euc, "down", 800, 400, 50, 50, 1, true, { true,false,false,false });

    Package_Loader::getInstance().Loader("limbo.zip", true);//Loader得记得清空，及时释放。
    euc.Init();
    Package_Loader::getInstance().Done();
    euc.Enter();
    WindowOperation::getInstance().AilgnWindow();
    WindowOperation::getInstance().SetCoveredWindow();
    while (!glfwWindowShouldClose(window))
    {
        if (PreWindow(WindowOperation::getInstance().GetSize(), WindowOperation::getInstance().GetPos())) {
            //TODO输入处理.
            euc.Update();
            AfWindow();
        }

    }
    ClearWindow();
    euc.Leave();
    euc.Quit();
    return 0;
}
