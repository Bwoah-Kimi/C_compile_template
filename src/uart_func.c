#include "uart.h"
#include <stdint.h>

// 测试数据
static const char* test_strings[] = {
    "Hello World from CVA6!\n",
    "UART Test String 1\n",
    "UART Test String 2\n",
    "Special chars: !@#$%^&*()\n",
    "Numbers: 0123456789\n",
    "End of test\n"
};

// 测试整数输出
void test_uart_integers() {
    print_uart("=== Integer Test ===\n");

    uint32_t test_ints[] = { 0x12345678, 0xABCDEF00, 0xDEADBEEF, 0x00000001, 0xFFFFFFFF };

    for (int i = 0; i < 5; i++) {
        print_uart("Int ");
        print_uart_char(i + '0');
        print_uart(": 0x");
        print_uart_int(test_ints[i]);
        print_uart("\n");
    }
}

// 测试地址输出
void test_uart_addresses() {
    print_uart("=== Address Test ===\n");

    uint64_t test_addrs[] = {
        0x80000000,
        0x10000000,
        0xa0000000,
        0x1234567890abcdef,
        0xfedcba0987654321
    };

    for (int i = 0; i < 5; i++) {
        print_uart("Addr ");
        print_uart_char(i + '0');
        print_uart(": 0x");
        print_uart_addr(test_addrs[i]);
        print_uart("\n");
    }
}

// 测试字节输出
void test_uart_bytes() {
    print_uart("=== Byte Test ===\n");

    uint8_t test_bytes[] = { 0x00, 0xFF, 0xAA, 0x55, 0x12, 0x34, 0x56, 0x78 };

    print_uart("Bytes: ");
    for (int i = 0; i < 8; i++) {
        print_uart("0x");
        print_uart_byte(test_bytes[i]);
        print_uart(" ");
    }
    print_uart("\n");
}

// 测试内存写入和UART输出结合
void test_memory_uart() {
    print_uart("=== Memory & UART Test ===\n");

    // 写入测试数据到内存
    uint64_t* mem_base = (uint64_t*)0x30000000;

    print_uart("Writing test pattern to memory at 0x");
    print_uart_addr((uint64_t)mem_base);
    print_uart("\n");

    // 写入测试模式
    for (int i = 0; i < 4; i++) {
        *(mem_base + i) = (uint64_t)(0x1122334455667788ULL + i);

        print_uart("Wrote to offset ");
        print_uart_char(i + '0');
        print_uart(": 0x");
        print_uart_addr(*(mem_base + i));
        print_uart("\n");
    }

    print_uart("Reading back from memory:\n");
    for (int i = 0; i < 4; i++) {
        uint64_t read_value = *(mem_base + i);
        print_uart("Read from offset ");
        print_uart_char(i + '0');
        print_uart(": 0x");
        print_uart_addr(read_value);
        print_uart("\n");
    }
}

// 压力测试 - 发送大量数据
void test_uart_stress() {
    print_uart("=== Stress Test ===\n");

    for (int i = 0; i < 10; i++) {
        print_uart("Stress test iteration ");
        print_uart_char((i / 10) + '0');
        print_uart_char((i % 10) + '0');
        print_uart(": ");

        // 发送一串连续字符
        for (char c = 'A'; c <= 'Z'; c++) {
            print_uart_char(c);
        }
        print_uart("\n");
    }
}

// 测试串行读取功能
void test_load_uart_char() {
    print_uart("=== Read Test ===\n");
    print_uart("Type 5 characters (will echo back):\n");

    uint8_t received_char;
    int char_count = 0;

    while (char_count < 5) {
        if (load_uart_char(&received_char)) {
            print_uart("Received: ");
            print_uart_byte(received_char);
            print_uart(" ('");
            print_uart_char(received_char); // Echo the character
            print_uart("')\n");
            char_count++;
        }
        // 简单延时，避免过度轮询
        for (volatile int i = 0; i < 1000; i++);
    }
}

// 测试load_uart_byte函数
void test_load_uart_byte() {
    print_uart("=== Load Byte Test ===\n");
    print_uart("Please input 3 bytes in hex format (e.g., FF A0 12):\n");

    uint8_t loaded_bytes[3];

    for (int i = 0; i < 3; i++) {
        print_uart("Enter byte ");
        print_uart_char(i + '0');
        print_uart(" (2 hex digits): ");

        load_uart_byte(&loaded_bytes[i]);

        print_uart("Loaded: 0x");
        print_uart_byte(loaded_bytes[i]);
        print_uart("\n");
    }

    print_uart("Summary - Loaded bytes: ");
    for (int i = 0; i < 3; i++) {
        print_uart("0x");
        print_uart_byte(loaded_bytes[i]);
        print_uart(" ");
    }
    print_uart("\n");
}

// 测试load_uart_int函数
void test_load_uart_int() {
    print_uart("=== Load Integer Test ===\n");
    print_uart("Please input 2 integers in hex format (8 digits each):\n");

    uint32_t loaded_ints[2];

    for (int i = 0; i < 2; i++) {
        print_uart("Enter 32-bit integer ");
        print_uart_char(i + '0');
        print_uart(" (8 hex digits): ");

        load_uart_int(&loaded_ints[i]);

        print_uart("Loaded: 0x");
        print_uart_int(loaded_ints[i]);
        print_uart("\n");
    }

    print_uart("Summary - Loaded integers:\n");
    for (int i = 0; i < 2; i++) {
        print_uart("Int ");
        print_uart_char(i + '0');
        print_uart(": 0x");
        print_uart_int(loaded_ints[i]);
        print_uart("\n");
    }
}

