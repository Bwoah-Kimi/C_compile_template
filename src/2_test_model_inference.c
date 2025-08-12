//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     C code for prediction model inference
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "quantized_model_inference.h"
#include "init_config.h"
#include "const_values.h"
#include "uart.h"

// Original Power Sensor Inputs (uint32_t format)
uint32_t power_sensors[1][14] = {
    { 10472426,  6355498,  3220088,   918076,   430496,  1687945,  3454872,  1944361,  2048773,   516321,    72757,  4126641,  3995519,   339726 }
};

// Original Thermal Sensor Inputs (uint16_t format)
uint16_t thermal_sensors[1][14] = {
    {    66,   122,    81,    82,   122,    85,   123,   121,    95,    73,    72,   130,   124,    74 }
};

// Expected Thermal Outputs (uint16_t sensor codes)
uint16_t expected_outputs[1][14] = {
    {    66,   122,    83,    85,   125,    88,   125,   123,    99,    75,    75,   130,   127,    76 }
};

// Expected Dequantized Results (uint16_t sensor codes)
uint16_t expected_dequantized[1][14] = {
    {    63,   125,    83,    79,   121,    83,   125,   121,    93,    75,    69,   133,   125,    73 }
};

// Read the cycle counter (mcycle CSR register)
static inline uint64_t read_cycles(void) {
    uint64_t cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}

// Read the instruction counter (minstret CSR register)
static inline uint64_t read_instructions(void) {
    uint64_t instret;
    asm volatile ("rdinstret %0" : "=r" (instret));
    return instret;
}

int run_mlp_inference(uint32_t* power_curr, uint16_t* thermal_curr, uint16_t* thermal_pred) {
    static QuantizedMLP mlp_model;
    static bool model_initialized = false;
    static int8_t quantized_input[NUM_SENSORS * 2];
    static int32_t raw_output[NUM_SENSORS];

    if (!model_initialized) {
        initialize_quantized_model(&mlp_model);
        model_initialized = true;
    }

    // Quantize input data
    quantize_input_data(power_curr, thermal_curr, quantized_input, NUM_SENSORS);

    // Perform inference
    int result = quantized_mlp_inference(&mlp_model, quantized_input, raw_output);
    if (result != 0) {
        print_uart("Inference failed\n");
        return -1; // Inference error
    }

    // Dequantize the output
    dequantize_output_data(raw_output, thermal_pred, NUM_SENSORS);

    return 0; // Success
}

// Print comparison results
void print_comparison(const uint16_t* actual, const uint16_t* expected, uint16_t size, const char* test_name) {
    print_uart("  ");
    print_uart(test_name);
    print_uart(":\n");

    print_uart("    Actual:   [");
    for (uint16_t i = 0; i < size; i++) {
        print_uart_int(actual[i]);
        if (i < size - 1) print_uart(", ");
    }
    print_uart("]\n");

    print_uart("    Expected: [");
    for (uint16_t i = 0; i < size; i++) {
        print_uart_int(expected[i]);
        if (i < size - 1) print_uart(", ");
    }
    print_uart("]\n");

    return;
}

int main() {
    init_uart(10000000, 101000);

    print_uart("\n================================================\n");
    print_uart("Starting Prediction Model Inference Test\n");
    print_uart("================================================\n");

    uint16_t inference_results[NUM_SENSORS];
    uint32_t total_inference_errors = 0;

    uint64_t total_cycles = 0;
    uint64_t total_instructions = 0;
    uint64_t min_cycles = UINT64_MAX;
    uint64_t max_cycles = 0;

    for (int sample = 0; sample < 5; sample++) {
        print_uart("Testing sample: ");
        print_uart_int(sample + 1);
        print_uart("\n");

        // Start performance measurement
        uint64_t start_cycles = read_cycles();
        uint64_t start_instructions = read_instructions();

        // Run inference
        int result = run_mlp_inference(power_sensors[sample], thermal_sensors[sample], inference_results);

        // End performance measurement
        uint64_t end_cycles = read_cycles();
        uint64_t end_instructions = read_instructions();

        uint64_t sample_cycles = end_cycles - start_cycles;
        uint64_t sample_instructions = end_instructions - start_instructions;

        total_cycles += sample_cycles;
        total_instructions += sample_instructions;
        if (sample_cycles < min_cycles) min_cycles = sample_cycles;
        if (sample_cycles > max_cycles) max_cycles = sample_cycles;

        // Print performance for this sample
        print_uart("  Cycles: ");
        print_uart_dec_formatted(sample_cycles);
        print_uart(" (0x");
        print_uart_addr(sample_cycles);
        print_uart(")\n");

        print_uart("  Instructions: ");
        print_uart_dec_formatted(sample_instructions);
        print_uart(" (0x");
        print_uart_addr(sample_instructions);
        print_uart(")\n");

        print_uart("  CPI: ");
        if (sample_instructions > 0) {
            uint32_t cpi_scaled = (uint32_t)((sample_cycles * 1000) / sample_instructions); // CPI * 1000
            print_uart_dec(cpi_scaled / 1000);
            print_uart(".");
            // Print 3 decimal places with leading zeros
            uint32_t fractional = cpi_scaled % 1000;
            if (fractional < 100) print_uart_char('0');
            if (fractional < 10) print_uart_char('0');
            print_uart_dec(fractional);
        }
        else {
            print_uart("N/A");
        }
        print_uart("\n");

        if (result != 0) {
            total_inference_errors++;
            print_uart("Inference failed for sample ");
            print_uart_int(sample + 1);
            print_uart("\n");
            continue; // Skip to next sample
        }

        // Compare results with expected outputs
        print_comparison(inference_results, expected_dequantized[sample], NUM_SENSORS, "C vs PyTorch");
    }

    // Performance Summary
    print_uart("\n================================================\n");
    print_uart("Performance Summary:\n");
    print_uart("================================================\n");

    uint32_t successful_samples = 5 - total_inference_errors;
    if (successful_samples > 0) {
        uint64_t avg_cycles = total_cycles / successful_samples;
        uint64_t avg_instructions = total_instructions / successful_samples;

        print_uart("Successful samples: ");
        print_uart_dec(successful_samples);
        print_uart("\n");

        print_uart("Average cycles per inference: ");
        print_uart_dec_formatted(avg_cycles);
        print_uart("\n");

        print_uart("Average instructions per inference: ");
        print_uart_dec_formatted(avg_instructions);
        print_uart("\n");

        print_uart("Min cycles: ");
        print_uart_dec_formatted(min_cycles);
        print_uart("\n");

        print_uart("Max cycles: ");
        print_uart_dec_formatted(max_cycles);
        print_uart("\n");

        if (avg_instructions > 0) {
            uint32_t avg_cpi_scaled = (uint32_t)((avg_cycles * 1000) / avg_instructions);
            print_uart("Average CPI: ");
            print_uart_dec(avg_cpi_scaled / 1000);
            print_uart(".");
            uint32_t fractional = avg_cpi_scaled % 1000;
            if (fractional < 100) print_uart_char('0');
            if (fractional < 10) print_uart_char('0');
            print_uart_dec(fractional);
            print_uart("\n");
        }
    }

    // Test Summary
    if (total_inference_errors == 0) {
        print_uart("All inference tests PASSED!\n");
    }
    else {
        print_uart("Inference tests FAILED with ");
        print_uart_int(total_inference_errors);
        print_uart(" errors.\n");
    }
    print_uart("================================================\n");

    return 0;
}