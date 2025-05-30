##################################################################################
# File Name:       post_process_hex.py     
# Author:          Mingxuan Li, Zhantong Zhu
# Acknowledgement: Cursor + Claude, GitHub Copilot
# Description:     Convert a 64-bit hex file to a 128-bit hex file by concatenating
#                  two lines, then split into multiple SRAM hex files
##################################################################################

import argparse
import os
import sys
from typing import List, Tuple

def process_hex_file(file_path: str) -> List[str]:
    """
    Process a hex file by concatenating adjacent 64-bit lines to form 128-bit lines.
    
    Args:
        file_path (str): Path to the input hex file
        
    Returns:
        List[str]: List of 128-bit hex lines
        
    Raises:
        FileNotFoundError: If the input file doesn't exist
    """
    try:
        with open(file_path, 'r') as f:
            # Remove comments and empty lines
            lines = [line.strip() for line in f.readlines() 
                    if line.strip() and not line.strip().startswith('//')]
    except FileNotFoundError:
        print(f"Error: File not found: {file_path}")
        sys.exit(1)

    new_lines = []
    
    i = 0
    while i < len(lines):
        if len(lines[i]) == 32:
            # If the line is already 128 bits, just add it to the new lines
            new_lines.append(lines[i])
            i += 1
        elif len(lines[i]) == 16:
            if i + 1 < len(lines) and len(lines[i+1]) == 16:
                # Combine two 64-bit lines into one 128-bit line (little endian)
                new_line = lines[i+1] + lines[i]
                new_lines.append(new_line)
                i += 2
            else:
                # Pad the last 64-bit line if there's no pairing line
                padding_needed = 32 - len(lines[i])
                new_line = '0' * padding_needed + lines[i]
                new_lines.append(new_line)
                i += 1
        else:
            # Handle irregular line lengths (should only happen at the end)
            assert i == len(lines) - 1, f"Unexpected line length {len(lines[i])} at line {i+1}"
            padding_needed = 32 - len(lines[i])
            new_line = '0' * padding_needed + lines[i]
            new_lines.append(new_line)
            i += 1
    
    return new_lines

def split_hex_file_for_srams(lines: List[str], output_dir: str, sram_size_bytes: int = 1024) -> Tuple[List[str], List[int]]:
    """
    Split 128-bit hex lines into multiple hex files for different SRAMs.
    Each SRAM entry is a full 128-bit value.
    
    Args:
        lines (List[str]): List of 128-bit hex lines
        output_dir (str): Directory to write output files
        sram_size_bytes (int): Size of each SRAM in bytes
        
    Returns:
        Tuple[List[str], List[int]]: List of output filenames and counts of entries in each SRAM
    """
    # Calculate how many 128-bit entries fit in each SRAM
    bytes_per_entry = 16  # 128 bits = 16 bytes
    entries_per_sram = sram_size_bytes // bytes_per_entry

    # Divide entries between SRAMs
    sram_entries = []
    for i in range(0, len(lines), entries_per_sram):
        sram_entries.append(lines[i:i + entries_per_sram])
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Write output files
    output_files = []
    entry_counts = []
    
    for i, entries in enumerate(sram_entries):
        if not entries:  # Skip empty SRAMs
            continue
            
        output_file = os.path.join(output_dir, f"main_sram{i}.hex")
        with open(output_file, 'w') as f:
            for entry in entries:
                f.write(f"{entry}\n")
                
        output_files.append(f"main_sram{i}.hex")
        entry_counts.append(len(entries))
    
    return output_files, entry_counts

def main():
    """Main function to process and split hex files"""
    parser = argparse.ArgumentParser(
        description='Convert 64-bit hex file to 128-bit and split for multiple SRAMs'
    )
    parser.add_argument('--input', default='build/main.hex',
                        help='Input hex file path (default: build/main.hex)')
    parser.add_argument('--output-dir', default='build',
                        help='Output directory (default: build)')
    parser.add_argument('--sram-size', type=int, default=16384,
                        help='SRAM size in bytes (default: 16384)')
    parser.add_argument('--verbose', '-v', action='store_true',
                        help='Enable verbose output')
    
    args = parser.parse_args()
    
    # Step 1: Process hex file to create 128-bit lines
    if args.verbose:
        print(f"Processing input file: {args.input}")
    
    processed_lines = process_hex_file(args.input)
    
    # Step 2: Write processed lines to the output file
    intermediate_file = os.path.join(args.output_dir, 'main_128b.hex')
    os.makedirs(args.output_dir, exist_ok=True)
    
    with open(intermediate_file, 'w') as f:
        for line in processed_lines:
            f.write(f"{line}\n")
    
    if args.verbose:
        print(f"Wrote {len(processed_lines)} 128-bit instructions to {intermediate_file}")
    
    # Step 3: Split the file for multiple SRAMs
    output_files, entry_counts = split_hex_file_for_srams(
        processed_lines, 
        args.output_dir,
        args.sram_size
    )
    
    # Print summary
    print("\nSplit complete!")
    for i, (filename, count) in enumerate(zip(output_files, entry_counts)):
        print(f"SRAM {i}: {count} instructions in {filename}")


if __name__ == "__main__":
    main()