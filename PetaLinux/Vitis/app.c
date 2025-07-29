#include "xparameters.h"
#include "xil_io.h"
#include <math.h>
#include <unistd.h>
#include <stdio.h>

#define BASE 0x43C00000

#define CTRL_RST	0x00
#define CONF_ALPHA	0x04
#define R			0x08
#define L            0x0C
#define TS_L         0x10
#define E            0x14
#define VM           0x18
#define R_INV        0x1C
#define V            0x20
#define I            0x24

#define PI        3.14159265359
#define F_CLK     1000000.0
#define TS        (1.0 / F_CLK)

#define VM_voltios	110.0
#define R_ohms		10.0
#define L_henrios	0.0
#define E_voltios	0.0
#define CONF		0//0:No controlado, 1:Controlado, 2:Semi Controlado
#define ALPHA_g     30.0

int32_t aQ16_15(float num) {
    return (int32_t)(num * (1 << 15));
}

int32_t aQ1_30(float num) {
    return (int32_t)(num * (1 << 30));
}

int32_t aQ0_24(float grados) {
    float rads = grados * (PI/180.0);
    return (int32_t)((rads/(2.0 * PI))*(1 << 24));
}

int main(){
    int32_t alpha_q24 = aQ0_24(ALPHA_g);
    int32_t r_q15     = aQ16_15(R_ohms);
    int32_t l_q15     = aQ16_15(L_henrios);
    int32_t e_q15     = aQ16_15(E_voltios);
    int32_t vm_q15    = aQ16_15(VM_voltios);
    int32_t ts_l_q30  = aQ1_30(TS/L_henrios);
    int32_t r_inv_q30 = aQ1_30(1.0/R_ohms);

    Xil_Out32(BASE + CONF_ALPHA, ((CONF & 0xF) << 0) | ((alpha_q24 & 0xFFFFFF) << 4));
    Xil_Out32(BASE + R, r_q15);
    Xil_Out32(BASE + L, l_q15);
    Xil_Out32(BASE + TS_L, ts_l_q30);
    Xil_Out32(BASE + E, e_q15);
    Xil_Out32(BASE + VM, vm_q15);
    Xil_Out32(BASE + R_INV, r_inv_q30);

    Xil_Out32(BASE + CTRL_RST, 0x00000001);

    usleep(200000);//200ms

    while(1) {
    //lectura de salidas
    int32_t v_pl = (int32_t)Xil_In32(BASE + V);
    int32_t i_pl = (int32_t)Xil_In32(BASE + I);

    float v_real = v_pl / (float)(1 << 15);
    float i_real = i_pl / (float)(1 << 15);

    printf("Vo = %.4f V, Io = %.4f A\n", v_real, i_real);
	usleep(2000); // 500 Hz
    }

	return 0;
}
