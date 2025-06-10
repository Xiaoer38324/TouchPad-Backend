#include "../header/Package_Loder.h"
#include <algorithm>
#include <cctype>
#include <iostream>

Package_Loader& Package_Loader::getInstance() {
    static Package_Loader instance;
    return instance;
}

Package_Loader::~Package_Loader() {
    DoneImpl();
}

bool Package_Loader::Loader(const std::filesystem::path& zip_path,const bool& texture_only) {
    DoneImpl();
    int error;
    zip_ = zip_open(zip_path.string().c_str(), 0, &error);
    if (!zip_) return false;

    if (!texture_only&&(!LoadConfigFile("E", e_json_) || !LoadConfigFile("T", t_json_) || !LoadConfigFile("I", i_json_))) {
        DoneImpl();
        return false;
    }

    CacheUiFiles();
    CacheTextureIndices();
    return true;
}

std::vector<uint8_t> Package_Loader::GetTexture(const std::string& name) {
    std::string full_path = "texture/" + name;
    return ReadZipEntry(FindEntry(full_path));
}

const json& Package_Loader::GetU(const std::string& ui_name) const {
    static const json empty_json;
    auto it = u_jsons_.find(ui_name);
    return it != u_jsons_.end() ? it->second : empty_json;
}

void Package_Loader::Done() {
    DoneImpl();
}

// 以下是私有方法实现
void Package_Loader::CacheUiFiles() {
    zip_int64_t num_entries = zip_get_num_entries(zip_, 0);
    for (zip_int64_t i = 0; i < num_entries; ++i) {
        if (const char* name = zip_get_name(zip_, i, 0)) {
            std::string path = NormalizePath(name);
            if (IsUiConfig(path)) {
                LoadUiConfig(i, path);
            }
        }
    }
}

void Package_Loader::LoadUiConfig(zip_int64_t index, const std::string& path) {
    auto data = ReadZipEntry(index);
    if (!data.empty()) {
        try {
            std::string key = ExtractUiKey(path);
            u_jsons_[key] = json::parse(data);
        }
        catch (const json::exception&) {
            std::cout << path << "UI导入失败" << std::endl;
            // 记录错误日志
        }
    }
}

std::vector<uint8_t> Package_Loader::ReadZipEntry(zip_uint64_t index) const {
    if (index == InvalidIndex) return {};

    struct zip_stat st;
    if (zip_stat_index(zip_, index, 0, &st) != 0) return {};

    std::vector<uint8_t> buffer(st.size);
    if (auto* file = zip_fopen_index(zip_, index, 0)) {
        zip_fread(file, buffer.data(), buffer.size());
        zip_fclose(file);
    }
    return buffer;
}

void Package_Loader::CacheTextureIndices() {
    zip_int64_t num_entries = zip_get_num_entries(zip_, 0);
    for (zip_int64_t i = 0; i < num_entries; ++i) {
        if (const char* name = zip_get_name(zip_, i, 0)) {
            std::string path = NormalizePath(name);
            if (path.find("texture/") == 0) {
                texture_indices_[path] = i;
            }
        }
    }
}

bool Package_Loader::LoadConfigFile(const char* name, json& target) {
    struct zip_stat st;
    if (zip_stat(zip_, name, 0, &st) != 0) return false;

    auto data = ReadZipEntry(st.index);
    if (data.empty()) return false;

    try {
        target = json::parse(data);
    }
    catch (const json::exception&) {
        return false;
    }
    return true;
}

void Package_Loader::DoneImpl() {
    if (zip_) {
        zip_close(zip_);
        zip_ = nullptr;
    }
    e_json_.clear();
    t_json_.clear();
    u_jsons_.clear();
    texture_indices_.clear();
}


std::string Package_Loader::NormalizePath(const std::string& path) {
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

bool  Package_Loader::IsUiConfig(const std::string& path) const {
    const std::string prefix = "ui/";
    return path.compare(0, prefix.length(), prefix) == 0&&path.size()!=3;
}

std::string Package_Loader::ExtractUiKey(const std::string& path) const {
    const size_t prefix_len = 3; // "ui/"
    return path.substr(prefix_len, path.length() - prefix_len);
}

zip_uint64_t Package_Loader::FindEntry(const std::string& path) const {
    auto it = texture_indices_.find(path);
    return it != texture_indices_.end() ? it->second : InvalidIndex;
}
