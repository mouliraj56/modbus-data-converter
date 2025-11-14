/**
 * @file modbus_conversion.c
 * @brief Modbus Data Conversion Library Implementation
 * @author Mouli Sai
 */

#include "modbus_conversion.h"
#include <string.h>
#include <stddef.h>

/* Helper macros */
#define SWAP_BYTES_16(val) ((((val) & 0xFF) << 8) | (((val) >> 8) & 0xFF))

/* Helper function prototypes */
static void regs_to_bytes(const uint16_t *regs, size_t count, uint8_t *bytes);
static void bytes_reverse(uint8_t *bytes, size_t len);
static uint16_t swap_bytes_16(uint16_t value);

/* Main conversion function */
int modbus_convert(const uint16_t *registers,
                   size_t reg_count,
                   modbus_data_type_t data_type,
                   uint8_t bit_pos,
                   double scaling_factor,
                   modbus_value_t *result)
{
    if (!registers || !result) {
        return MODBUS_CONV_ERR_NULL_PTR;
    }
    
    if (reg_count == 0) {
        return MODBUS_CONV_ERR_INSUFF_REGS;
    }
    
    /* Handle bit boolean type */
    if (data_type == MODBUS_BIT_BOOLEAN) {
        return modbus_convert_bit_bool(registers, bit_pos, &result->bool_val);
    }
    
    /* Handle 8-bit types */
    if (data_type == MODBUS_INT8_SIGNED) {
        return modbus_convert_int8_signed(registers, scaling_factor, &result->i8);
    }
    if (data_type == MODBUS_INT8_UNSIGNED) {
        return modbus_convert_int8_unsigned(registers, scaling_factor, &result->u8);
    }
    
    /* Handle 16-bit types */
    if (data_type == MODBUS_INT16_SIGNED_AB) {
        return modbus_convert_int16_signed(registers, false, scaling_factor, &result->i16);
    }
    if (data_type == MODBUS_INT16_SIGNED_BA) {
        return modbus_convert_int16_signed(registers, true, scaling_factor, &result->i16);
    }
    if (data_type == MODBUS_INT16_UNSIGNED_AB) {
        return modbus_convert_int16_unsigned(registers, false, scaling_factor, &result->u16);
    }
    if (data_type == MODBUS_INT16_UNSIGNED_BA) {
        return modbus_convert_int16_unsigned(registers, true, scaling_factor, &result->u16);
    }
    
    /* Handle 32-bit integer types */
    if (data_type >= MODBUS_INT32_SIGNED_ABCD && data_type <= MODBUS_INT32_UNSIGNED_CDAB) {
        return modbus_convert_int32(registers, data_type, scaling_factor, result);
    }
    
    /* Handle 64-bit integer types */
    if (data_type >= MODBUS_INT64_SIGNED_ABCDEFGH && data_type <= MODBUS_INT64_UNSIGNED_EFGHABCD) {
        return modbus_convert_int64(registers, data_type, scaling_factor, result);
    }
    
    /* Handle IEEE float32 types */
    if (data_type >= MODBUS_IEEE_FLOAT32_ABCD && data_type <= MODBUS_IEEE_FLOAT32_BADC) {
        return modbus_convert_float32(registers, data_type, scaling_factor, &result->f32);
    }
    
    /* Handle IEEE float64 types */
    if (data_type >= MODBUS_IEEE_FLOAT64_ABCDEFGH && data_type <= MODBUS_IEEE_FLOAT64_EFGHABCD) {
        return modbus_convert_float64(registers, data_type, scaling_factor, &result->f64);
    }
    
    return MODBUS_CONV_ERR_INVALID_TYPE;
}

/* Bit boolean conversion */
int modbus_convert_bit_bool(const uint16_t *registers,
                             uint8_t bit_pos,
                             bool *result)
{
    if (!registers || !result) {
        return MODBUS_CONV_ERR_NULL_PTR;
    }
    
    if (bit_pos > 15) {
        return MODBUS_CONV_ERR_INVALID_BIT;
    }
    
    *result = (registers[0] >> bit_pos) & 1;
    return MODBUS_CONV_OK;
}

