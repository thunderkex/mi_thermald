# mi_thermald-mod

## Intelligent Charging Control Program

This project provides a custom thermal management daemon for MIUI / HyperOS systems. The program automatically adjusts charging power based on battery status, balancing charging speed and temperature control.

### Features

- Supports standard and fast charging modes
- Dynamically adjusts charging power based on battery level
- Smart thermal control to extend battery life
- Fully open source, transparent code

### How It Works

The program automatically adjusts charging current based on current battery capacity and charging type:
- Standard charging: Maintains constant power before 80%, then reduces power
- Fast charging: Full power before 30%, linear reduction from 30-80%, lowest power after 80%

This project is optimized based on @SutoLiu's original code, with focus on improving charging strategy and thermal control logic.

---

## ⚠️ Usage Precautions

1. **Do not use this program with enhanced modules like "Quick Charge for Low Battery" enabled**. Such modes may conflict with this program, causing charging anomalies, such as:  
   - Battery level barely increases  
   - Battery temperature rises rapidly  
   Recommend using **official standard mode or fast mode**.

2. **Not recommended to increase power limit**. Higher power will increase temperature, potentially triggering system "thermal wall", instantly dropping charging power to 0, slowing overall charging progress.

3. **Download compiled thermal control files from [GitHub Actions page](../../actions)**. New version of `mi_thermald` is auto-generated after each code commit. Just enter the corresponding build record and get it from the Artifacts section at the bottom.

---

### 编译说明

需要安装以下依赖：
- CMake (3.10+)
- GCC/Clang (Linux) 或 MSVC (Windows)

#### Linux/Unix 系统
```bash
chmod +x build.sh
./build.sh
```

#### Windows 系统
```cmd
build.bat
```

---

Please test the program's performance on your device and assume related risks.

