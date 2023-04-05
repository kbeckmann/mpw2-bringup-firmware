#include "firmware/defs.h"

// --------------------------------------------------------
// Firmware routines
// --------------------------------------------------------

#define MPRJ_BASE 0x30000000
#define REG0 ((volatile uint32_t*) (MPRJ_BASE + 0x00))
#define REG1 ((volatile uint32_t*) (MPRJ_BASE + 0x04))
#define REG2 ((volatile uint32_t*) (MPRJ_BASE + 0x08))
#define REG3 ((volatile uint32_t*) (MPRJ_BASE + 0x0c))

#define MPRJ_FB 0x30000100
#define FB0 ((volatile uint32_t*) (MPRJ_FB + 0x00))
#define FB1 ((volatile uint32_t*) (MPRJ_FB + 0x04))
#define FB2 ((volatile uint32_t*) (MPRJ_FB + 0x08))
#define FB3 ((volatile uint32_t*) (MPRJ_FB + 0x0c))

#define NANOSECONDS_PER_CYCLE 2700
#define CYCLES_PER_MS 370

#define FORCE_INLINE __attribute__((always_inline)) inline

static FORCE_INLINE void delay_ms(int x)
{
    // x *= CYCLES_PER_MS;
    while (x--) {
        asm("nop");
    }
}

static void putchar(char c)
{
    if (c == '\n')
        putchar('\r');
    while (reg_uart_txfull == 1);
    reg_uart_data = c;
}

static void print(const char *p)
{
    while (*p)
        putchar(*(p++));
}

static void blink_setup()
{
    reg_gpio_mode1 = 1;
    reg_gpio_mode0 = 0;
    reg_gpio_ien = 1;
    reg_gpio_oe = 1;
}

static FORCE_INLINE void blink_short()
{
    reg_gpio_out = 0;
    delay_ms(500); // ON
    reg_gpio_out = 1;
    delay_ms(500); // OFF
}

static void blink_long()
{
    reg_gpio_out = 0;
    delay_ms(2000); // ON
    reg_gpio_out = 1;
    delay_ms(2000); // OFF
}


/*
    IO Test:
        - Configures MPRJ lower 8-IO pins as outputs
        - Observes counter value through the MPRJ lower 8 IO pins (in the testbench)
*/

void main()
{
    /*
    IO Control Registers
    | DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
    | 3-bits | 1-bit | 1-bit | 1-bit  | 1-bit  | 1-bit | 1-bit   | 1-bit   | 1-bit | 1-bit | 1-bit   |

    Output: 0000_0110_0000_1110  (0x1808) = GPIO_MODE_USER_STD_OUTPUT
    | DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
    | 110    | 0     | 0     | 0      | 0      | 0     | 0       | 1       | 0     | 0     | 0       |


    Input: 0000_0001_0000_1111 (0x0402) = GPIO_MODE_USER_STD_INPUT_NOPULL
    | DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
    | 001    | 0     | 0     | 0      | 0      | 0     | 0       | 0       | 0     | 1     | 0       |

    */


    // reg_mprj_io_8    =  GPIO_MODE_USER_STD_OUTPUT;
    // reg_mprj_io_9 =  GPIO_MODE_USER_STD_OUTPUT;
    // reg_mprj_io_10 =  GPIO_MODE_USER_STD_OUTPUT;
    // reg_mprj_io_11 =  GPIO_MODE_USER_STD_OUTPUT;
    // reg_mprj_io_12 =  GPIO_MODE_USER_STD_OUTPUT;
    // reg_mprj_io_13 =  GPIO_MODE_USER_STD_OUTPUT;
    // reg_mprj_io_14 =  GPIO_MODE_USER_STD_OUTPUT;
    // reg_mprj_io_15 =  GPIO_MODE_USER_STD_OUTPUT;

    // PDM out
    // reg_mprj_io_16 =  GPIO_MODE_USER_STD_OUTPUT;

    // HS
    // reg_mprj_io_17 =  GPIO_MODE_USER_STD_OUTPUT;
    // VS
    // reg_mprj_io_18 =  GPIO_MODE_USER_STD_OUTPUT;

    // R
    // reg_mprj_io_19 =  GPIO_MODE_USER_STD_OUTPUT;
    // G
    // reg_mprj_io_20 =  GPIO_MODE_USER_STD_OUTPUT;
    // B
    // reg_mprj_io_21 =  GPIO_MODE_USER_STD_OUTPUT;

    // clock out
    reg_mprj_io_22 =  GPIO_MODE_USER_STD_OUTPUT;

    // gpio_config_io();

    reg_gpio_mode1 = 1;
    reg_gpio_mode0 = 0;
    reg_gpio_ien = 0;
    reg_gpio_oe = 1;

    // LED ON
    // reg_gpio_out = 0;


    // blink gpio forever
    // while (1) {
    //     reg_gpio_out = 0;
    //     reg_gpio_out = 1;
    // }

    // while (1) {
    //     blink_short();
    // }

    // blink_short();
    // blink_short();

    // activate the project by setting the 1st bit of 2nd bank of LA
    reg_la1_iena = 0;
    reg_la1_oenb = 0;
    reg_la1_data = 1 << 12;  // project #12

    // reset design with 0bit of 1st bank of LA
    reg_la0_iena = 0;
    reg_la0_oenb = 0;
    reg_la0_data = 1;
    // delay_ms(100);
    reg_la0_data = 0;

    // LED ON
    reg_gpio_out = 0;

    // set the screen to be white
    *REG2 = 0x00FF00FF;
    *REG3 = 0x00000000;

    *FB0  = 0xFFFFFFFF;
    *FB1  = 0xFFFFFFFF;
    *FB2  = 0xFFFFFFFF;
    *FB3  = 0xFFFFFFFF;

    *REG0 = 0x7FFF;

    // no need for anything else as this design is free running.
    while (1) {
        // blink_short();
        for (int i = 0; i < 0xFFFF; i += 0xFF)
            *REG0 = i;
    }
}