/* 8-bit signed conversion */
int modbus_convert_int8_signed(const uint16_t *registers,
                                double scaling_factor,
                                int8_t *result)
{
    if (!registers || !result) {
        return MODBUS_CONV_ERR_NULL_PTR;
    }
    
    uint8_t val = registers[0] & 0xFF;
    int16_t signed_val = (val > 127) ? (val - 256) : val;
    *result = (int8_t)(signed_val * scaling_factor);
    return MODBUS_CONV_OK;
}

/* 8-bit unsigned conversion */
int modbus_convert_int8_unsigned(const uint16_t *registers,
                                  double scaling_factor,
                                  uint8_t *result)
{
    if (!registers || !result) {
        return MODBUS_CONV_ERR_NULL_PTR;
    }
    
    *result = (uint8_t)((registers[0] & 0xFF) * scaling_factor);
    return MODBUS_CONV_OK;
}

/* 16-bit signed conversion */
int modbus_convert_int16_signed(const uint16_t *registers,
                                 bool swap_bytes,
                                 double scaling_factor,
                                 int16_t *result)
{
    if (!registers || !result) {
        return MODBUS_CONV_ERR_NULL_PTR;
    }
    
    uint16_t val = swap_bytes ? swap_bytes_16(registers[0]) : registers[0];
    int32_t signed_val = (val > 32767) ? (val - 65536) : val;
    *result = (int16_t)(signed_val * scaling_factor);
    return MODBUS_CONV_OK;
}

/* 16-bit unsigned conversion */
int modbus_convert_int16_unsigned(const uint16_t *registers,
                                   bool swap_bytes,
                                   double scaling_factor,
                                   uint16_t *result)
{
    if (!registers || !result) {
        return MODBUS_CONV_ERR_NULL_PTR;
    }
    
    uint16_t val = swap_bytes ? swap_bytes_16(registers[0]) : registers[0];
    *result = (uint16_t)(val * scaling_factor);
    return MODBUS_CONV_OK;
}

/* 32-bit integer conversion */
int modbus_convert_int32(const uint16_t *registers,
                          modbus_data_type_t data_type,
                          double scaling_factor,
                          modbus_value_t *result)
{
    if (!registers || !result) {
        return MODBUS_CONV_ERR_NULL_PTR;
    }
    
    uint8_t bytes[4];
    uint16_t regs[2];
    
    /* Reorder based on endianness */
    switch (data_type) {
        case MODBUS_INT32_SIGNED_ABCD:
        case MODBUS_INT32_UNSIGNED_ABCD:
            regs[0] = registers[0];
            regs[1] = registers[1];
            regs_to_bytes(regs, 2, bytes);
            break;
            
        case MODBUS_INT32_SIGNED_DCBA:
        case MODBUS_INT32_UNSIGNED_DCBA:
            regs[0] = registers[1];
            regs[1] = registers[0];
            regs_to_bytes(regs, 2, bytes);
            bytes_reverse(bytes, 4);
            break;
            
        case MODBUS_INT32_SIGNED_BADC:
        case MODBUS_INT32_UNSIGNED_BADC:
            regs[0] = swap_bytes_16(registers[0]);
            regs[1] = swap_bytes_16(registers[1]);
            regs_to_bytes(regs, 2, bytes);
            break;
            
        case MODBUS_INT32_SIGNED_CDAB:
        case MODBUS_INT32_UNSIGNED_CDAB:
            regs[0] = registers[1];
            regs[1] = registers[0];
            regs_to_bytes(regs, 2, bytes);
            break;
            
        default:
            return MODBUS_CONV_ERR_INVALID_TYPE;
    }
    
    /* Convert bytes to 32-bit value */
    uint32_t u32_val = ((uint32_t)bytes[0] << 24) |
                       ((uint32_t)bytes[1] << 16) |
                       ((uint32_t)bytes[2] << 8) |
                       ((uint32_t)bytes[3]);
    
    /* Apply scaling based on signed/unsigned */
    if (data_type >= MODBUS_INT32_UNSIGNED_ABCD && data_type <= MODBUS_INT32_UNSIGNED_CDAB) {
        result->u32 = (uint32_t)(u32_val * scaling_factor);
    } else {
        int32_t i32_val = (int32_t)u32_val;
        result->i32 = (int32_t)(i32_val * scaling_factor);
    }
    
    return MODBUS_CONV_OK;
}

