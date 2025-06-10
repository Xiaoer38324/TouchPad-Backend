#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <zip.h>
#include <nlohmann/json.hpp>
#include"Material.h"
#include"UI_Event_Resources.h"
#include "test_MouseClick.h"
#include "ContentManager.h"
namespace fs = std::filesystem;
using json = nlohmann::json;
class Package_Saver {
public:
    explicit Package_Saver(const fs::path& project_root,const std::string& main_ui,const int& width,const int &height)
        : root_(fs::absolute(project_root)),
        e_file_(root_ / "E"),
        t_file_(root_ / "T"),
        ui_dir_(root_ / "ui"),
        texture_dir_(root_ / "texture"),
        output_file_(root_ / "package.p"),
        i_file_(root_/"I"),
        main_ui(main_ui),
        width(width),
        height(height){}

    bool process() {
        try {
            if (!validate_structure()) return false;
            if (!validate_resources()) return false;
            if (!set_info()) return false;
            if (!create_package()) return false;
            std::cout << "Test pack!\n";
            test_ui();
            std::cout << "Finish\n";
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            return false;
        }
    }

private:
    void test_ui()const {
        std::unique_ptr<testMouseClick> tmc = std::make_unique<testMouseClick>();
        std::unique_ptr<InputInstance> ii = std::move(tmc);
        ContentManager::getInstance().TestRun(ii, output_file_, getFilesInDir());
    }
    bool set_info()const {
        const nlohmann::json i = {
            { "initial_ui",main_ui },
            {"width",width},
            {"height",height}
        };
       std::ofstream file(i_file_, std::ios::out);
       if (!file.is_open()) {
           throw std::runtime_error("Cannot open file: " + i_file_.string());
           return false;
       }
       file << i;
       file.close();
       return true;
    }
    // 基础结构验证
    bool validate_structure() const {
        const std::vector<fs::path> required_paths = {
            e_file_,
            t_file_,
            ui_dir_,
            texture_dir_
        };

        for (const auto& path : required_paths) {
            if (!fs::exists(path)) {
                std::cerr << "Missing required path: " << path << "\n";
                return false;
            }
        }

        if (!fs::is_directory(ui_dir_)) {
            std::cerr << "ui path is not a directory\n";
            return false;
        }

        if (!fs::is_directory(texture_dir_)) {
            std::cerr << "texture path is not a directory\n";
            return false;
        }

        return true;
    }

    // 资源内容验证
    bool validate_resources() {
        return validate_special_file(t_file_, &Package_Saver::validate_T) &&
            validate_special_file(e_file_, &Package_Saver::validate_E) &&
            validate_ui_files();
    }

    // 特殊文件验证模板
    template<typename Validator>
    bool validate_special_file(const fs::path& path, Validator validator) {
        try {
            std::string content = read_file(path);
            json data = json::parse(content);
            return (this->*validator)(data);
        }
        catch (const json::parse_error& e) {
            std::cerr << "JSON parse error in " << path << ": " << e.what() << "\n";
            return false;
        }
        catch (const std::exception& e) {
            std::cerr << "Error processing " << path << ": " << e.what() << "\n";
            return false;
        }
    }

    // UI文件验证
    bool validate_ui_files() {
        size_t success_count = 0;
        size_t total_files = 0;

        try {
            for (const auto& entry : fs::directory_iterator(ui_dir_)) {
                if (!entry.is_regular_file()) continue;

                ++total_files;
                if (validate_ui_file(entry.path())) {
                    ++success_count;
                }
                else {
                    std::cerr << "Failed to validate: " << entry.path() << "\n";
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Directory iteration failed: " << e.what() << "\n";
            return false;
        }

        if (total_files == 0) {
            std::cerr << "No UI files found in directory\n";
            return false;
        }

        return success_count == total_files;
    }

    bool validate_ui_file(const fs::path& path) {
        try {
            std::string content = read_file(path);
            json data = json::parse(content);
            return validate_UI(data);
        }
        catch (const json::parse_error& e) {
            std::cerr << "Invalid JSON in " << path << ": " << e.what() << "\n";
            return false;
        }
        catch (const std::exception& e) {
            std::cerr << "Error reading " << path << ": " << e.what() << "\n";
            return false;
        }
    }

    // 文件读取
    std::string read_file(const fs::path& path) const {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + path.string());
        }

        return std::string(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );
    }

    // 验证方法（需根据实际需求实现）
    bool validate_T(const json& data) {

        return Material_Resources::getInstance().Deserialization(data);

    }

    bool validate_E(const json& data) {
        return  UI_Event_Resources::getInstance().Deserialization(data);

    }

    bool validate_UI(const json& data) {
        return true;
    }

    // 创建压缩包
    bool create_package() const {
        zip_t* zip = zip_open(output_file_.string().c_str(), ZIP_CREATE | ZIP_TRUNCATE, nullptr);
        if (!zip) {
            std::cerr << "Cannot create zip archive\n";
            return false;
        }

        try {
            add_to_zip(zip, e_file_);
            add_to_zip(zip, i_file_);
            add_to_zip(zip, t_file_);
            add_directory(zip, ui_dir_, "ui");
            add_directory(zip, texture_dir_, "texture");

            if (zip_close(zip) != 0) {
                throw std::runtime_error("Failed to finalize zip archive");
            }
        }
        catch (...) {
            zip_discard(zip);
            throw;
        }

        return true;
    }
    inline std::vector<std::string> getFilesInDir() const {
        std::vector<std::string> files;

        if (!std::filesystem::exists(ui_dir_) || !std::filesystem::is_directory(ui_dir_)) {
            throw std::runtime_error("Invalid directory: " + ui_dir_.string());
        }

        for (const auto& entry : std::filesystem::directory_iterator(ui_dir_)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());  // 只要文件名
            }
        }

        return files;
    }
    void add_directory(zip_t* zip, const fs::path& dir, const std::string& prefix) const {
        for (const auto& entry : fs::recursive_directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                const auto relative_path = get_relative_path(entry.path(), dir, prefix);
                add_to_zip(zip, entry.path(), relative_path);
            }
        }
    }

    std::string get_relative_path(const fs::path& full_path,
        const fs::path& base_dir,
        const std::string& prefix) const {
        auto relative = fs::relative(full_path, base_dir);
        return (fs::path(prefix) / relative).generic_string();
    }

    void add_to_zip(zip_t* zip, const fs::path& file, const std::string& zip_path = "") const {
        const std::string target_path = zip_path.empty() ? file.filename().string() : zip_path;

        zip_source_t* source = zip_source_file(zip, file.string().c_str(), 0, 0);
        if (!source) {
            throw std::runtime_error("Failed to create zip source for: " + file.string());
        }

        if (zip_file_add(zip, target_path.c_str(), source, ZIP_FL_ENC_UTF_8) == -1) {
            zip_source_free(source);
            throw std::runtime_error("Failed to add file to zip: " + file.string());
        }
    }

    // 成员变量
    const fs::path root_;
    const fs::path e_file_;
    const fs::path t_file_;
    const fs::path ui_dir_;
    const fs::path texture_dir_;
    const fs::path output_file_;
    const fs::path i_file_;
    const std::string main_ui;
    const int width;
    const int height;
};
