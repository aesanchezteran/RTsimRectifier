#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <math.h>

#define BASE        0x43C00000
#define SPAN        0x10000

#define CTRL_RST_OFFSET  0x00
#define CONF_ALPHA_OFFSET 0x04
#define R_OFFSET          0x08
#define L_OFFSET          0x0C
#define TS_L_OFFSET       0x10
#define E_OFFSET          0x14
#define VM_OFFSET         0x18
#define R_INV_OFFSET	  0x01C
#define V_OFFSET          0x20
#define I_OFFSET          0x24

#define SAMPLING_INTERVAL_US 8000  

static inline int32_t aQ16_15(float num) {
    return (int32_t)(num * (1 << 15));
}

static inline int32_t aQ1_30(float num) {
    return (int32_t)(num * (1 << 30));
}

int main() {
    const float PI = 3.14159265359;
    const float F_CLK = 1000000.0;
    const float TS = 1.0 / F_CLK;

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open /dev/mem");
        return 1;
    }

    void *map_base = mmap(NULL, SPAN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE);
    if (map_base == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    volatile uint32_t *CTRL_RST = (uint32_t *)(map_base + CTRL_RST_OFFSET);
    volatile uint32_t *CONF_ALPHA = (uint32_t *)(map_base + CONF_ALPHA_OFFSET);
    volatile uint32_t *R          = (uint32_t *)(map_base + R_OFFSET);
    volatile uint32_t *L          = (uint32_t *)(map_base + L_OFFSET);
    volatile uint32_t *TS_L       = (uint32_t *)(map_base + TS_L_OFFSET);
    volatile uint32_t *E          = (uint32_t *)(map_base + E_OFFSET);
    volatile uint32_t *VM         = (uint32_t *)(map_base + VM_OFFSET);
    volatile uint32_t *R_inv      = (uint32_t *)(map_base + R_INV_OFFSET);
    volatile uint32_t *V          = (uint32_t *)(map_base + V_OFFSET);
    volatile uint32_t *I          = (uint32_t *)(map_base + I_OFFSET);

    //RESET DEL HARDWARE (solo 1 vez)
    *CTRL_RST = 0x2;
    usleep(10);
    *CTRL_RST = 0x0;

    while (1) {
        //LECTURA DE ENTRADAS DESDE EL TEXT FILE
        FILE *intxt = fopen("/tmp/inputs.txt", "r");
        if (intxt) {
            int ctrl, conf;
            float alpha_g, r_ohms, l_henrios, e_voltios, vm_voltios;

            if (fscanf(intxt, "%d %d %f %f %f %f %f",
                       &ctrl, &conf, &alpha_g, &r_ohms, &l_henrios,
                       &e_voltios, &vm_voltios) == 7) {

                //TRANSFORMACION A PUNTO FIJO
                //alpha
                float alpha_rad = alpha_g * (PI / 180.0);
                int alpha_q24 = (int)((alpha_rad / (2.0 * PI)) * (1 << 24));
		//r,l,e,vm
                int r_q15     = aQ16_15(r_ohms);
                int l_q15     = aQ16_15(l_henrios);
                int e_q15     = aQ16_15(e_voltios);
                int vm_q15    = aQ16_15(vm_voltios);
		//TS/L
                float ts_l = TS / l_henrios;
                int ts_l_q30  = aQ1_30(ts_l);
		//1/R
                float r_inv = (r_ohms != 0.0f) ? 1.0f / r_ohms : 0.0f;
                int r_inv_q30 = aQ1_30(r_inv);

                //ESCRITURA DE ENTRADAS A LOS REGISTROS
                *CONF_ALPHA = ((conf & 0xF) << 0) | ((alpha_q24 & 0x0FFFFFFF) << 4);
                *R          = r_q15;
                *L          = l_q15;
                *TS_L       = ts_l_q30;
                *E          = e_q15;
                *VM         = vm_q15;
                *R_inv      = r_inv_q30;
                *CTRL_RST = (ctrl & 0x1);
            }

            fclose(intxt);
        }

        // LECTURA DE SALIDAS 
        int32_t v_raw = (int32_t)(*V);
        int32_t i_raw = (int32_t)(*I);
	//TRANSFORMACION DE PUNTO FIJO A REALES
        float v_real = v_raw / (float)(1 << 15);
        float i_real = i_raw / (float)(1 << 15);

        // ESCRITURA DE LAS SALIDAS EN EL PIPE
        FILE *pipe = fopen("/tmp/ws_pipe", "w");
        if (pipe) {
            fprintf(pipe, "%.5f %.5f\n", v_real, i_real);
            fclose(pipe);
        }

        usleep(SAMPLING_INTERVAL_US);
    }

    return 0;
}
