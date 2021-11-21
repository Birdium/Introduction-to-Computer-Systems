#include <stdint.h>
#include <fixedptc.h>

int main(){
    // float a = 1.2;
    // float b = 10;
    // int c = 0;
    // if (b > 7.9) {
    //     c = (a + 1) * b / 2.3;
    // }
    fixedpt a = fixedpt_rconst(1.2);
    fixedpt b = fixedpt_fromint(10);
    // printf("%d\n",fixedpt_toint(fixedpt_floor(a)));
    // printf("%d\n",fixedpt_toint(fixedpt_ceil(a)));
    // printf("%d\n",fixedpt_toint(fixedpt_floor(b)));
    // printf("%d\n",fixedpt_toint(fixedpt_ceil(b)));
    printf("%x\n",a*b);
    printf("%x\n",b);
    int c = 0;
    if (b > fixedpt_rconst(7.9)) {
        c = fixedpt_toint(fixedpt_div(fixedpt_mul(a + FIXEDPT_ONE, b), fixedpt_rconst(2.3)));
        // c = fixedpt_toint(fixedpt_mul(a + FIXEDPT_ONE, b));
    }
    printf("%d\n",c);
    
    return 0;
}