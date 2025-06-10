#include "../header/Material.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../header/stb_image.h"
#include "../header/stb_image_resize2.h"
#include "../header/VTP_Type.h"
#include "../header/Package_Loder.h"
//std::unordered_map<uint32_t, unsigned char>  Area_Material::Load(const std::string& path)
//{
//    int channel;
//    unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channel, 0);
//    if (!pixels) {
//        printf("Failed to load image: %s\n", path);
//        //��־
//    }
//    std::unordered_map<uint32_t, unsigned char> color_to_id_map;
//    color_to_id_map[0] = 0;
//    area.reserve(width * height);
//    for (int i = 0; i < width * height; ++i) {
//        uint32_t key = color_to_key(pixels + i * channel, channel);
//        auto it = color_to_id_map.find(key);
//        if (it != color_to_id_map.end()) {
//            area[i] = it->second; // ��ɫ�Ѵ��ڣ����ؾ�ID
//        }
//        else {
//            color_to_id_map[key] = next_region_id;
//            area[i] = next_region_id;
//            next_region_id++; // ������ID
//        }
//    }
//    stbi_image_free(pixels);
//    return color_to_id_map;
//}
//
//unsigned char Area_Material::GetIndex( float x, float y)
//{
//    TouchINT pixelX = static_cast<int>(x*width);
//    TouchINT pixelY = static_cast<int>(y*height);
//    if (pixelY * width + pixelX < area.size())
//        return area[pixelY * width + pixelX];
//    return 255;
//}
#include <vector>
#include <thread>
#include <cstdint>
#include <windows.h>
#include <immintrin.h> // SIMD
#include <intrin.h>    // __cpuid, _xgetbv
#include <iostream>
#include <algorithm>

// �ж��Ƿ��� x86/x64 �ܹ�
bool isX86orX64() {
    SYSTEM_INFO sysInfo;
    GetNativeSystemInfo(&sysInfo);
    return sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
        sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL;
}

// �ж��Ƿ�֧�� AVX2������ʱ�ж� + �ܹ���⣩
bool supportsAVX2() {
    if (!isX86orX64()) return false;

    int cpuInfo[4];
    __cpuid(cpuInfo, 0);
    int nIds = cpuInfo[0];
    if (nIds < 7) return false;

    // ������ϵͳ�Ƿ������� AVX ״̬
    __cpuid(cpuInfo, 1);
    bool osUsesXSAVE_XRSTORE = (cpuInfo[2] & (1 << 27)) != 0;
    bool cpuAVXSupport = (cpuInfo[2] & (1 << 28)) != 0;

    if (!(osUsesXSAVE_XRSTORE && cpuAVXSupport)) return false;

    unsigned long long xcrFeatureMask = _xgetbv(0);
    if ((xcrFeatureMask & 0x6) != 0x6) return false;

    // ��� AVX2 ֧��
    __cpuidex(cpuInfo, 7, 0);
    return (cpuInfo[1] & (1 << 5)) != 0;
}

// SIMD ���ٴ���AVX2��
// ��alphaͨ����mask��alpha < thresholdΪtrue
void process_simd(const uint8_t* pixels, std::vector<bool>& mask, int width, int height, uint8_t alpha_threshold, int start_row, int end_row) {
    const int stride = width * 4;

    for (int i = start_row; i < end_row; ++i) {
        const uint8_t* row = pixels + i * stride;
        for (int j = 0; j < width; j += 32) {
            int count = std::min(32, width - j);
            alignas(32) uint8_t alphas[32] = {};
            for (int k = 0; k < count; ++k) {
                alphas[k] = row[(j + k) * 4 + 3];
            }

            __m256i alphaVec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(alphas));
            __m256i threshold = _mm256_set1_epi8(alpha_threshold); // ע������ֱ��uint8_t����
            __m256i diff = _mm256_subs_epu8(threshold, alphaVec);  // �޷��ű��ͼ���

            alignas(32) uint8_t maskResults[32];
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(maskResults), diff); // ע��u

            for (int k = 0; k < count; ++k) {
                mask[i * width + j + k] = (maskResults[k] != 0); // ֻҪdiff���㣬˵��alpha < threshold
            }
        }
    }
}

// ��׼���̴߳����� SIMD��
void process_scalar(const uint8_t* pixels, std::vector<bool>& mask, int width, int height, uint8_t alpha_threshold, int start_row, int end_row) {
    const int stride = width * 4;

    for (int i = start_row; i < end_row; ++i) {
        for (int j = 0; j < width; ++j) {
            uint8_t alpha = pixels[(i * width + j) * 4 + 3];
            mask[i * width + j] = alpha < alpha_threshold;
        }
    }
}

