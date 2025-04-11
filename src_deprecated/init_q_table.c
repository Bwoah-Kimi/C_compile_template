//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-04-09
// Description:     C Code to Initialize Q-table.
//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "init_q_table.h"

void init_q_table(void)
{
    const int NUM_STATES = 512;
    const int ACTIONS_PER_TABLE = 4;
    const uint64_t Q_TABLE_0_BASE_ADDR = 0x60000018; // Replace with actual address
    const uint64_t Q_TABLE_1_BASE_ADDR = 0x60001018; // Replace with actual address

    uint64_t *q_table_0_base = (uint64_t *)Q_TABLE_0_BASE_ADDR;
    uint64_t *q_table_1_base = (uint64_t *)Q_TABLE_1_BASE_ADDR;

    for (int i = 0; i < NUM_STATES; i++)
    {
        uint64_t q_values_0 = 0;
        uint64_t q_values_1 = 0;

        for (int j = 0; j < ACTIONS_PER_TABLE * 2; j++)
        {
            uint16_t q_value;

            // Calculate q_value based on pattern
            switch ((j + i) % 4)
            {
            case 0:
                q_value = 0x0100 + 2 * i + j * 0x0020;
                break;
            case 1:
                q_value = 0xFF00 - i - j * 0x0025;
                break;
            case 2:
                q_value = 0x0000;
                break;
            case 3:
                q_value = 0x0080 + i + j * 0x0010;
                break;
            }

            if (j < ACTIONS_PER_TABLE)
            {
                // Place each action's Q-value in the correct position for table 0
                q_values_0 |= ((uint64_t)q_value << (j * 16));
            }
            else
            {
                // Place each action's Q-value in the correct position for table 1
                q_values_1 |= ((uint64_t)q_value << ((j - ACTIONS_PER_TABLE) * 16));
            }
        }

        // Write values to Q-tables
        q_table_0_base[i] = q_values_0;
        q_table_1_base[i] = q_values_1;
    }
}
