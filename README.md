# GD32E230 嵌入式开发项目

一个基于GD32E230C8微控制器的嵌入式项目，包含TFT LCD显示、I2C设备扫描、NOR Flash访问和动画演示功能。

## 项目概述

这是一个基于淘宝老王电子所售卖的低价开发板 仅需3.5元（不含运费） 所实现的功能包括：

- **软件I2C实现**用于设备扫描和通信
- **TFT LCD显示**（240x240 ST7789驱动）支持图形和文本渲染
- **动画演示**包括弹跳球、扩散光环、粒子效果和旋转立方体
- **NOR Flash存储器**读写操作

## 硬件包含

- **MCU**: GD32E230C8（ARM Cortex-M23 @ 72MHz，64KB Flash，8KB RAM）
- **开发环境**: Keil MDK（uVision）
- **显示**: 240x240 TFT LCD，ST7789驱动
- **存储**: GD25Q128 NOR Flash存储器
- **通信**: 软件I2C，硬件SPI
- 可使用CN2座子与CN5排针孔外接I2C设备与SPI设备(注：CN2座子的5V电源为输入请不要接入器件)

## 项目功能

### 1. I2C设备扫描器

- 使用软件I2C实现
- 扫描I2C地址（0x08-0x77）
- 根据地址识别常见I2C设备
- 实时扫描并提供视觉反馈

### 2. TFT LCD图形显示

- 240x240分辨率，16位RGB565颜色
- ST7789驱动，优化通信效率
- 基本图形原语（直线、矩形、圆形）
- 文本渲染支持自定义字体
- 双缓冲技术减少闪烁

### 3. 动画演示

ps:只是AI写的小动画

- **弹跳球**: 多个彩色球的物理模拟
- **扩散光环**: 同心彩色光环，渐隐效果
- **颜色扫描**: 全屏颜色过渡效果
- **星空粒子**: 视差滚动的粒子系统
- **旋转立方体**: 3D立方体旋转，透视投影

### 5. 系统架构
- 模块化驱动设计（SPI、I2C、GPIO、RCU、SysTick）

## 目录结构

```
GD32E230/
├── Basic/                    # 基础硬件驱动
│   ├── Hardware_SPI/        # 硬件SPI驱动
│   ├── RCU/                 # 时钟控制单元驱动
│   ├── Software_IIC/        # 软件I2C驱动（.C/.H文件）
│   └── SysTick/             # 系统滴答定时器驱动
├── Hardware/                # 外设设备驱动
│   ├── NorFlash_25Q/       # NOR Flash存储器驱动
│   └── TFT_LCD/            # TFT LCD显示驱动
├── Lib/                     # GD32E23x外设库
│   ├── inc/                # 外设头文件
│   └── src/                # 外设源文件
├── CMSIS/                   # ARM CMSIS核心文件
│   ├── GD/GD32E23x/        # GD32E23x特定文件
│   │   ├── Include/        # gd32e23x.h, system_gd32e23x.h
│   │   └── Source/         # system_gd32e23x.c, startup_gd32e23x.s
│   └── LICENSE.TXT         # CMSIS许可证
├── User/                    # 应用程序代码
│   ├── main/               # 主应用程序
│   │   ├── main.c          # 入口点，包含I2C扫描功能
│   │   ├── main.h          # 主头文件
│   │   └── animation_demo.c # 动画演示代码
│   ├── clock/              # 时钟配置（未使用）
│   └── int/                # 中断处理程序（空）
├── RTE/                     # Keil运行时环境
│   └── _GD32E23x/          # 设备特定RTE文件
├── list/                    # 列表文件
├── objects/                 # 对象文件（遗留）
├── Project.uvprojx          # Keil MDK工程文件
├── Project.uvoptx           # Keil项目选项文件
└── Project.uvguix.*         # Keil GUI布局文件
```

## 构建项目

### 构建条件

- Keil MDK（uVision）V5.3或更高
- Keil中安装GD32E23x芯片包

### 构建步骤

1. 在Keil MDK中打开工程文件`Project.uvprojx`
2. 构建项目（F7或构建菜单）

### 构建配置

- **编译器**: ARMCLANG（V6.22）
- **构建后处理（可选）**：
  1. 使用`fromelf`生成二进制文件
  2. 使用`Keil5_disp_size_bar_v0.3.exe`显示占用空间

## 烧录到硬件

### 方法1：Keil MDK调试器

1. 通过SWD接口连接GD32E230开发板
2. 在Keil选项目标 → 调试中配置调试设置
3. 使用ULINK2、J-Link或DAP-Link及其他兼容调试器
4. 点调试即可

## 项目功能

###  I2C扫描

修改main.c 13行定义的变量`app_int`数值为1 进入I2C扫描模式
- 扫描所有I2C地址从0x08到0x77
- 显示找到的设备地址和设备名称（仅展示常用）

### 动画演示模式

修改main.c 13行定义的变量`app_int`数值为2 进入动画演示

## 原理图

![](https://cloudflareimg.cdn.sn/i/69e5e7b3b57d6_1776674739.png)

## 项目状态和已知问题

### 工作正常的功能 ✅

- I2C设备扫描和显示
- TFT LCD图形和文本渲染
- 所有动画演示
- NOR Flash读写操作
- 系统初始化和外设驱动

- 中断处理程序目录为空

## 许可证

本项目包含多个许可证下的代码：

### CMSIS组件
CMSIS文件采用Apache License 2.0许可。详见`CMSIS/LICENSE.TXT`。

### GD32外设库
GD32E23x外设库文件由GigaDevice Semiconductor Inc.提供。请参考其官方文档了解许可条款。

## 资源

### 官方文档

- [GD32E230数据手册](https://www.gigadevice.com/)
- [GD32E23x用户手册](https://www.gigadevice.com/document/)
- [ARM Cortex-M23技术参考手册](https://developer.arm.com/documentation)

### 开发工具

- [Keil MDK](https://www.keil.com/arm/mdk.asp)
- [GD32开发工具](https://www.gigadevice.com/

## 支持

如有问题和需要支持：
- 查看[问题页面](https://github.com/yourusername/GD32E230/issues)
- 参考GD32官方手册

## 致谢

- GigaDevice Semiconductor提供GD32E230微控制器和库文件
- ARM Limited提供Cortex-M23架构和CMSIS
- Keil（ARM）提供MDK开发环境
- 特别感谢@MXL8876提供原理图及其底层
- 所有本项目的贡献者和用户
