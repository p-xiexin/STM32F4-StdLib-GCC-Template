# STM32F4 项目编译、烧录与调试使用文档

## 环境准备

### 下载交叉编译工具链

访问：[GNU Toolchains for ARM Embedded](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

```bash
mkdir -p ~/opts/arm-toolchain
wget https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz
tar -xf arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz -C ~/opts/arm-toolchain 
```

安装完成后，将路径添加到环境变量中：

```bash
echo 'export PATH="$HOME/opts/arm-toolchain/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

验证工具链版本：

```bash
arm-none-eabi-gcc --version
```

### 安装 OpenOCD

```bash
sudo apt install openocd
openocd --version
```

## 编译项目

**创建构建目录并编译：**

```bash
mkdir build
cd build
cmake ..
make -j
```

**VSCode 快捷任务示例**

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "cmake",
            "type": "shell",
            "command": "cmake",
            "args": ["-B", "${workspaceFolder}/build", "-G", "Unix Makefiles"],
            "options": {"cwd": "${workspaceFolder}"},
            "detail": "配置 CMake 构建系统"
        },
        {
            "label": "build",
            "type": "shell",
            "command": "cmake",
            "args": ["--build", "${workspaceFolder}/build", "-j", "8"],
            "options": {"cwd": "${workspaceFolder}"},
            "group": {"kind": "build", "isDefault": true},
            "dependsOn": ["cmake"],
            "presentation": {"clear": true, "panel": "shared"},
            "detail": "编译 STM32 项目"
        },
        {
            "label": "flash",
            "type": "shell",
            "command": "openocd",
            "args": [
                "-s", "/usr/share/openocd/scripts",
                "-f", "interface/stlink-v2.cfg",
                "-f", "target/stm32f4x.cfg",
                "-c", "program ${workspaceFolder}/build/Test.elf verify reset exit"
            ],
            "options": {"cwd": "${workspaceFolder}"},
            "dependsOn": ["build"],
            "presentation": {"clear": true, "panel": "shared"},
            "detail": "将程序烧录到 STM32 芯片"
        },
      		
        {
            "label": "clean",
            "type": "shell",
            "command": "rm",
            "args": ["-rf", "${workspaceFolder}/build"],
            "options": {"cwd": "${workspaceFolder}"},
            "detail": "删除 build 目录"
        },
        {
            "label": "rebuild",
            "type": "shell",
            "dependsOn": ["clean", "build"],
            "dependsOrder": "sequence",
            "detail": "清空并重新编译"
        }
    ]
}
```

## 命令行烧录与调试流程

### 启动 OpenOCD

```bash
openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg \
        -f /usr/share/openocd/scripts/target/stm32f4x.cfg
```

> 说明：
>
> - `interface/stlink-v2.cfg`：ST-Link V2 配置
> - `target/stm32f4x.cfg`：STM32F4 系列芯片
> - 启动成功会显示监听端口 3333，用于 GDB 连接

### 启动 GDB

```bash
cd ~/Downloads/STM32-Project-Template-main/STM32f407_freeRTOS/build
/home/pxx/opts/arm-toolchain/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gdb Test.elf
```

### 连接 OpenOCD

在 GDB 提示符下输入：

```gdb
target remote localhost:3333
monitor reset halt
```

> `monitor reset halt`：复位 MCU 并停止在入口

### 烧录程序

```gdb
load
```

> ELF 文件被写入 MCU 的 Flash 或 RAM

### 初始化 MCU 并运行

```gdb
monitor reset init
continue
```

### 常用 GDB 调试命令

| 操作          | 命令                    |
| ------------- | ----------------------- |
| 设置断点      | `break main`            |
| 单步执行      | `step`                  |
| 下一行        | `next`                  |
| 查看寄存器    | `info registers`        |
| 打印变量      | `print var_name`        |
| 暂停 MCU      | `interrupt` 或 `Ctrl+C` |
| MCU halt      | `monitor halt`          |
| MCU 复位 halt | `monitor reset halt`    |
| 查看内存      | `x/16xw 0x20020000`     |

### 流程总结

1. 打开 OpenOCD，连接开发板
2. 启动 GDB 并连接 OpenOCD
3. `monitor reset halt` 停在入口
4. `load` 烧录程序
5. `monitor reset init` 初始化 MCU
6. `continue` 开始运行
7. 使用断点/单步/打印命令调试

> ⚡ 此流程可绕开 VSCode 插件，直接验证硬件、OpenOCD 和工具链是否正常工作

## VSCode 调试配置

> ⚠️ 注意：Cortex-Debug 插件在某些环境下稳定性不佳，因此这里使用 `Attach` 方式直接连接已运行的 OpenOCD 服务器（依旧依赖Cortex-Debug）。
> 目前在笔记本上调试正确，但是在台式电脑中一直出现`Failed to launch OpenOCD GDB Server: Timeout.`

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "showDevDebugOutput": "raw",
            "cwd": "${workspaceFolder}",
            "type": "cortex-debug",
            "request": "launch",
            "name": "OpenOCD Debug",
            "servertype": "openocd",
            "executable": "${workspaceFolder}/build/Test.elf",
            "runToEntryPoint": "main",
            "device": "STM32F407",
            "configFiles": [
                "/usr/share/openocd/scripts/interface/stlink-v2.cfg",
                "/usr/share/openocd/scripts/target/stm32f4x.cfg"
            ],
            "serverArgs": ["-c", "adapter_khz 1000"]
        },
        {
            "name": "Attach to STM32F407",
            "type": "cortex-debug",
            "request": "launch",
            "executable": "${workspaceFolder}/build/Test.elf",
            "servertype": "external",
            "gdbTarget": "localhost:3333",
            "device": "STM32F407VG",
            "showDevDebugOutput": "raw",
            "runToEntryPoint": "main"
        }
    ]
}
```

为了获得更好的代码补全和静态分析，可在 VSCode 中配置 Clangd。

`clangd` 是 LLVM 提供的语言服务器，支持 C/C++ 的智能补全、跳转、语法检查和重构，基于 `compile_commands.json` 获取编译参数，能正确识别交叉编译工程的 include 路径和宏定义。

相比 VSCode 的另一款常用插件（C/C++ 插件），`clangd` 的优势在于：

- 解析精度高，更贴近真实编译器行为
- 对大型工程性能更好，索引和补全速度快
- 支持跨平台和交叉编译项目，无需修改 VSCode 内置 IntelliSense 设置

C/C++ 插件虽方便，但在交叉编译环境下容易出现头文件、宏识别错误，智能感知不如 `clangd` 稳定。

**配置方法：**

1. 安装 Clangd 插件
2. 在项目根目录创建 `compile_flags.txt` 或配置 `.clangd` 文件

示例 `.clangd` 配置：

CompileFlags:

  Remove: ["-mthumb-interwork"]

> 说明：STM32F4 工程中 `-mthumb-interwork` 可被移除以避免 Clangd 报告不必要的警告。
