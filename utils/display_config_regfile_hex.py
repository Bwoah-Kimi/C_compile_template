#!/usr/bin/env python3
##################################################################################
# Author:          Zhantong Zhu
# Acknowledgement: GitHub Copilot + DeepSeek
# Date:            2025-04-11
# Description:     Python script to display hex values of a configuration register file.
#################################################################################

def display_therm_top_config():
    """Display the configuration register file for the THERM_TOP module."""
    # Configuration parameters
    # THERM_TOP CONFIG REGFILE 0
    therm_top_start = 1
    therm_top_en = 1
    therm_top_stop = 0
    collect_en = 1
    collect_mode = 0
    pred_en = 0
    schedule_en = 0
    store_sensor_en = 1
    store_pred_en = 0
    store_action_en = 0
    action_offset = 4
    num_itr = 10
    sampling_intvl = 200

    # THERM_TOP CONFIG REGFILE 1
    sensor_data_base_addr = 0xF000
    pred_data_base_addr = 0x2000
    action_base_addr = 0x2800

    # THERM_TOP CONFIG REGFILE 2
    npu_input_buf_base_addr = 0x10
    npu_output_buf_base_addr = 0x5
    synthetic_sensor_thermal_encodings = 10
    synthetic_sensor_current_encodings = 20
    synthetic_sensor_voltage_encodings = 30

    # THERM_TOP CONFIG REGFILE 3
    synthetic_action_sequence =  342391

    # Calculate register values
    # THERM_TOP CONFIG REGFILE 0
    reg0 = (therm_top_start & 0x1)
    reg0 |= (therm_top_en & 0x1) << 1
    reg0 |= (therm_top_stop & 0x1) << 2
    reg0 |= (collect_en & 0x1) << 3
    reg0 |= (collect_mode & 0x1) << 4
    reg0 |= (pred_en & 0x1) << 5
    reg0 |= (schedule_en & 0x1) << 6
    reg0 |= (store_sensor_en & 0x1) << 7
    reg0 |= (store_pred_en & 0x1) << 8
    reg0 |= (store_action_en & 0x1) << 9
    reg0 |= (action_offset & 0x1F) << 10
    reg0 |= (num_itr & 0x1FFFF) << 15
    reg0 |= (sampling_intvl & 0xFFFFFFFF) << 32

    # THERM_TOP CONFIG REGFILE 1
    reg1 = (sensor_data_base_addr & 0xFFFFFFFF)         # 32 bits [31:0]
    reg1 |= (pred_data_base_addr & 0xFFFFFFFF) << 32    # 32 bits [63:32]

    # THERM_TOP CONFIG REGFILE 2
    reg2 = 0
    reg2 = (npu_input_buf_base_addr & 0x3FF)                    # 10 bits [9:0]
    reg2 |= (npu_output_buf_base_addr & 0x3FF) << 10            # 10 bits [19:10]
    # Bits [31:20] are reserved and set to 0
    reg2 |= (synthetic_sensor_thermal_encodings & 0xFF) << 32   # 8 bits [39:32]
    reg2 |= (synthetic_sensor_current_encodings & 0xFF) << 40   # 8 bits [47:40]
    reg2 |= (synthetic_sensor_voltage_encodings & 0xFF) << 48   # 8 bits [55:48]
    # Bits [63:56] are reserved and set to 0

    # THERM_TOP CONFIG REGFILE 3
    reg3 = 0
    reg3 |= (synthetic_action_sequence & 0xFFFFFF)      # 24 bits [23:0]
    # Bits [31:24] are reserved and set to 0
    reg3 |= (action_base_addr & 0xFFFFFFFF) << 32       # 32 bits [63:32]
    # Bits [63:24] are reserved and set to 0

    therm_top_base_addr = 0x60002218

    # Print the register values for debugging
    print(f"THERM_TOP CONFIG REGFILE 0: Addr: {therm_top_base_addr + 0 * 8:#010x}, Data: {reg0:#018x}")
    print(f"THERM_TOP CONFIG REGFILE 1: Addr: {therm_top_base_addr + 1 * 8:#010x}, Data: {reg1:#018x}")
    print(f"THERM_TOP CONFIG REGFILE 2: Addr: {therm_top_base_addr + 2 * 8:#010x}, Data: {reg2:#018x}")
    print(f"THERM_TOP CONFIG REGFILE 3: Addr: {therm_top_base_addr + 3 * 8:#010x}, Data: {reg3:#018x}")

if __name__ == "__main__":
    display_therm_top_config()