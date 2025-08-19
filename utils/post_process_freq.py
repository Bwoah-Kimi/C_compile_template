##################################################################################
# File Name:       post_process_freq.py     
# Author:          Zhantong Zhu, Xueping Liu
# Acknowledgement: GitHub Copilot
# Description:     Post-process the gdb dump of sensor frequency readings
##################################################################################

import re
import csv
def parse_gdb_log_to_csv(log_file, output_csv):
    # Read the log file
    with open(log_file, 'r') as f:
        gdb_output = f.read()
    
    # Extract ALL hex values (both per line)
    hex_pairs = re.findall(r'0x[0-9a-f]+:\s+(0x[0-9a-f]+)\s+(0x[0-9a-f]+)', gdb_output)
    hex_values = [value for pair in hex_pairs for value in pair]  # Flatten tuples
    
    # Convert hex strings to integers
    values = [int(h, 16) for h in hex_values]
    
    # Calculate number of iterations (14 sensors * 3 frequencies per iteration)
    num_iterations = len(values) // (14 * 3)
    print(f"Number of iterations: {num_iterations}")

    # Prepare data structure: iterations x sensors x frequencies
    data = []
    
    for itr in range(num_iterations):
        iteration_data = []
        for sensor in range(14):
            # Calculate base index for this sensor in this iteration
            base_idx = itr * 14 * 3 + sensor * 3
            freq0 = values[base_idx]
            freq1 = values[base_idx + 1]
            freq2 = values[base_idx + 2]
            iteration_data.extend([freq0, freq1, freq2])
        data.append(iteration_data)
    
    # Write to CSV
    with open(output_csv, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        
        # # Write header
        # headers = []
        # for sensor in range(14):
        #     headers.extend([
        #         f'freq0_sensor_{sensor}',
        #         f'freq1_sensor_{sensor}',
        #         f'freq2_sensor_{sensor}'
        #     ])
        # writer.writerow(headers)
        
        # Write data rows (one per iteration)
        writer.writerows(data)
    
    print(f"Successfully wrote data to {output_csv}")

# Example usage:
parse_gdb_log_to_csv('/root/isscc26_testing/C_compile_template/results/log.txt', '/root/isscc26_testing/C_compile_template/results/sensor_frequencies.csv')