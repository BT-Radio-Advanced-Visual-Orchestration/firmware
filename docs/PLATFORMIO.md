# PlatformIO Complete Beginner's Guide

## What is PlatformIO?

PlatformIO is a professional, cross-platform development environment for embedded systems and IoT (Internet of Things) devices. It's an ecosystem that provides:

- **Unified Development Environment**: Write, compile, and upload code to hundreds of different microcontroller boards
- **Library Manager**: Easily find, install, and manage thousands of libraries
- **Build System**: Automated compilation and dependency management
- **Project Management**: Organize multiple projects with different configurations
- **Debugging Tools**: Built-in debugging support for many platforms

Think of PlatformIO as a "Swiss Army knife" for embedded development - it handles all the complex toolchain management, so you can focus on writing code.

## Why Use PlatformIO?

### Compared to Arduino IDE

| Feature | Arduino IDE | PlatformIO |
|---------|------------|------------|
| Board Support | Limited to Arduino boards | 1000+ boards (ESP32, STM32, Arduino, etc.) |
| Library Management | Manual installation | Automatic dependency resolution |
| Multi-Project | No | Yes, with separate configurations |
| Build System | Basic | Advanced with optimizations |
| Code Editor | Basic | Full-featured (VS Code, CLion, etc.) |
| Command Line | Limited | Full CLI support |
| Testing | No | Built-in unit testing framework |
| Debugging | Limited | Full debugging support |

### Key Benefits

1. **One Tool for Everything**: ESP32, Arduino, STM32, Teensy, etc. - all in one environment
2. **Dependency Management**: Libraries are installed per-project, no global conflicts
3. **Professional Tools**: Integration with VS Code, CLion, Atom, and more
4. **Reproducible Builds**: Easy to share projects with exact dependencies
5. **CI/CD Ready**: Perfect for automated testing and deployment

## Installation

### Prerequisites

- **Operating System**: Windows, macOS, or Linux
- **Python 3.6+**: Required for PlatformIO Core
- **Internet Connection**: For downloading tools and libraries

### Method 1: VS Code Extension (Recommended for Beginners)

1. **Install Visual Studio Code**
   - Download from: https://code.visualstudio.com/
   - Install and launch VS Code

2. **Install PlatformIO Extension**
   - Open VS Code
   - Click Extensions icon (left sidebar) or press `Ctrl+Shift+X` (Windows/Linux) or `Cmd+Shift+X` (macOS)
   - Search for "PlatformIO IDE"
   - Click "Install" on the official PlatformIO extension
   - Wait for installation to complete (this may take several minutes)

3. **Verify Installation**
   - Look for the PlatformIO icon (alien head) in the left sidebar
   - Click it to open the PlatformIO Home page
   - If you see the home page, installation was successful!

### Method 2: Command Line Installation

If you prefer the command line or want to use PlatformIO without an IDE:

```bash
# Install PlatformIO Core using pip
pip install platformio

# Or using pipx (recommended for isolation)
pipx install platformio

# Verify installation
pio --version
```

**Note**: You may need to add PlatformIO to your PATH after installation.

## Understanding PlatformIO Project Structure

Every PlatformIO project has a standard structure:

```
my-project/
├── platformio.ini       # Project configuration file (IMPORTANT!)
├── src/                 # Your source code goes here
│   └── main.cpp        # Main program file
├── include/            # Header files (.h files)
│   └── myheader.h
├── lib/                # Private libraries (optional)
│   └── MyLib/
│       ├── MyLib.h
│       └── MyLib.cpp
├── test/               # Unit tests (optional)
│   └── test_main.cpp
└── .pio/               # Build files (auto-generated, don't commit!)
    ├── build/          # Compiled binaries
    └── libdeps/        # Downloaded libraries
```

### Key Files

1. **platformio.ini**: The heart of your project
   - Defines which board you're using
   - Specifies library dependencies
   - Sets compilation options
   - Configures upload settings

2. **src/main.cpp**: Your main program
   - Must contain `setup()` and `loop()` functions
   - Similar to Arduino .ino files but C++ format

