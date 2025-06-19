# mi_thermald-mod

## Smart Battery Charging Controller

A custom thermal control daemon for MIUI/HyperOS systems that automatically adjusts charging power based on battery status, balancing charging speed and temperature control.

### Features

- Supports both Standard and Fast charging modes
- Dynamic power adjustment based on battery level
- Intelligent thermal management for battery longevity
- Open source and transparent codebase

### How It Works

The daemon automatically adjusts charging current based on battery capacity and charging type:
- Standard Charging: Maintains constant power up to 80%, then reduces power
- Fast Charging: Full power until 30%, linear reduction 30-80%, minimum power above 80%

### Build Instructions

Prerequisites:
- CMake (3.10+)
- GCC/Clang (Linux) or MSVC (Windows)

#### Linux/Unix Systems
```bash
chmod +x build.sh
./build.sh
```

#### Windows Systems
```cmd
build.bat
```

### ⚠️ Important Notes

1. **Do not use this program with enhanced charging modules** like "Quick Charge Boost". Such modes may conflict with this program and cause:
   - Minimal battery level increase
   - Rapid temperature rise
   Recommendation: Use official Standard or Fast charging modes.

2. **Avoid increasing power limits**. Excessive power can trigger thermal throttling, reducing overall charging efficiency.

3. **Download compiled binaries from [GitHub Actions](../../actions)**. New versions are auto-generated after each commit.

---

Please test the program on your device and use at your own risk.

