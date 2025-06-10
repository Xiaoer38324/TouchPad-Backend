# 虚拟按键后端（C++）

本项目是一个基于 C++  实现的虚拟按键后端。通过命令行操作，可实现自定义 UI 项目的打包与运行。适用于虚拟按键的可视化渲染和项目分发。

## 演示说明

> **演示中所用游戏画面与本项目无关，仅为 UI 功能演示用途。所有游戏内容的版权归原开发者所有。**

（明天再研究一下如何把MP4更好的展示。睡觉）
---

## 功能简介

- 支持将 UI 项目目录打包为可分发文件。
- 支持从打包文件中加载并运行 UI。
- 可设置初始界面、窗口大小、目标窗口等参数。
- 支持调试模式显示控件区域。

## 依赖项

本项目依赖以下第三方库，请确保在编译前已正确安装：

- **[libzip](https://libzip.org/)**（版本：1.11.3）—— 用于处理zip格式的UI项目文件。
- **[cxxopts](https://github.com/jarro2783/cxxopts)**（版本：3.2.0）—— 用于命令行参数解析。
- **[nlohmann/json](https://github.com/nlohmann/json)**（建议使用最新版）—— 用于处理JSON格式数据。
- **[ImGui](https://github.com/ocornut/imgui)** —— 实现UI渲染。
- **[stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)**、**[stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h)**、**[stb_image_resize2.h](https://github.com/nothings/stb/blob/master/stb_image_resize2.h)** —— 用于图片加载、缩放与写入，已直接集成于本项目源码。

> 请根据依赖项目自行编译安装。

## 命令行用法

执行主程序后，支持如下命令（UI Tool是软件的名称）：

### 帮助指令

```sh
UI Tool -h
```
或
```sh
UI Tool --help
```

会输出：

```
Available commands:
  run       Run the UI from a packaged file
  package   Package a UI project directory

Usage:
  UI Tool [OPTION...] positional parameters

  -h, --help  Print usage
```

---

### 1. 运行UI（run）

用于从已打包的UI文件启动界面。

```sh
UI Tool run [OPTION...]
```

**可选项说明：**

| 参数 | 简写 | 说明 |
| ---- | ---- | ---- |
| --help | -h | 打印用法说明 |
| --file | -f | UI文件路径（必填） |
| --debug | -d | 显示控件区域（可选） |
| --initiativeui | -i | 初始UI界面（可选，默认：""） |
| --covered | -c | 是否覆盖（可选，若为true需指定目标窗口） |
| --target | -t | 目标窗口名（当covered为true时必填，默认：""） |

**示例：**

```sh
UI Tool run -f my_ui.zip -i MainMenu
UI Tool run -f my_ui.zip -d --covered --target "Game Window"
```

---

### 2. 打包UI项目（package）

用于将UI项目目录打包为zip文件。

```sh
UI Tool package [OPTION...]
```

**可选项说明：**

| 参数 | 简写 | 说明 |
| ---- | ---- | ---- |
| --dir | -d | UI项目目录路径（必填） |
| --initiativeui | -i | 初始UI界面（必填） |
| --width | -w | 窗口宽度 |
| --height | -e | 窗口高度 |
| --help | -h | 打印用法说明 |

**示例：**

```sh
UI Tool package -d ./ui_project -i MainMenu -w 800 -e 600
```

---

## UI 项目结构

每一个 UI 项目需遵循以下目录与文件结构：

```
your_ui_project/
├── texture/      # 存放纹理图片资源，如 PNG、JPG 等
├── ui/           # 存放 UI 布局文件（JSON 格式）
├── E             # 事件库文件（JSON 格式）
├── T             # 纹理库文件（JSON 格式）
```

- **texture/**  
  存放所有用到的纹理图片资源（如 PNG、JPG 等），用于 UI 元素的可视化渲染。

- **ui/**  
  存放 UI 布局文件。每个布局文件定义了一个界面或界面片段，通常为 JSON 格式。

- **E**  
  事件库文件，存储 UI 相关的预设逻辑资源，本质为 JSON 文件。

- **T**  
  纹理库文件，对 UI 项目中用到的纹理资源索引，本质为 JSON 文件。

> **注意**：  
> - 该目录结构是项目打包、运行的基础。请确保每个 UI 项目都具备上述文件夹和文件。  
> - `texture` 和 `ui` 为文件夹，`E` 和 `T` 为文件。  
> - 布局文件、事件库、纹理库本质都是 JSON。项目中有完善的接口用于添加和序列化脚本、事件、纹理等数据。**不建议直接手动编辑这些结构**。如确需了解格式，请参考项目中的 UI 案例。

---

## 编译产物目录结构示例

编译和打包操作完成后，项目根目录下的文件结构可能如下：

```
your_project_root/
├── E           # 事件库文件
├── I           # 中间文件
├── package.p   # 可运行的打包存档
├── T           # 纹理库文件
```

- **I**：中间文件，用于记录或缓存部分中间数据。
- **package.p**：可运行的打包档案文件，包含完整的 UI 项目内容，可直接通过命令行工具运行。

---

## TODO

1. 格式化代码风格，整理代码。
2. 优化功能。
3. 测试触屏机制是否可用（目前没有设备，暂时无法测试）。
4. 增强模拟输入。

---

## 注意事项

- 本项目旨在为学习和技术交流提供便利，建议用户谨慎、辩证地学习和使用本项目内容，不建议直接用于生产环境。
- 触屏输入机制目前没有经过实际设备测试，相关代码仅为理论实现，实际可用性有待验证。如有需求，欢迎自行测试并反馈问题。
- 使用本项目过程中，请遵守相关法律法规和道德规范，合理使用虚拟按键软件。
- 本软件测试游戏有限，请慎重使用，特别是网游之类的。

---

## 其他说明

- 工具所有命令均可通过`-h`或`--help`查看详细用法。
- 渲染基于 ImGui，确保环境支持相关依赖。
- 如需了解布局、事件、纹理的 JSON 结构，请参考项目中的实际 UI 案例。

---

## License

本项目采用 [GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.txt) 协议开源。  
你可以自由地学习、改造、分发和商业使用本项目，但必须在衍生作品中同样采用GPL协议并开源。详情见 LICENSE 文件。