3. **include/**: Header files for your code
   - Put your `.h` files here
   - Automatically included in compilation

## Creating Your First Project

### Using VS Code

1. **Open PlatformIO Home**
   - Click the PlatformIO icon (left sidebar)
   - Click "Home" button in the bottom toolbar

2. **Create New Project**
   - Click "New Project" button
   - Fill in the form:
     - **Name**: My First Project
     - **Board**: Start typing your board name (e.g., "ESP32 Dev Module")
     - **Framework**: Arduino (most common for beginners)
   - Click "Finish"
   - Wait for project initialization (downloads tools and framework)

3. **Write Your Code**
   - Open `src/main.cpp`
   - You'll see a template with `setup()` and `loop()` functions
   - Write your code!

### Using Command Line

```bash
# Create a new project directory
mkdir my-project
cd my-project

# Initialize a PlatformIO project
pio project init --board esp32dev

# This creates the basic project structure
```

## Understanding platformio.ini

This is the configuration file that controls everything about your project.

### Basic Example

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

Let's break this down:

- **[env:esp32dev]**: Environment name (you can have multiple)
- **platform**: Which chip family (espressif32 = ESP32)
- **board**: Specific board model
- **framework**: Programming framework (Arduino, ESP-IDF, etc.)

### Common Configuration Options

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

; Serial Monitor settings
monitor_speed = 115200
monitor_filters = esp32_exception_decoder

; Build options
build_flags = 
    -D DEBUG_MODE=1
    -D MY_CUSTOM_DEFINE

; Library dependencies
lib_deps = 
    bblanchon/ArduinoJson@^6.21.3
    adafruit/Adafruit BusIO@^1.14.1

; Upload settings
upload_speed = 921600
upload_port = /dev/ttyUSB0  ; Linux/macOS
; upload_port = COM3        ; Windows
```

### Configuration Breakdown

**Serial Monitor:**
```ini
monitor_speed = 115200          # Baud rate for Serial.begin()
monitor_filters = 
    esp32_exception_decoder     # Decode ESP32 crash dumps
    colorize                    # Color-code log output
    time                        # Add timestamps
```

**Build Flags:**
```ini
build_flags = 
    -D CORE_DEBUG_LEVEL=5       # Enable verbose logging
    -D MY_VALUE=42              # Define preprocessor constants
    -Wall                       # Enable all warnings
```

**Library Dependencies:**
```ini
lib_deps = 
    ; Format: owner/library@version
    bblanchon/ArduinoJson@^6.21.3   # Compatible with 6.21.3+
    adafruit/Adafruit BusIO         # Latest version
    https://github.com/user/repo.git # From GitHub
```

**Upload Options:**
```ini
upload_speed = 921600           # Faster upload
upload_protocol = esptool       # Upload method
upload_port = COM3              # Which serial port
```

## Working with Libraries

### Finding Libraries

**Method 1: PlatformIO Registry**

Visit: https://registry.platformio.org/

Search for libraries by name, keyword, or functionality.

**Method 2: Command Line**

```bash
# Search for a library
pio pkg search "ArduinoJson"

# Show library details
pio pkg show bblanchon/ArduinoJson
```

**Method 3: VS Code**

- Open PlatformIO Home
- Click "Libraries" tab
- Search for your library
- Click "Install" and select your project

### Installing Libraries

**Automatic (Recommended):**

Add to `platformio.ini`:

```ini
lib_deps = 
    bblanchon/ArduinoJson@^6.21.3
```

Then build your project - PlatformIO automatically downloads the library.

**Manual:**

```bash
# Install a library
pio pkg install --library "bblanchon/ArduinoJson@^6.21.3"
```

### Using Libraries in Code

```cpp
#include <Arduino.h>
#include <ArduinoJson.h>  // Installed library

void setup() {
    Serial.begin(115200);
    
    // Use the library
    StaticJsonDocument<200> doc;
    doc["sensor"] = "temperature";
    doc["value"] = 23.5;
    
    serializeJson(doc, Serial);
}

void loop() {
    // Your code
}
```

## Building and Uploading

### Using VS Code

**Build (Compile):**
- Click the checkmark (✓) icon in the bottom toolbar
- Or press `Ctrl+Alt+B` (Windows/Linux) or `Cmd+Shift+B` (macOS)
- Output appears in the terminal below

**Upload:**
- Connect your board via USB
- Click the arrow (→) icon in the bottom toolbar
- Or press `Ctrl+Alt+U`
- PlatformIO automatically detects the port

**Serial Monitor:**
- Click the plug icon in the bottom toolbar
- Or press `Ctrl+Alt+S`
- View output from `Serial.print()` statements

### Using Command Line

```bash
# Build the project
pio run

# Upload to the device
pio run --target upload

# Open serial monitor
pio device monitor

# Build, upload, and monitor in one command
pio run --target upload && pio device monitor

# Clean build files
pio run --target clean
```

### Common Build Targets

```bash
pio run --target upload        # Upload firmware
pio run --target clean         # Clean build files
pio run --target monitor       # Serial monitor
pio run --target uploadfs      # Upload filesystem (SPIFFS/LittleFS)
pio run --target erase         # Erase flash memory
pio run --target fuses         # Set board fuses
```

## For This B.R.A.V.O Project

### Our Project Configuration

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

monitor_speed = 115200
monitor_filters = esp32_exception_decoder

build_flags = 
    -D CORE_DEBUG_LEVEL=3
    -D CONFIG_ARDUHAL_LOG_COLORS=1

lib_deps = 
    sandeepmistry/LoRa@^0.8.0
    mikalhart/TinyGPSPlus@^1.0.3
    adafruit/Adafruit MPU6050@^2.2.4
    adafruit/Adafruit BusIO@^1.14.1
    bblanchon/ArduinoJson@^6.21.3
    h2zero/NimBLE-Arduino@^1.4.1

upload_speed = 921600
```

### Quick Start for This Project

```bash
# 1. Clone the repository
git clone https://github.com/beacon-relay-asset-view-orchestration/firmware.git
cd firmware

# 2. Open in VS Code with PlatformIO
code .

# Or initialize from command line
# pio project init

# 3. Build the project
pio run

# 4. Connect your ESP32 via USB

# 5. Upload the firmware
pio run --target upload

# 6. Monitor serial output
pio device monitor
```

### Customizing for Your Device

Edit `src/main.cpp`:

```cpp
// Device configuration
#define DEVICE_ID           "BRAVO_001"  // Change this for each device
#define DEVICE_TYPE_COLLAR  true         // Set to false for dongle
```

## Troubleshooting

### Common Issues and Solutions

**1. "Command 'pio' not found"**

Solution:
```bash
# Make sure PlatformIO is installed
pip install platformio

# Add to PATH (Linux/macOS)
export PATH=$PATH:~/.platformio/penv/bin

# Add to PATH (Windows)
# Add C:\Users\YourName\.platformio\penv\Scripts to system PATH
```

**2. "Could not find a board with ID 'esp32dev'"**

Solution:
```bash
# Update platforms
pio platform update

# Or install specific platform
pio platform install espressif32
```

**3. "Library not found"**

Solution:
```bash
# Install libraries
pio pkg install

# Or update library registry
pio pkg update
```

**4. "Serial port not found" / "Could not open port"**

Solution:
- Check USB cable (data cable, not charge-only)
- Install USB drivers:
  - ESP32: CP210x or CH340 drivers
  - Arduino: FTDI or CH340 drivers
- Linux: Add user to dialout group: `sudo usermod -a -G dialout $USER`
- Check which port: `pio device list`

**5. "Upload fails" / "Timeout"**

Solution:
- Hold BOOT button on ESP32 while uploading
- Reduce upload speed in platformio.ini: `upload_speed = 115200`
- Try different USB port
- Check serial monitor isn't open (close it before upload)

**6. "Out of memory" errors**

Solution:
```ini
; Increase partition size
board_build.partitions = huge_app.csv

; Or use custom partition
board_build.partitions = custom_partitions.csv
```

## Advanced Topics

### Multiple Environments

You can have different configurations for different boards:

```ini
; Configuration for collar device
[env:collar]
platform = espressif32
board = esp32dev
framework = arduino
build_flags = -D DEVICE_TYPE_COLLAR=1

; Configuration for dongle
[env:dongle]
platform = espressif32
board = esp32dev
framework = arduino
build_flags = -D DEVICE_TYPE_DONGLE=1
```

Build specific environment:
```bash
pio run -e collar
pio run -e dongle --target upload
```

### Custom Upload Port

```ini
; Auto-detect (default)
; upload_port = auto

; Specific port
upload_port = COM3              ; Windows
upload_port = /dev/ttyUSB0      ; Linux
upload_port = /dev/cu.usbserial ; macOS
```

### Debugging

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

; Enable debugging
debug_tool = esp-prog
debug_init_break = tbreak setup

; Debug build flags
build_type = debug
build_flags = -g -O0
```

### Unit Testing

Create tests in `test/` directory:

```cpp
// test/test_main.cpp
#include <Arduino.h>
#include <unity.h>

void test_addition(void) {
    TEST_ASSERT_EQUAL(2, 1 + 1);
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_addition);
    UNITY_END();
}

void loop() {
    // Nothing here
}
```

Run tests:
```bash
pio test
```

### Over-The-Air (OTA) Updates

For network-based updates:

```ini
[env:esp32dev]
upload_protocol = espota
upload_port = 192.168.1.100  ; Device IP address
upload_flags = 
    --auth=password123        ; OTA password
```

## Best Practices

### 1. Use Version Control

Always use Git with PlatformIO:

```gitignore
# .gitignore for PlatformIO projects
.pio/
.vscode/
*.pyc
__pycache__/
```

### 2. Lock Library Versions

Use specific versions in production:

```ini
lib_deps = 
    bblanchon/ArduinoJson@6.21.3    ; Exact version
    ; NOT: bblanchon/ArduinoJson    ; Could break later
```

### 3. Organize Code

```
src/
├── main.cpp           # Main program
├── config.h           # Configuration constants
├── network.cpp        # Network functions
└── sensors.cpp        # Sensor functions

include/
├── network.h
└── sensors.h
```

### 4. Use Build Flags for Configuration

```ini
build_flags = 
    -D VERSION="2.0.0"
    -D DEBUG_MODE=1
    -D WIFI_SSID=\"MyNetwork\"
```

Then in code:
```cpp
#ifdef DEBUG_MODE
    Serial.println("Debug mode enabled");
#endif
```

### 5. Test Before Deploying

Always test on one device before updating multiple devices:

```bash
# Build
pio run

# Upload to test device
pio run --target upload

# Monitor output
pio device monitor

# If successful, deploy to others
```

## Continuous Integration (CI/CD)

PlatformIO works great with CI systems:

### GitHub Actions Example

```yaml
# .github/workflows/build.yml
name: Build Firmware

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - uses: actions/setup-python@v2
      - name: Install PlatformIO
        run: pip install platformio
      - name: Build
        run: pio run
```

## Resources and Learning

### Official Documentation

- **PlatformIO Core**: https://docs.platformio.org/en/latest/core/index.html
- **PlatformIO IDE**: https://docs.platformio.org/en/latest/integration/ide/vscode.html
- **Library Registry**: https://registry.platformio.org/
- **Boards**: https://docs.platformio.org/en/latest/boards/index.html

### Community Resources

- **Forum**: https://community.platformio.org/
- **GitHub**: https://github.com/platformio
- **Blog**: https://platformio.org/blog/

### ESP32 Specific

- **ESP32 Platform**: https://docs.platformio.org/en/latest/platforms/espressif32.html
- **ESP32 Arduino Core**: https://github.com/espressif/arduino-esp32

### Tutorials

- **PlatformIO Tutorials**: https://docs.platformio.org/en/latest/tutorials/index.html
- **Getting Started**: https://platformio.org/platformio-ide

## Quick Reference Card

### Essential Commands

```bash
# Project Management
pio project init                  # Initialize new project
pio project init --board esp32dev # Initialize for specific board

# Building
pio run                          # Build project
pio run -e collar                # Build specific environment
pio run --target clean           # Clean build files

# Uploading
pio run --target upload          # Upload firmware
pio run -e collar --target upload # Upload specific environment

# Monitoring
pio device list                  # List connected devices
pio device monitor               # Serial monitor
pio device monitor -b 115200     # Monitor at specific baud rate

# Libraries
pio pkg search "keyword"         # Search libraries
pio pkg install                  # Install dependencies
pio pkg install --library "name@version" # Install specific library
pio pkg update                   # Update libraries

# Testing
pio test                         # Run tests
pio test -e collar               # Run tests for environment

# Utilities
pio platform list                # List installed platforms
pio platform install espressif32 # Install platform
pio platform update              # Update platforms
pio --version                    # Show version
```

### VS Code Shortcuts

| Action | Windows/Linux | macOS |
|--------|--------------|-------|
| Build | Ctrl+Alt+B | Cmd+Shift+B |
| Upload | Ctrl+Alt+U | Cmd+Shift+U |
| Serial Monitor | Ctrl+Alt+S | Cmd+Shift+S |
| Clean | Ctrl+Alt+C | Cmd+Shift+C |
| Test | Ctrl+Alt+T | Cmd+Shift+T |

## Conclusion

PlatformIO is a powerful tool that makes embedded development much easier. Key takeaways:

1. **One tool for many platforms** - ESP32, Arduino, STM32, and more
2. **Professional features** - Library management, testing, debugging
3. **Easy to learn** - Start with simple projects, grow into advanced features
4. **Great for teams** - Reproducible builds, CI/CD integration

### Getting Help

If you get stuck:
1. Check the error message carefully
2. Search the PlatformIO documentation
3. Ask on the PlatformIO community forum
4. Check GitHub issues for similar problems

### Next Steps

1. Install PlatformIO (VS Code or CLI)
2. Create your first project
3. Try the B.R.A.V.O firmware project
4. Experiment with different libraries
5. Share your projects!

Happy coding! 🚀