/* 64-bit integer conversion */
int modbus_convert_int64(const uint16_t *registers,
                          modbus_data_type_t data_type,
                          double scaling_factor,
                          modbus_value_t *result)
{
    if (!registers || !result) {
        return MODBUS_CONV_ERR_NULL_PTR;
    }
    
    uint8_t bytes[8];
    uint16_t regs[4];
    int i;
    
    /* Reorder based on endianness pattern */
    switch (data_type) {
        case MODBUS_INT64_SIGNED_ABCDEFGH:
        case MODBUS_INT64_UNSIGNED_ABCDEFGH:
            regs_to_bytes(registers, 4, bytes);
            break;
            
        case MODBUS_INT64_SIGNED_HGFEDCBA:
        case MODBUS_INT64_UNSIGNED_HGFEDCBA:
            regs[0] = registers[3];
            regs[1] = registers[2];
            regs[2] = registers[1];
            regs[3] = registers[0];
            regs_to_bytes(regs, 4, bytes);
            bytes_reverse(bytes, 8);
            break;
            
        case MODBUS_INT64_SIGNED_BADCFEHG:
        case MODBUS_INT64_UNSIGNED_BADCFEHG:
            for (i = 0; i < 4; i++) {
                regs[i] = swap_bytes_16(registers[i]);
            }
            regs_to_bytes(regs, 4, bytes);
            break;
            
        case MODBUS_INT64_SIGNED_CDABGHEF:
        case MODBUS_INT64_UNSIGNED_CDABGHEF:
            regs[0] = registers[1];
            regs[1] = registers[0];
            regs[2] = registers[3];
            regs[3] = registers[2];
            regs_to_bytes(regs, 4, bytes);
            break;
            
        case MODBUS_INT64_SIGNED_DCBAHGFE:
        case MODBUS_INT64_UNSIGNED_DCBAHGFE:
            for (i = 0; i < 4; i++) {
                regs[i] = swap_bytes_16(registers[3-i]);
            }
            regs_to_bytes(regs, 4, bytes);
            bytes_reverse(bytes, 8);
            break;
            
        case MODBUS_INT64_SIGNED_GHEFCDAB:
        case MODBUS_INT64_UNSIGNED_GHEFCDAB:
            regs[0] = registers[3];
            regs[1] = registers[2];
            regs[2] = registers[1];
            regs[3] = registers[0];
            regs_to_bytes(regs, 4, bytes);
            bytes_reverse(bytes, 8);
            break;
            
        case MODBUS_INT64_SIGNED_FEHGBADC:
        case MODBUS_INT64_UNSIGNED_FEHGBADC:
            regs[0] = registers[2];
            regs[1] = registers[3];
            regs[2] = registers[0];
            regs[3] = registers[1];
            regs_to_bytes(regs, 4, bytes);
            bytes_reverse(bytes, 8);
            break;
            
        case MODBUS_INT64_SIGNED_EFGHABCD:
        case MODBUS_INT64_UNSIGNED_EFGHABCD:
            regs[0] = swap_bytes_16(registers[2]);
            regs[1] = swap_bytes_16(registers[3]);
            regs[2] = swap_bytes_16(registers[0]);
            regs[3] = swap_bytes_16(registers[1]);
            regs_to_bytes(regs, 4, bytes);
            bytes_reverse(bytes, 8);
            break;
            
        default:
            return MODBUS_CONV_ERR_INVALID_TYPE;
    }
    
    /* Convert bytes to 64-bit value */
    uint64_t u64_val = ((uint64_t)bytes[0] << 56) |
                       ((uint64_t)bytes[1] << 48) |
                       ((uint64_t)bytes[2] << 40) |
                       ((uint64_t)bytes[3] << 32) |
                       ((uint64_t)bytes[4] << 24) |
                       ((uint64_t)bytes[5] << 16) |
                       ((uint64_t)bytes[6] << 8) |
                       ((uint64_t)bytes[7]);
    
    /* Apply scaling based on signed/unsigned */
    if (data_type >= MODBUS_INT64_UNSIGNED_ABCDEFGH && data_type <= MODBUS_INT64_UNSIGNED_EFGHABCD) {
        result->u64 = (uint64_t)(u64_val * scaling_factor);
    } else {
        int64_t i64_val = (int64_t)u64_val;
        result->i64 = (int64_t)(i64_val * scaling_factor);
    }
    
    return MODBUS_CONV_OK;
}

