#pragma once
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <zip.h>
using json = nlohmann::json;
class Package_Loader {
public:
    static Package_Loader& getInstance();

    bool Loader(const std::filesystem::path& zip_path,const bool& texture_only=false);
    std::vector<uint8_t> GetTexture(const std::string& name);
    const json& GetU(const std::string& ui_name) const;
    void Done();

    // 访问器
    const json& E() const { return e_json_; }
    const json& T() const { return t_json_; }
    const json& I() const { return i_json_; }

    Package_Loader(const Package_Loader&) = delete;
    Package_Loader& operator=(const Package_Loader&) = delete;

private:
    Package_Loader() = default;
    ~Package_Loader();

    // 实现细节
    void CacheUiFiles();
    void LoadUiConfig(zip_int64_t index, const std::string& path);
    std::vector<uint8_t> ReadZipEntry(zip_uint64_t index) const;
    void CacheTextureIndices();
    bool LoadConfigFile(const char* name, json& target);
    void DoneImpl();

    // 辅助函数
    static std::string NormalizeKey(std::string key);
    static std::string NormalizePath(const std::string& path);
    bool IsUiConfig(const std::string& path) const;
    std::string ExtractUiKey(const std::string& path) const;
    zip_uint64_t FindEntry(const std::string& path) const;

    // 成员变量
    static constexpr zip_uint64_t InvalidIndex = -1;
    zip_t* zip_ = nullptr;
    json e_json_;
    json t_json_;
    json i_json_;
    std::map<std::string, json> u_jsons_;
    std::map<std::string, zip_uint64_t> texture_indices_;
};
