#include <cstdio>
#include "../src/Cpu8080.h"

int main(void)
{
    Cpu8080 cpu;

    int diag_size;

    uint8_t memory[0x4000];
    uint8_t ports[0x7];

    memset(memory, 0, sizeof(memory));
    memset(ports, 0, sizeof(ports));

    /* Open the diag file */
    FILE *fp = fopen("cpudiag.bin", "rb");
    if (!fp) {
        printf("Failed to open cpudiag.bin\n");
        return -1;
    } else {
        printf("Opened cpudiag.bin\n");
    }

    /* Get its size */
    fseek(fp, 0, SEEK_END);
    diag_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    printf("Diag size: %d\n", diag_size);


    /* Read the file into cpu memory */
    int num_read = fread(&memory[0x100], 1, diag_size, fp);
    printf("Read from file: %d\n", num_read);

    /* We're done with the file now */
    fclose(fp);



    /* Make the first instruction JMP 0x100 */
    memory[0] = 0xC3;
    memory[1] = 0x00;
    memory[2] = 0x01;

    /* Make the stack pointer 0x7ad instead of 0x6ad */
    memory[368] = 0x7;

    /* Skip DAA test */
    memory[0x59C] = 0xC3;
    memory[0x59D] = 0xC2;
    memory[0x59E] = 0x05;


    /* Init the CPU */
    cpu.init(memory, ports);

    for (;;)
    {
        cpu.run_next_op();
    }

    return 0;
}