/* IEEE 32-bit float conversion */
int modbus_convert_float32(const uint16_t *registers,
                            modbus_data_type_t data_type,
                            double scaling_factor,
                            float *result)
{
    if (!registers || !result) {
        return MODBUS_CONV_ERR_NULL_PTR;
    }
    
    uint8_t bytes[4];
    uint16_t regs[2];
    
    /* Reorder based on endianness */
    switch (data_type) {
        case MODBUS_IEEE_FLOAT32_ABCD:
            regs[0] = registers[0];
            regs[1] = registers[1];
            regs_to_bytes(regs, 2, bytes);
            break;
            
        case MODBUS_IEEE_FLOAT32_CDAB:
            regs[0] = registers[1];
            regs[1] = registers[0];
            regs_to_bytes(regs, 2, bytes);
            break;
            
        case MODBUS_IEEE_FLOAT32_DCBA:
            regs[0] = registers[1];
            regs[1] = registers[0];
            regs_to_bytes(regs, 2, bytes);
            bytes_reverse(bytes, 4);
            break;
            
        case MODBUS_IEEE_FLOAT32_BADC:
            regs[0] = swap_bytes_16(registers[0]);
            regs[1] = swap_bytes_16(registers[1]);
            regs_to_bytes(regs, 2, bytes);
            break;
            
        default:
            return MODBUS_CONV_ERR_INVALID_TYPE;
    }
    
    /* Convert bytes to float */
    uint32_t u32_val = ((uint32_t)bytes[0] << 24) |
                       ((uint32_t)bytes[1] << 16) |
                       ((uint32_t)bytes[2] << 8) |
                       ((uint32_t)bytes[3]);
    
    float f32_val;
    memcpy(&f32_val, &u32_val, sizeof(float));
    *result = f32_val * scaling_factor;
    
    return MODBUS_CONV_OK;
}

