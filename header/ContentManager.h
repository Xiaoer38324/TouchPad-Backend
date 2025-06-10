#pragma once
// ContentManager.h
#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include<filesystem>
#define BACKTOORIGIN "BaCkToOrIgIn"
class InputInstance;
class PlayingUIContent;
class WindowOperation;

class ContentManager {
public:
    bool debug = true;
    static ContentManager& getInstance();

    void Run(std::unique_ptr<InputInstance> input,
        const std::wstring& window_title, const std::filesystem::path& page,
        std::string initial_ui = "", const bool& covered = false);
    bool TestRun(std::unique_ptr<InputInstance>& input,
        const std::filesystem::path& page,std::vector<std::string> initial_ui, int test_frame = 10);
    void SwitchContent(const std::string& ui_name);

    // É¾³ý¿½±´²Ù×÷
    ContentManager(const ContentManager&) = delete;
    ContentManager& operator=(const ContentManager&) = delete;
    
private:
    ContentManager() = default;
    ~ContentManager();

    void LoadContent(const std::string& ui_name, const std::string& package);
    void CleanupContents();

    std::unordered_map<std::string, std::unique_ptr<PlayingUIContent>> contents_;
    PlayingUIContent* current_euc_ = nullptr;
    std::unique_ptr<InputInstance> global_input_;
    std::string current_ui_;
    std::filesystem::path package;
};