// ���ĺ�����ͳһ����
void processImage(const int& width, const int& height, const uint8_t* pixels, Material& material, uint8_t alpha_threshold, int num_threads = 4) {
    material.mask.resize(width * height);
    std::vector<std::thread> threads;
    bool use_simd = supportsAVX2();

    int rows_per_thread = height / num_threads;

    for (int t = 0; t < num_threads; ++t) {
        int start_row = t * rows_per_thread;
        int end_row = (t == num_threads - 1) ? height : start_row + rows_per_thread;

        threads.emplace_back([=, &material, &pixels]() {
            if (use_simd) {
                process_simd(pixels, material.mask, width, height, alpha_threshold, start_row, end_row);
            }
            else {
                process_scalar(pixels, material.mask, width,height, alpha_threshold, start_row, end_row);
            }
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

bool MaterialInfo::LoadMaterial(Material& material)
{
    TextureData data;
    std::vector<uint8_t> texture = Package_Loader::getInstance().GetTexture(path);
    int len = static_cast<int>(texture.size());
    if (len > INT_MAX)
    {
        std::cout <<path<<"ͼƬ̫��" << std::endl;
        //��־������С
        return false;
    }
    if (len == 0) {
        std::cout << path << "��ͼƬ�����ܲ�δ�ҵ���ͼƬ�����Կ�ͼƬ���" << std::endl;
    }
    unsigned char* pixels = stbi_load_from_memory(texture.data(), len, &data.width, &data.height, &data.channel,0);
    //unsigned char* pixels = stbi_load(path.c_str(), &data.width, &data.height, &data.channel, 0);
    if (!pixels) {
        std::cout << path << "����ʧ��" << std::endl;
        //��־
        return false;
    }
    GLuint gltextureID;
    glGenTextures(1, &gltextureID);
    glBindTexture(GL_TEXTURE_2D, gltextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    data.id = gltextureID;
   
    if (_type== MaskGenerType::ALPHA) {
        if (data.channel != 4) {
            stbi_image_free(pixels);
            std::cout << path << "��͸��ͼƬ��" << std::endl;
            //��־
            return false;
        }
        int threads = std::thread::hardware_concurrency();

        processImage(data.width,data.height,pixels, material, alpha , threads);
        //�Ϸ��������á��Ż�
        //for (int i = 0; i < data.height; i++) {
        //    for (int j = 0; j < data.width; j++) {
        //        //std::cout << (int)pixels[i * data.width + j + 3] << std::endl;
        //        material.mask.push_back(pixels[(i * data.width + j) * 4 + 3] < alpha);
        //    }
        //}
    }
    else if (_type == MaskGenerType::ANOTHER)
    {
        int w, h, c;
        unsigned char* maskpixels;
        unsigned char* resized_img;
        std::vector<uint8_t> mask_texture = Package_Loader::getInstance().GetTexture(mask);
        int mask_len = static_cast<int>(mask_texture.size());
        if (mask_len > INT_MAX)
        {
            stbi_image_free(pixels);
            std::cout << path << "ͼƬ̫��" << std::endl;
            //��־������С
            return false;
        }
        maskpixels = stbi_load_from_memory(mask_texture.data(), mask_len, &w, &h, &c, 0);
        if (c != 1) {
            //��Mask��
            stbi_image_free(pixels);
            printf("Mask Image only has one channel: %s\n", path.c_str());
            return false;
        }
        resized_img = (unsigned char*)malloc(data.width * data.height * c);
        stbir_resize_uint8_linear(maskpixels, w, h, 0, resized_img, data.width, data.height, 0, STBIR_1CHANNEL);

        int threads = std::thread::hardware_concurrency();
        processImage(data.width, data.height, pixels, material, alpha, threads);
        //�Ϸ��������á��Ż�
        //for (int i = 0; i < data.height; i++) {
        //    for (int j = 0; j < data.width; j++) {
        //        //std::cout << (int)pixels[i * data.width + j + 3] << std::endl;
        //        material.mask.push_back(resized_img[i * data.width + j] < 10);
        //    }
        //}
        free(resized_img);
        stbi_image_free(maskpixels);
    }
    else if (_type == MaskGenerType::NONE) {
        material.mask.clear();
    }
    stbi_image_free(pixels);
    material.texture = data;
    return true;
}