/* IEEE 64-bit float conversion */
int modbus_convert_float64(const uint16_t *registers,
                            modbus_data_type_t data_type,
                            double scaling_factor,
                            double *result)
{
    if (!registers || !result) {
        return MODBUS_CONV_ERR_NULL_PTR;
    }
    
    uint8_t bytes[8];
    uint16_t regs[4];
    int i;
    
    /* Reorder based on endianness pattern (same as int64) */
    switch (data_type) {
        case MODBUS_IEEE_FLOAT64_ABCDEFGH:
            regs_to_bytes(registers, 4, bytes);
            break;
            
        case MODBUS_IEEE_FLOAT64_HGFEDCBA:
            regs[0] = registers[3];
            regs[1] = registers[2];
            regs[2] = registers[1];
            regs[3] = registers[0];
            regs_to_bytes(regs, 4, bytes);
            bytes_reverse(bytes, 8);
            break;
            
        case MODBUS_IEEE_FLOAT64_BADCFEHG:
            for (i = 0; i < 4; i++) {
                regs[i] = swap_bytes_16(registers[i]);
            }
            regs_to_bytes(regs, 4, bytes);
            break;
            
        case MODBUS_IEEE_FLOAT64_CDABGHEF:
            regs[0] = registers[1];
            regs[1] = registers[0];
            regs[2] = registers[3];
            regs[3] = registers[2];
            regs_to_bytes(regs, 4, bytes);
            break;
            
        case MODBUS_IEEE_FLOAT64_DCBAHGFE:
            for (i = 0; i < 4; i++) {
                regs[i] = swap_bytes_16(registers[3-i]);
            }
            regs_to_bytes(regs, 4, bytes);
            bytes_reverse(bytes, 8);
            break;
            
        case MODBUS_IEEE_FLOAT64_GHEFCDAB:
            regs[0] = registers[3];
            regs[1] = registers[2];
            regs[2] = registers[1];
            regs[3] = registers[0];
            regs_to_bytes(regs, 4, bytes);
            bytes_reverse(bytes, 8);
            break;
            
        case MODBUS_IEEE_FLOAT64_FEHGBADC:
            regs[0] = registers[2];
            regs[1] = registers[3];
            regs[2] = registers[0];
            regs[3] = registers[1];
            regs_to_bytes(regs, 4, bytes);
            bytes_reverse(bytes, 8);
            break;
            
        case MODBUS_IEEE_FLOAT64_EFGHABCD:
            regs[0] = swap_bytes_16(registers[2]);
            regs[1] = swap_bytes_16(registers[3]);
            regs[2] = swap_bytes_16(registers[0]);
            regs[3] = swap_bytes_16(registers[1]);
            regs_to_bytes(regs, 4, bytes);
            bytes_reverse(bytes, 8);
            break;
            
        default:
            return MODBUS_CONV_ERR_INVALID_TYPE;
    }
    
    /* Convert bytes to double */
    uint64_t u64_val = ((uint64_t)bytes[0] << 56) |
                       ((uint64_t)bytes[1] << 48) |
                       ((uint64_t)bytes[2] << 40) |
                       ((uint64_t)bytes[3] << 32) |
                       ((uint64_t)bytes[4] << 24) |
                       ((uint64_t)bytes[5] << 16) |
                       ((uint64_t)bytes[6] << 8) |
                       ((uint64_t)bytes[7]);
    
    double f64_val;
    memcpy(&f64_val, &u64_val, sizeof(double));
    *result = f64_val * scaling_factor;
    
    return MODBUS_CONV_OK;
}

/* Get error string */
const char* modbus_conv_get_error_string(int error_code)
{
    switch (error_code) {
        case MODBUS_CONV_OK:
            return "Success";
        case MODBUS_CONV_ERR_NULL_PTR:
            return "Null pointer error";
        case MODBUS_CONV_ERR_INVALID_TYPE:
            return "Invalid data type";
        case MODBUS_CONV_ERR_INVALID_BIT:
            return "Invalid bit position (must be 0-15)";
        case MODBUS_CONV_ERR_INSUFF_REGS:
            return "Insufficient registers for conversion";
        case MODBUS_CONV_ERR_UNKNOWN:
            return "Unknown error";
        default:
            return "Unrecognized error code";
    }
}

/* Helper function implementations */
static void regs_to_bytes(const uint16_t *regs, size_t count, uint8_t *bytes)
{
    size_t i;
    for (i = 0; i < count; i++) {
        bytes[i * 2] = (regs[i] >> 8) & 0xFF;
        bytes[i * 2 + 1] = regs[i] & 0xFF;
    }
}

static void bytes_reverse(uint8_t *bytes, size_t len)
{
    size_t i;
    for (i = 0; i < len / 2; i++) {
        uint8_t temp = bytes[i];
        bytes[i] = bytes[len - 1 - i];
        bytes[len - 1 - i] = temp;
    }
}

static uint16_t swap_bytes_16(uint16_t value)
{
    return ((value & 0xFF) << 8) | ((value >> 8) & 0xFF);
}
