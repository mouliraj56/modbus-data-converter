/**
 * @file modbus_conversion.h
 * @brief Modbus Data Conversion Library
 * @details Converts raw Modbus register data into specified data types with endianness handling
 * @author Mouli Sai
 * @version 1.0
 */

#ifndef MODBUS_CONVERSION_H
#define MODBUS_CONVERSION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Return codes */
#define MODBUS_CONV_OK                  0
#define MODBUS_CONV_ERR_NULL_PTR       -1
#define MODBUS_CONV_ERR_INVALID_TYPE   -2
#define MODBUS_CONV_ERR_INVALID_BIT    -3
#define MODBUS_CONV_ERR_INSUFF_REGS    -4
#define MODBUS_CONV_ERR_UNKNOWN        -5

/* Data type definitions */
typedef enum {
    /* Bit types */
    MODBUS_BIT_BOOLEAN,
    
    /* 8-bit integers */
    MODBUS_INT8_SIGNED,
    MODBUS_INT8_UNSIGNED,
    
    /* 16-bit integers */
    MODBUS_INT16_SIGNED_AB,
    MODBUS_INT16_SIGNED_BA,
    MODBUS_INT16_UNSIGNED_AB,
    MODBUS_INT16_UNSIGNED_BA,
    
    /* 32-bit integers */
    MODBUS_INT32_SIGNED_ABCD,
    MODBUS_INT32_SIGNED_DCBA,
    MODBUS_INT32_SIGNED_BADC,
    MODBUS_INT32_SIGNED_CDAB,
    MODBUS_INT32_UNSIGNED_ABCD,
    MODBUS_INT32_UNSIGNED_DCBA,
    MODBUS_INT32_UNSIGNED_BADC,
    MODBUS_INT32_UNSIGNED_CDAB,
    
    /* 64-bit integers */
    MODBUS_INT64_SIGNED_ABCDEFGH,
    MODBUS_INT64_SIGNED_HGFEDCBA,
    MODBUS_INT64_SIGNED_BADCFEHG,
    MODBUS_INT64_SIGNED_CDABGHEF,
    MODBUS_INT64_SIGNED_DCBAHGFE,
    MODBUS_INT64_SIGNED_GHEFCDAB,
    MODBUS_INT64_SIGNED_FEHGBADC,
    MODBUS_INT64_SIGNED_EFGHABCD,
    MODBUS_INT64_UNSIGNED_ABCDEFGH,
    MODBUS_INT64_UNSIGNED_HGFEDCBA,
    MODBUS_INT64_UNSIGNED_BADCFEHG,
    MODBUS_INT64_UNSIGNED_CDABGHEF,
    MODBUS_INT64_UNSIGNED_DCBAHGFE,
    MODBUS_INT64_UNSIGNED_GHEFCDAB,
    MODBUS_INT64_UNSIGNED_FEHGBADC,
    MODBUS_INT64_UNSIGNED_EFGHABCD,
    
    /* IEEE float types */
    MODBUS_IEEE_FLOAT32_ABCD,
    MODBUS_IEEE_FLOAT32_CDAB,
    MODBUS_IEEE_FLOAT32_DCBA,
    MODBUS_IEEE_FLOAT32_BADC,
    MODBUS_IEEE_FLOAT64_ABCDEFGH,
    MODBUS_IEEE_FLOAT64_HGFEDCBA,
    MODBUS_IEEE_FLOAT64_BADCFEHG,
    MODBUS_IEEE_FLOAT64_CDABGHEF,
    MODBUS_IEEE_FLOAT64_DCBAHGFE,
    MODBUS_IEEE_FLOAT64_GHEFCDAB,
    MODBUS_IEEE_FLOAT64_FEHGBADC,
    MODBUS_IEEE_FLOAT64_EFGHABCD
} modbus_data_type_t;

/* Union for conversion results */
typedef union {
    bool bool_val;
    int8_t i8;
    uint8_t u8;
    int16_t i16;
    uint16_t u16;
    int32_t i32;
    uint32_t u32;
    int64_t i64;
    uint64_t u64;
    float f32;
    double f64;
} modbus_value_t;

/**
 * @brief Convert Modbus registers to specified data type
 * @param registers Array of 16-bit register values
 * @param reg_count Number of registers in array
 * @param data_type Type of conversion to perform
 * @param bit_pos Bit position for bit-type conversions (0-15)
 * @param scaling_factor Multiplier to apply after conversion
 * @param result Pointer to store conversion result
 * @return MODBUS_CONV_OK on success, error code otherwise
 */
