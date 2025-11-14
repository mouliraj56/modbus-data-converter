# Modbus Data Conversion Library

A lightweight, portable C library for converting Modbus register data into various data types with comprehensive endianness support.

you can also visit [Online Modbus Data Converter](https://mouliraj56.github.io/modbus-data-converter/) which implements the same library. (ported to js)

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
- [Data Types](#data-types)
- [Byte Order (Endianness)](#byte-order-endianness)
- [Usage Examples](#usage-examples)
- [Error Handling](#error-handling)
- [Integration](#integration)
- [License](#license)

## 🎯 Overview

This library provides robust conversion functions for Modbus RTU/TCP register data. It handles all common data types used in industrial automation, SCADA systems, and IIoT applications.

**Key Use Cases:**
- Modbus RTU/TCP data parsing
- SCADA system integration
- PLC data interpretation
- Industrial sensor data conversion
- IIoT gateway applications

## ✨ Features

- ✅ **Zero dependencies** - Pure C implementation
- ✅ **Portable** - C99 compliant, works on embedded systems
- ✅ **Comprehensive** - Supports all common Modbus data types
- ✅ **Flexible endianness** - Multiple byte order variants
- ✅ **Type-safe** - Strong typing with unions
- ✅ **Error handling** - Detailed error codes
- ✅ **Scaling support** - Built-in scaling factor application
- ✅ **Small footprint** - Minimal memory usage

## 🚀 Quick Start

### 1. Include the library

```c
#include "modbus_conversion.h"
```

### 2. Basic usage

```c
// Example: Convert two Modbus registers to a 32-bit float
uint16_t registers[2] = {0x4049, 0x0FDB};  // PI as IEEE 754 float
modbus_value_t result;

int status = modbus_convert(
    registers,                      // Register array
    2,                              // Number of registers
    MODBUS_IEEE_FLOAT32_ABCD,      // Data type
    0,                              // Bit position (unused for floats)
    1.0,                            // Scaling factor
    &result                         // Result pointer
);

if (status == MODBUS_CONV_OK) {
    printf("Float value: %f\n", result.f32);  // Output: 3.141593
}
```

## 📚 API Reference

### Main Conversion Function

```c
int modbus_convert(
    const uint16_t *registers,      // Input: Array of Modbus registers
    size_t reg_count,               // Input: Number of registers
    modbus_data_type_t data_type,   // Input: Target data type
    uint8_t bit_pos,                // Input: Bit position (0-15, for bit types only)
    double scaling_factor,          // Input: Scaling multiplier
    modbus_value_t *result          // Output: Conversion result
);
```

**Returns:**
- `MODBUS_CONV_OK` (0) on success
- Negative error code on failure

### Specialized Conversion Functions

```c
// Bit boolean
int modbus_convert_bit_bool(const uint16_t *registers, uint8_t bit_pos, bool *result);

// 8-bit integers
int modbus_convert_int8_signed(const uint16_t *registers, double scaling_factor, int8_t *result);
int modbus_convert_int8_unsigned(const uint16_t *registers, double scaling_factor, uint8_t *result);

// 16-bit integers
int modbus_convert_int16_signed(const uint16_t *registers, bool swap_bytes, double scaling_factor, int16_t *result);
int modbus_convert_int16_unsigned(const uint16_t *registers, bool swap_bytes, double scaling_factor, uint16_t *result);

// 32-bit integers
int modbus_convert_int32(const uint16_t *registers, modbus_data_type_t data_type, double scaling_factor, modbus_value_t *result);

// 64-bit integers
int modbus_convert_int64(const uint16_t *registers, modbus_data_type_t data_type, double scaling_factor, modbus_value_t *result);

// IEEE 754 floats
int modbus_convert_float32(const uint16_t *registers, modbus_data_type_t data_type, double scaling_factor, float *result);
int modbus_convert_float64(const uint16_t *registers, modbus_data_type_t data_type, double scaling_factor, double *result);
```

### Error Handling

```c
const char* modbus_conv_get_error_string(int error_code);
```

## 📊 Data Types

### Bit Types
- `MODBUS_BIT_BOOLEAN` - Single bit boolean value (0-15)

### 8-bit Integers
- `MODBUS_INT8_SIGNED` - Signed 8-bit integer (-128 to 127)
- `MODBUS_INT8_UNSIGNED` - Unsigned 8-bit integer (0 to 255)

### 16-bit Integers
- `MODBUS_INT16_SIGNED_AB` - Signed, Big Endian
- `MODBUS_INT16_SIGNED_BA` - Signed, Little Endian
- `MODBUS_INT16_UNSIGNED_AB` - Unsigned, Big Endian
- `MODBUS_INT16_UNSIGNED_BA` - Unsigned, Little Endian

### 32-bit Integers (4 byte orders)
- `MODBUS_INT32_SIGNED_ABCD` / `MODBUS_INT32_UNSIGNED_ABCD` - Big Endian
- `MODBUS_INT32_SIGNED_DCBA` / `MODBUS_INT32_UNSIGNED_DCBA` - Little Endian
- `MODBUS_INT32_SIGNED_BADC` / `MODBUS_INT32_UNSIGNED_BADC` - Mid-Big Endian
- `MODBUS_INT32_SIGNED_CDAB` / `MODBUS_INT32_UNSIGNED_CDAB` - Mid-Little Endian

### 64-bit Integers (8 byte orders)
- `MODBUS_INT64_SIGNED_ABCDEFGH` / `MODBUS_INT64_UNSIGNED_ABCDEFGH`
- `MODBUS_INT64_SIGNED_HGFEDCBA` / `MODBUS_INT64_UNSIGNED_HGFEDCBA`
- `MODBUS_INT64_SIGNED_BADCFEHG` / `MODBUS_INT64_UNSIGNED_BADCFEHG`
- `MODBUS_INT64_SIGNED_CDABGHEF` / `MODBUS_INT64_UNSIGNED_CDABGHEF`
- `MODBUS_INT64_SIGNED_DCBAHGFE` / `MODBUS_INT64_UNSIGNED_DCBAHGFE`
- `MODBUS_INT64_SIGNED_GHEFCDAB` / `MODBUS_INT64_UNSIGNED_GHEFCDAB`
- `MODBUS_INT64_SIGNED_FEHGBADC` / `MODBUS_INT64_UNSIGNED_FEHGBADC`
- `MODBUS_INT64_SIGNED_EFGHABCD` / `MODBUS_INT64_UNSIGNED_EFGHABCD`

### IEEE 754 Floats
**32-bit (4 byte orders):**
- `MODBUS_IEEE_FLOAT32_ABCD`, `MODBUS_IEEE_FLOAT32_CDAB`
- `MODBUS_IEEE_FLOAT32_DCBA`, `MODBUS_IEEE_FLOAT32_BADC`

**64-bit (8 byte orders):**
- `MODBUS_IEEE_FLOAT64_ABCDEFGH`, `MODBUS_IEEE_FLOAT64_HGFEDCBA`
- `MODBUS_IEEE_FLOAT64_BADCFEHG`, `MODBUS_IEEE_FLOAT64_CDABGHEF`
- `MODBUS_IEEE_FLOAT64_DCBAHGFE`, `MODBUS_IEEE_FLOAT64_GHEFCDAB`
- `MODBUS_IEEE_FLOAT64_FEHGBADC`, `MODBUS_IEEE_FLOAT64_EFGHABCD`

## 🔄 Byte Order (Endianness)

Understanding byte order is crucial for correct data interpretation in Modbus systems.

### What is Byte Order?

Byte order (endianness) determines how multi-byte values are stored in memory:

**Example: 32-bit value `0x40490FDB` (PI as float)**

| Byte Order | Register 0 | Register 1 | Byte Sequence | Description |
|------------|------------|------------|---------------|-------------|
| **ABCD** | 0x4049 | 0x0FDB | 40 49 0F DB | Big Endian (Most Common) |
| **DCBA** | 0xDB0F | 0x4940 | DB 0F 49 40 | Little Endian |
| **BADC** | 0x4940 | 0xDB0F | 49 40 DB 0F | Mid-Big Endian |
| **CDAB** | 0x0FDB | 0x4049 | 0F DB 40 49 | Mid-Little Endian |

### How to Choose the Right Byte Order?

1. **Check device documentation** - Most devices specify their byte order
2. **Test with known values** - Use a known value (like 100.0) and try different orders
3. **Common defaults:**
   - **Schneider Electric**: ABCD (Big Endian)
   - **Siemens**: CDAB (Mid-Little Endian)
   - **Allen-Bradley**: BADC (Mid-Big Endian)
   - **Modicon**: ABCD (Big Endian)

## 💡 Usage Examples

### Example 1: Reading Temperature (16-bit signed)

```c
#include "modbus_conversion.h"

// Temperature sensor returns value in 0.1°C units
uint16_t registers[1] = {0x00FA};  // 250 in decimal
modbus_value_t result;

int status = modbus_convert(
    registers,
    1,
    MODBUS_INT16_SIGNED_AB,
    0,
    0.1,  // Scaling: convert to actual °C
    &result
);

if (status == MODBUS_CONV_OK) {
    printf("Temperature: %.1f°C\n", result.i16 * 0.1);  // Output: 25.0°C
}
```

### Example 2: Reading Power (32-bit float)

```c
// Power meter returns IEEE 754 float in CDAB byte order
uint16_t registers[2] = {0x0FDB, 0x4049};  // PI in CDAB order
modbus_value_t result;

int status = modbus_convert(
    registers,
    2,
    MODBUS_IEEE_FLOAT32_CDAB,
    0,
    1.0,
    &result
);

if (status == MODBUS_CONV_OK) {
    printf("Power: %.2f kW\n", result.f32);  // Output: 3.14 kW
}
```

### Example 3: Reading Digital Input (Bit Boolean)

```c
// Read bit 5 from register
uint16_t registers[1] = {0x0020};  // Binary: 0000 0000 0010 0000
modbus_value_t result;

int status = modbus_convert(
    registers,
    1,
    MODBUS_BIT_BOOLEAN,
    5,  // Bit position
    1.0,
    &result
);

if (status == MODBUS_CONV_OK) {
    printf("Input 5: %s\n", result.bool_val ? "ON" : "OFF");  // Output: ON
}
```

### Example 4: Reading Energy Counter (64-bit unsigned)

```c
// Energy meter with 64-bit counter in ABCDEFGH order
uint16_t registers[4] = {0x0000, 0x0000, 0x0000, 0x03E8};  // 1000 in decimal
modbus_value_t result;

int status = modbus_convert(
    registers,
    4,
    MODBUS_INT64_UNSIGNED_ABCDEFGH,
    0,
    0.001,  // Scaling: convert to kWh
    &result
);

if (status == MODBUS_CONV_OK) {
    printf("Energy: %llu kWh\n", (unsigned long long)(result.u64 * 0.001));
}
```

### Example 5: Error Handling

```c
uint16_t registers[2] = {0x4049, 0x0FDB};
modbus_value_t result;

int status = modbus_convert(
    registers,
    2,
    MODBUS_IEEE_FLOAT32_ABCD,
    0,
    1.0,
    &result
);

if (status != MODBUS_CONV_OK) {
    fprintf(stderr, "Conversion error: %s\n",
            modbus_conv_get_error_string(status));
    return -1;
}

// Success - use result
printf("Value: %f\n", result.f32);
```

## ⚠️ Error Handling

### Error Codes

| Code | Constant | Description |
|------|----------|-------------|
| 0 | `MODBUS_CONV_OK` | Success |
| -1 | `MODBUS_CONV_ERR_NULL_PTR` | Null pointer passed |
| -2 | `MODBUS_CONV_ERR_INVALID_TYPE` | Invalid data type |
| -3 | `MODBUS_CONV_ERR_INVALID_BIT` | Invalid bit position (must be 0-15) |
| -4 | `MODBUS_CONV_ERR_INSUFF_REGS` | Insufficient registers for conversion |
| -5 | `MODBUS_CONV_ERR_UNKNOWN` | Unknown error |

### Best Practices

```c
// Always check return values
int status = modbus_convert(...);
if (status != MODBUS_CONV_OK) {
    // Handle error
    const char* error_msg = modbus_conv_get_error_string(status);
    // Log or handle error appropriately
}

// Validate inputs before calling
if (registers == NULL || result == NULL) {
    return MODBUS_CONV_ERR_NULL_PTR;
}

// Ensure sufficient registers
if (reg_count < 2) {
    return MODBUS_CONV_ERR_INSUFF_REGS;
}
```

## 🔧 Integration

### Compilation

**Simple compilation:**
```bash
gcc -c modbus_conversion.c -o modbus_conversion.o
gcc your_application.c modbus_conversion.o -o your_app
```

**With optimization:**
```bash
gcc -O2 -c modbus_conversion.c -o modbus_conversion.o
gcc -O2 your_application.c modbus_conversion.o -o your_app
```

**For embedded systems (ARM):**
```bash
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -c modbus_conversion.c
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb your_app.c modbus_conversion.o -o your_app.elf
```

### Makefile Example

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99

OBJS = modbus_conversion.o your_application.o

all: your_app

your_app: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o your_app

.PHONY: all clean
```

### CMake Example

```cmake
cmake_minimum_required(VERSION 3.10)
project(ModbusApp C)

set(CMAKE_C_STANDARD 99)

add_library(modbus_conversion STATIC
    modbus_conversion.c
    modbus_conversion.h
)

add_executable(your_app
    your_application.c
)

target_link_libraries(your_app modbus_conversion)
```

### Arduino/PlatformIO Integration

```cpp
// In your Arduino sketch or PlatformIO project
extern "C" {
    #include "modbus_conversion.h"
}

void setup() {
    Serial.begin(115200);

    uint16_t registers[2] = {0x4049, 0x0FDB};
    modbus_value_t result;

    int status = modbus_convert(
        registers, 2,
        MODBUS_IEEE_FLOAT32_ABCD,
        0, 1.0, &result
    );

    if (status == MODBUS_CONV_OK) {
        Serial.print("Value: ");
        Serial.println(result.f32);
    }
}
```

## 📝 Register Requirements

Different data types require different numbers of registers:

| Data Type | Registers Required | Bytes |
|-----------|-------------------|-------|
| Bit Boolean | 1 | 2 |
| 8-bit Integer | 1 | 1 (uses low byte) |
| 16-bit Integer | 1 | 2 |
| 32-bit Integer | 2 | 4 |
| 32-bit Float | 2 | 4 |
| 64-bit Integer | 4 | 8 |
| 64-bit Float | 4 | 8 |

## 🎯 Common Use Cases

### SCADA Systems
```c
// Reading multiple sensor values from a PLC
uint16_t temp_regs[1], pressure_regs[2], flow_regs[2];
modbus_value_t temp, pressure, flow;

// Temperature (16-bit signed, 0.1°C resolution)
modbus_convert(temp_regs, 1, MODBUS_INT16_SIGNED_AB, 0, 0.1, &temp);

// Pressure (32-bit float, CDAB order)
modbus_convert(pressure_regs, 2, MODBUS_IEEE_FLOAT32_CDAB, 0, 1.0, &pressure);

// Flow rate (32-bit unsigned, ABCD order)
modbus_convert(flow_regs, 2, MODBUS_INT32_UNSIGNED_ABCD, 0, 0.01, &flow);
```

### Energy Monitoring
```c
// Reading power meter data
uint16_t voltage_regs[2], current_regs[2], power_regs[2], energy_regs[4];
modbus_value_t voltage, current, power, energy;

modbus_convert(voltage_regs, 2, MODBUS_IEEE_FLOAT32_ABCD, 0, 1.0, &voltage);
modbus_convert(current_regs, 2, MODBUS_IEEE_FLOAT32_ABCD, 0, 1.0, &current);
modbus_convert(power_regs, 2, MODBUS_IEEE_FLOAT32_ABCD, 0, 1.0, &power);
modbus_convert(energy_regs, 4, MODBUS_INT64_UNSIGNED_ABCDEFGH, 0, 0.001, &energy);

printf("Voltage: %.2f V\n", voltage.f32);
printf("Current: %.2f A\n", current.f32);
printf("Power: %.2f kW\n", power.f32);
printf("Energy: %llu kWh\n", (unsigned long long)energy.u64);
```

### Digital I/O
```c
// Reading 16 digital inputs from one register
uint16_t di_register[1] = {0xA5F0};  // Binary: 1010 0101 1111 0000
modbus_value_t bit_value;

for (int bit = 0; bit < 16; bit++) {
    modbus_convert_bit_bool(di_register, bit, &bit_value.bool_val);
    printf("Input %2d: %s\n", bit, bit_value.bool_val ? "ON" : "OFF");
}
```

## 🔍 Troubleshooting

### Problem: Getting incorrect values

**Solution:** Check byte order
```c
// Try all 4 byte orders for 32-bit values
modbus_data_type_t orders[] = {
    MODBUS_IEEE_FLOAT32_ABCD,
    MODBUS_IEEE_FLOAT32_CDAB,
    MODBUS_IEEE_FLOAT32_DCBA,
    MODBUS_IEEE_FLOAT32_BADC
};

for (int i = 0; i < 4; i++) {
    modbus_convert(registers, 2, orders[i], 0, 1.0, &result);
    printf("Order %d: %f\n", i, result.f32);
}
```

### Problem: Value seems scaled incorrectly

**Solution:** Adjust scaling factor
```c
// If device returns value * 10
modbus_convert(registers, 1, MODBUS_INT16_SIGNED_AB, 0, 0.1, &result);

// If device returns value * 100
modbus_convert(registers, 1, MODBUS_INT16_SIGNED_AB, 0, 0.01, &result);
```

### Problem: Negative values appearing as large positive

**Solution:** Use signed type instead of unsigned
```c
// Wrong: Using unsigned for temperature
modbus_convert(registers, 1, MODBUS_INT16_UNSIGNED_AB, 0, 1.0, &result);

// Correct: Using signed for temperature
modbus_convert(registers, 1, MODBUS_INT16_SIGNED_AB, 0, 1.0, &result);
```

## 📄 License

MIT License

## 🤝 Support

For issues, questions, or contributions, please refer to the project repository or contact the maintainer.

## 📚 Additional Resources

- [Modbus Protocol Specification](https://www.modbus.org/file/secure/modbusprotocolspecification.pdf)
- [IEEE 754 Floating Point Standard](https://en.wikipedia.org/wiki/IEEE_754)
- [Endianness Explained](https://en.wikipedia.org/wiki/Endianness)

---

**Version:** 1.0
**Last Updated:** 2025
**Compatibility:** C99 and later, embedded systems compatible


