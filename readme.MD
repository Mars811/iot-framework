# 文件说明
该文件基于STM32F407板子，是一个多功能的IOT框架，旨在对多种IOT功能进行整合，并进行易于移植处理。按照说明即可对需要部分进行快速移植，提高开发效率。

支持的功能：
- FreeRTOS
- ESP8266 - WIFI
- MQTT
- LVGL
- OTA
- Deepseek API调用例程（待进一步融合）

# 移植说明
## RTOS
/main/Middlewares/freertos 是已适配 ​Cortex-M4 的 ​FreeRTOS v9 移植包，所有必要的移植修改均已包含。**只需将该文件夹完整复制到目标工程，并手动调整三个中断向量即可完成移植。**

为提升代码结构清晰度，新增了 rtos_app 部分，开发者可在 main() 函数中使用 rtos_init（）跳转后，即可直接基于任务模式进行编程。

## IOT
WIFI-MQTT 模块位于 /main/MyCodes/aliyun，完整实现了 ​WIFI 连接 → MQTT 连接 → MQTT 交互 的功能流程。所有配置参数均可通过 aliyun_config.h 统一修改。移植时，只需复制整个 aliyun 文件夹至新工程，并适配相关 API 接口即可。

## LVGL
LVGL 移植包位于 /main/Middlewares/LVGL，包含两个子目录：
- APP：用于存放 GUI Guider 生成的 UI 文件
- LVGL_SRC：LVGL 官方源码
当前移植尚未完全优化，除复制文件外，仍需手动调整部分配置。后续会进一步简化移植流程。

## OTA
OTA程序位于/boot处，是一份单独的程序，需要单独烧录，烧录后，即可实现程序的更新。

移植说明
- 直接复制整个boot文件夹，对bootloader程序进行编写
- 将usart2串口直接接入蓝牙透传模块
- 移植时，需要对main函数地址进行修改，修改为0x08004000，并且设置中断向量的偏移，以保证正常的转跳。
- 烧录时，需要将bootloader的地址设置为0x08000000，否则会导致程序无法正常烧录。

# 未来计划
未来会整合更多IOT的常见功能，进一步做更详细的说明和可移植性的优化处理。😊