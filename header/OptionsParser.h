#pragma once
#include <cxxopts.hpp>
#include <iostream>
#include <filesystem>
#include <string>
#include <stdexcept>

namespace fs = std::filesystem;

struct RunOptions {
    fs::path ui_file;
    std::string initial_ui;
    bool covered = false;
    bool debug = false;
    std::wstring target_window;
};

struct PackageOptions {
    fs::path ui_project_dir;
    std::string initial_ui;
    int width;
    int height;
};

class OptionsParser {
public:
    enum class Command { Run, Package, Unknown };

    Command parse(int argc, char* argv[], RunOptions& runOpts, PackageOptions& pkgOpts) {
        std::string command;

        try {
            cxxopts::Options options("UI Tool", "A tool for running and packaging UI files\n\n"
                "Available commands:\n"
                "  run       Run the UI from a packaged file\n"
                "  package   Package a UI project directory\n");
            // Common options
            options.allow_unrecognised_options();
            options.add_options()
                ("h,help", "Print usage")
                ("command", "Command to execute (run or package)", cxxopts::value<std::string>())
                ;

            options.parse_positional({ "command" });
            auto result = options.parse(argc, argv);

            if ((result.count("help") || argc == 1)&& !result.count("command")) {
                std::cout << options.help() << std::endl;
                return Command::Unknown;
            }

            if (!result.count("command")) {
                throw std::runtime_error("No command specified");
            }

            command = result["command"].as<std::string>();
        }
        catch (const cxxopts::exceptions::exception& e) {
            throw std::runtime_error("Options error: " + std::string(e.what()));
        }

        // 根据 command 创建新的 options
        try {
            if (command == "run") {
                cxxopts::Options runOptions("UI Tool - run", "Run the UI");
                runOptions.add_options()
                    ("h,help", "Print usage")
                    ("f,file", "UI file path (zip format, required)", cxxopts::value<std::string>())
                    ("d,debug", "Show the box area", cxxopts::value<bool>()->default_value("false"))
                    ("i,initiativeui", "Initial UI interface (optional)", cxxopts::value<std::string>()->default_value(""))
                    ("c,covered", "Whether to cover (optional, requires target window if true)", cxxopts::value<bool>()->default_value("false"))
                    ("t,target", "Target window name (required if covered is true)", cxxopts::value<std::string>()->default_value(""))
                    ;

                auto result = runOptions.parse(argc, argv);
                if (result.count("help")||argc==2) {
                    std::cout << runOptions.help() << std::endl;
                    return Command::Unknown;
                }
                return parseRunOptions(result, runOpts);
            }
            else if (command == "package") {
                cxxopts::Options pkgOptions("UI Tool - package", "Package the UI");
                pkgOptions.add_options()
                    ("d,dir", "UI project directory path (required)", cxxopts::value<std::string>())
                    ("i,initiativeui", "Initial UI interface (required)", cxxopts::value<std::string>())
                    ("w,width", "Width of Window", cxxopts::value<int>())
                    ("e,height", "Height of Window", cxxopts::value<int>())
                    ("h,help", "Print usage");

                auto result = pkgOptions.parse(argc, argv);
                if (result.count("help") || argc == 2) {
                    std::cout << pkgOptions.help() << std::endl;
                    return Command::Unknown;
                }
                return parsePackageOptions(result, pkgOpts);
            }
            else {
                throw std::runtime_error("Unknown command: " + command);
            }
        }
        catch (const cxxopts::exceptions::exception& e) {
            throw std::runtime_error("Options error: " + std::string(e.what()));
        }

    }

private:
    Command parseRunOptions(const cxxopts::ParseResult& result, RunOptions& opts) {
        if (!result.count("file")) {
            throw std::runtime_error("File path is required for run command");
        }

        // Convert to wide string for file path
        std::string filePath = result["file"].as<std::string>();
        opts.ui_file = fs::path(filePath.begin(), filePath.end());

        // Validate file path
        if (!fs::exists(opts.ui_file)) {
            throw std::runtime_error("File does not exist: " + filePath);
        }

        if (fs::is_directory(opts.ui_file)) {
            throw std::runtime_error("Path must be a file, not a directory: " + filePath);
        }

        // Removed .zip extension check as per requirement
        // File content validation should be done when actually processing the file

        // Get optional initial UI
        opts.initial_ui = result["initiativeui"].as<std::string>();

        // Handle covered and target window
        opts.covered = result["covered"].as<bool>();
        opts.debug = result["debug"].as<bool>();
        if (opts.covered) {
            if (!result.count("target")) {
                throw std::runtime_error("Target window name is required when covered is true");
            }
            std::string target = result["target"].as<std::string>();
            opts.target_window = std::wstring(target.begin(), target.end());
        }

        return Command::Run;
    }

    Command parsePackageOptions(const cxxopts::ParseResult& result, PackageOptions& opts) {
        if (!result.count("dir")) {
            throw std::runtime_error("Project directory path is required for package command");
        }

        if (!result.count("initiativeui")) {
            throw std::runtime_error("Initial UI interface is required for package command");
        }
        if (!result.count("width")) {
            throw std::runtime_error("Width of Window is required for package command");
        }
        if (!result.count("height")) {
            throw std::runtime_error("Hidth of Window is required for package command");
        }

        // Convert to wide string for directory path
        std::string dirPath = result["dir"].as<std::string>();
        opts.ui_project_dir = fs::path(dirPath.begin(), dirPath.end());

        // Validate directory path
        if (!fs::exists(opts.ui_project_dir)) {
            throw std::runtime_error("Directory does not exist: " + dirPath);
        }

        if (!fs::is_directory(opts.ui_project_dir)) {
            throw std::runtime_error("Path must be a directory: " + dirPath);
        }

        // Get required initial UI
        opts.initial_ui = result["initiativeui"].as<std::string>();
        opts.height = result["height"].as<int>();
        opts.width = result["width"].as<int>();
        if (opts.height <= 0 || opts.width <= 0) {
            throw std::runtime_error("Size of window must larger than zero");
        }
        if (opts.initial_ui.empty()) {
            throw std::runtime_error("Initial UI interface cannot be empty");
        }
        std::filesystem::path _ui_path = opts.ui_project_dir /"ui"/ opts.initial_ui;
        if (!fs::exists(_ui_path)) {
            throw std::runtime_error("Initial UI does not exist: " + opts.initial_ui);
        }
        if (!fs::is_regular_file(_ui_path)) {
            throw std::runtime_error("Initial UI is not a regular file: " + opts.initial_ui);
        }
        
        return Command::Package;
    }
};
