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
uint32_t power_sensors[5][14] = {
    { 10472426,  6355498,  3220088,   918076,   430496,  1687945,  3454872,  1944361,  2048773,   516321,    72757,  4126641,  3995519,   339726 },
    { 10473530,  6355498,  3286929,   940283,   436809,  1687591,  3475357,  1987539,  2087227,   526255,    73994,  4126641,  4033001,   342640 },
    { 10472205,  6355498,  3341541,   950481,   443122,  1687900,  3530852,  2017207,  2125284,   538616,    75671,  4126641,  4063067,   347187 },
    { 10472161,  6355498,  3243442,   923286,   432218,  1687724,  3393549,  1962639,  2072570,   521663,    72669,  4126641,  4008190,   340256 },
    { 10473309,  6355498,  3268519,   927347,   433056,  1687591,  3479948,  1954692,  2073409,   519191,    73155,  4126641,  4011192,   342595 }
};

// Original Thermal Sensor Inputs (uint16_t format)
uint16_t thermal_sensors[5][14] = {
    {    66,   122,    81,    82,   122,    85,   123,   121,    95,    73,    72,   130,   124,    74 },
    {    66,   122,    83,    85,   125,    88,   125,   123,    99,    75,    75,   130,   127,    76 },
    {    65,   123,    85,    86,   126,    89,   126,   125,   100,    75,    76,   131,   128,    76 },
    {    65,   123,    85,    86,   125,    89,   125,   124,   100,    76,    76,   131,   127,    76 },
    {    65,   123,    85,    87,   126,    90,   126,   124,   100,    76,    76,   131,   128,    77 }
};

// Expected Thermal Outputs (uint16_t sensor codes)
uint16_t expected_outputs[5][14] = {
    {    66,   122,    83,    85,   125,    88,   125,   123,    99,    75,    75,   130,   127,    76 },
    {    65,   123,    85,    86,   126,    89,   126,   125,   100,    75,    76,   131,   128,    76 },
    {    65,   123,    85,    86,   125,    89,   125,   124,   100,    76,    76,   131,   127,    76 },
    {    65,   123,    85,    87,   126,    90,   126,   124,   100,    76,    76,   131,   128,    77 },
    {    65,   123,    85,    87,   126,    90,   126,   124,   100,    76,    77,   131,   128,    77 }
};

// Expected Dequantized Results (uint16_t sensor codes)
uint16_t expected_dequantized[5][14] = {
    {    89,   169,    79,    89,   123,    63,   111,   117,   123,    67,    81,   195,   103,    67 },
    {    91,   167,    81,    91,   127,    63,   113,   119,   127,    69,    83,   193,   105,    71 },
    {    91,   167,    83,    91,   127,    67,   113,   123,   129,    67,    83,   191,   105,    71 },
    {    91,   165,    85,    91,   127,    65,   113,   123,   127,    67,    83,   193,   103,    73 },
    {    91,   165,    85,    91,   127,    65,   113,   123,   129,    67,    85,   193,   103,    73 }
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