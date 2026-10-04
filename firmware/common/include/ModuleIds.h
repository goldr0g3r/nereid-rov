/**
 * @file    ModuleIds.h
 * @brief   Module identifiers used for Det (development error) reports and logging.
 *
 * Values for MCAL/BSW modules follow the AUTOSAR module ID list where one exists;
 * project-specific modules use the 0x100+ range.
 */
#ifndef MODULE_IDS_H
#define MODULE_IDS_H

/* MCAL */
#define MODULE_ID_MCU  101u
#define MODULE_ID_GPT  100u
#define MODULE_ID_WDG  102u
#define MODULE_ID_DIO  120u
#define MODULE_ID_UART 0x110u /* AUTOSAR has no generic UART driver */

/* ECU abstraction */
#define MODULE_ID_IOHWAB 254u

/* Services */
#define MODULE_ID_OS   1u
#define MODULE_ID_ECUM 10u
#define MODULE_ID_WDGM 13u
#define MODULE_ID_DET  15u
#define MODULE_ID_DEM  54u
#define MODULE_ID_BSWM 42u
#define MODULE_ID_SCHM 130u
#define MODULE_ID_LOG  0x120u

/* RTE */
#define MODULE_ID_RTE 2u

#endif /* MODULE_IDS_H */
