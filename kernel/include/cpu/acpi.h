/**
 * @file        acpi.h
 * @brief       ACPI header file
 * 
 * @author      Elliot Laborieux
 * @copyright   Copyright (c) 2026 Ellicode
*/

#pragma once
#include <stdint.h>

typedef struct rsdp_descriptor_v1_t {
    char        signature[8];
    uint8_t     checksum;
    char        oem_id[6];
    uint8_t     revision;
    uint32_t    rsdt_address;
} __attribute__ ((packed)) rsdp_descriptor_v1_t;

typedef struct rsdp_descriptor_v2_t {
    rsdp_descriptor_v1_t v1_fields;
    uint32_t    length;
    uint64_t    xsdt_address;
    uint8_t     extended_checksum;
    uint8_t     reserved[3];
} __attribute__ ((packed)) rsdp_descriptor_v2_t;

typedef struct sdt_header_t {
    char        signature[4];
    uint32_t    length;
    uint8_t     revision;
    uint8_t     checksum;
    char        oem_id[6];
    char        oem_table_id[8];
    uint32_t    oem_revision;
    uint32_t    creator_id;
    uint32_t    creator_revision;
} __attribute__ ((packed)) sdt_header_t;

typedef struct xsdt_header_t {
    sdt_header_t    header;
    uint64_t        sdtAddresses[];
} __attribute__ ((packed)) xsdt_header_t;

typedef struct rsdt_header_t {
    sdt_header_t    header;
    uint32_t        sdtAddresses[];
} __attribute__ ((packed)) rsdt_header_t;

#define CPUID_FLAG_ACPI (1ULL << 22U)

void acpi_init();