// 测试load_uart_addr函数
void test_load_uart_addr() {
    print_uart("=== Load Address Test ===\n");
    print_uart("Please input 2 addresses in hex format (16 digits each):\n");

    uint64_t loaded_addrs[2];

    for (int i = 0; i < 2; i++) {
        print_uart("Enter 64-bit address ");
        print_uart_char(i + '0');
        print_uart(" (16 hex digits): ");

        loaded_addrs[i] = 0; // 初始化为0
        load_uart_addr(&loaded_addrs[i]);

        print_uart("Loaded: 0x");
        print_uart_addr(loaded_addrs[i]);
        print_uart("\n");
    }

    print_uart("Summary - Loaded addresses:\n");
    for (int i = 0; i < 2; i++) {
        print_uart("Addr ");
        print_uart_char(i + '0');
        print_uart(": 0x");
        print_uart_addr(loaded_addrs[i]);
        print_uart("\n");
    }
}

// 测试load_uart函数（字符串读取）
void test_load_uart_string() {
    print_uart("=== Load String Test ===\n");
    print_uart("Please type strings (end with '#'):\n");

    char loaded_strings[3][64]; // 3个字符串，每个最多63字符

    for (int i = 0; i < 3; i++) {
        print_uart("Enter string ");
        print_uart_char(i + '0');
        print_uart(" (end with '#'): ");

        load_uart(loaded_strings[i], '#');

        print_uart("Loaded: \"");
        print_uart(loaded_strings[i]);
        print_uart("\"\n");
    }

    print_uart("Summary - Loaded strings:\n");
    for (int i = 0; i < 3; i++) {
        print_uart("String ");
        print_uart_char(i + '0');
        print_uart(": \"");
        print_uart(loaded_strings[i]);
        print_uart("\"\n");
    }
}

// 测试load_uart_timeout函数
void test_load_uart_timeout() {
    print_uart("=== Load Timeout Test ===\n");
    print_uart("Type something within 30 seconds (end with '*'):\n");

    char timeout_buffer[100];
    uint32_t timeout = 50000000 * 30;    // 50MHz * 30 seconds

    load_uart_timeout(timeout_buffer, '*', sizeof(timeout_buffer), timeout);

    print_uart("Result: \"");
    print_uart(timeout_buffer);
    print_uart("\"\n");

    if (timeout_buffer[0] == '\0') {
        print_uart("Timeout occurred - no input received\n");
    }
    else {
        print_uart("Input received successfully\n");
    }
}

// 综合测试 - 回环测试
void test_uart_loopback() {
    print_uart("=== Loopback Test ===\n");
    print_uart("This test will print data and ask you to type it back\n");

    // 测试数据
    uint32_t test_int = 0x12345678;
    uint64_t test_addr = 0x1234567890ABCDEFULL;
    uint8_t test_byte = 0xAB;

    // 输出测试数据
    print_uart("Please type back the following data:\n");

    print_uart("1. Integer (8 hex digits): ");
    print_uart_int(test_int);
    print_uart("\n");

    print_uart("2. Address (16 hex digits): ");
    print_uart_addr(test_addr);
    print_uart("\n");

    print_uart("3. Byte (2 hex digits): ");
    print_uart_byte(test_byte);
    print_uart("\n");

    // 读取用户输入
    uint32_t input_int;
    uint64_t input_addr = 0;
    uint8_t input_byte;

    print_uart("\nNow type them back:\n");

    print_uart("Enter integer:\n");
    load_uart_int(&input_int);

    print_uart("Enter address:\n");
    load_uart_addr(&input_addr);

    print_uart("Enter byte:\n");
    load_uart_byte(&input_byte);

    // 验证结果
    print_uart("\n=== Verification ===\n");

    print_uart("Integer - Expected: 0x");
    print_uart_int(test_int);
    print_uart(", Got: 0x");
    print_uart_int(input_int);
    print_uart(input_int == test_int ? " ✓\n" : " ✗\n");

    print_uart("Address - Expected: 0x");
    print_uart_addr(test_addr);
    print_uart(", Got: 0x");
    print_uart_addr(input_addr);
    print_uart(input_addr == test_addr ? " ✓\n" : " ✗\n");

    print_uart("Byte - Expected: 0x");
    print_uart_byte(test_byte);
    print_uart(", Got: 0x");
    print_uart_byte(input_byte);
    print_uart(input_byte == test_byte ? " ✓\n" : " ✗\n");
}

int main() {
    // 初始化UART
    print_uart("Initializing UART...\n");
    init_uart(10000000, 101000);

    print_uart("\n");
    print_uart("========================================\n");
    print_uart("      UART Function Test Suite\n");
    print_uart("========================================\n");

    // 测试基本字符串输出
    print_uart("=== String Test ===\n");
    for (int i = 0; i < 6; i++) {
        print_uart(test_strings[i]);
    }

    // 测试整数输出
    test_uart_integers();

    // 测试地址输出  
    test_uart_addresses();

    // 测试字节输出
    test_uart_bytes();

    // 测试内存和UART结合
    // test_memory_uart();

    // 压力测试
    test_uart_stress();

    print_uart("\n");
    print_uart("========================================\n");
    print_uart("      UART Load Function Tests\n");
    print_uart("========================================\n");

    // 输入测试（交互式）
    // test_load_uart_char();
    // test_load_uart_byte();
    // test_load_uart_string();
    test_load_uart_int();
    // test_load_uart_addr();
    // test_load_uart_timeout();
    test_uart_loopback();

    print_uart("\n");
    print_uart("========================================\n");
    print_uart("      All UART Tests Completed!\n");
    print_uart("========================================\n");
    print_uart("\n\n");

    return 0;
}