int modbus_convert(const uint16_t *registers, 
                   size_t reg_count,
                   modbus_data_type_t data_type,
                   uint8_t bit_pos,
                   double scaling_factor,
                   modbus_value_t *result);

/**
 * @brief Convert Modbus registers to boolean bit value
 * @param registers Array of 16-bit register values
 * @param bit_pos Bit position (0-15)
 * @param result Pointer to store result
 * @return MODBUS_CONV_OK on success, error code otherwise
 */
int modbus_convert_bit_bool(const uint16_t *registers,
                             uint8_t bit_pos,
                             bool *result);

/**
 * @brief Convert Modbus registers to 8-bit signed integer
 * @param registers Array of 16-bit register values
 * @param scaling_factor Multiplier to apply
 * @param result Pointer to store result
 * @return MODBUS_CONV_OK on success, error code otherwise
 */
int modbus_convert_int8_signed(const uint16_t *registers,
                                double scaling_factor,
                                int8_t *result);

/**
 * @brief Convert Modbus registers to 8-bit unsigned integer
 * @param registers Array of 16-bit register values
 * @param scaling_factor Multiplier to apply
 * @param result Pointer to store result
 * @return MODBUS_CONV_OK on success, error code otherwise
 */
int modbus_convert_int8_unsigned(const uint16_t *registers,
                                  double scaling_factor,
                                  uint8_t *result);

/**
 * @brief Convert Modbus registers to 16-bit signed integer
 * @param registers Array of 16-bit register values
 * @param swap_bytes True for BA byte order, false for AB
 * @param scaling_factor Multiplier to apply
 * @param result Pointer to store result
 * @return MODBUS_CONV_OK on success, error code otherwise
 */
int modbus_convert_int16_signed(const uint16_t *registers,
                                 bool swap_bytes,
                                 double scaling_factor,
                                 int16_t *result);

/**
 * @brief Convert Modbus registers to 16-bit unsigned integer
 * @param registers Array of 16-bit register values
 * @param swap_bytes True for BA byte order, false for AB
 * @param scaling_factor Multiplier to apply
 * @param result Pointer to store result
 * @return MODBUS_CONV_OK on success, error code otherwise
 */
int modbus_convert_int16_unsigned(const uint16_t *registers,
                                   bool swap_bytes,
                                   double scaling_factor,
                                   uint16_t *result);

/**
 * @brief Convert Modbus registers to 32-bit integer
 * @param registers Array of 16-bit register values (minimum 2 registers)
 * @param data_type Specific 32-bit type with endianness
 * @param scaling_factor Multiplier to apply
 * @param result Pointer to store result
 * @return MODBUS_CONV_OK on success, error code otherwise
 */
int modbus_convert_int32(const uint16_t *registers,
                          modbus_data_type_t data_type,
                          double scaling_factor,
                          modbus_value_t *result);

/**
 * @brief Convert Modbus registers to 64-bit integer
 * @param registers Array of 16-bit register values (minimum 4 registers)
 * @param data_type Specific 64-bit type with endianness
 * @param scaling_factor Multiplier to apply
 * @param result Pointer to store result
 * @return MODBUS_CONV_OK on success, error code otherwise
 */
int modbus_convert_int64(const uint16_t *registers,
                          modbus_data_type_t data_type,
                          double scaling_factor,
                          modbus_value_t *result);

/**
 * @brief Convert Modbus registers to IEEE 32-bit float
 * @param registers Array of 16-bit register values (minimum 2 registers)
 * @param data_type Specific float32 type with endianness
 * @param scaling_factor Multiplier to apply
 * @param result Pointer to store result
 * @return MODBUS_CONV_OK on success, error code otherwise
 */
int modbus_convert_float32(const uint16_t *registers,
                            modbus_data_type_t data_type,
                            double scaling_factor,
                            float *result);

/**
 * @brief Convert Modbus registers to IEEE 64-bit float
 * @param registers Array of 16-bit register values (minimum 4 registers)
 * @param data_type Specific float64 type with endianness
 * @param scaling_factor Multiplier to apply
 * @param result Pointer to store result
 * @return MODBUS_CONV_OK on success, error code otherwise
 */
int modbus_convert_float64(const uint16_t *registers,
                            modbus_data_type_t data_type,
                            double scaling_factor,
                            double *result);

/**
 * @brief Get string description of error code
 * @param error_code Error code from conversion function
 * @return String description of error
 */
const char* modbus_conv_get_error_string(int error_code);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_CONVERSION_H */
