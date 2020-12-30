#include <stdio.h>
#include <stdlib.h>

#define MDebug printf
#define D1 2
#define D2 1
#if 0
double divop(double orig, int slots) {
    MDebug(".");
    if (slots == 1 || orig == 0)
        return orig;
    int od = slots & 1;
    double result = divop(orig / D1, od ? (slots + D2) >> 1 : slots >> 1);
    MDebug("o");
    if (od) {
        result += divop(result, slots);
        MDebug("i");
    }
    MDebug("e");
    return result;
}
#else
double divop(double orig, int slots) {
    MDebug("L%f,%d\n",orig,slots);
    if (slots == 1 || orig == 0)
        return orig;
    int od = slots & 1;
    double result = divop(orig / D1, od ? (slots + D2) >> 1 : slots >> 1);
    MDebug("o");
    if (od) {
        result += divop(result, slots);
        MDebug("i");
    }
    MDebug("\n");
    return result;
}
#endif

int main(int argc, char *argv[]) {
  double adOrigs[] = {10, 4, 128, 6, 30};
  int aiSlots[] = {5, 2, 16, 3, 7};
  int i = 1;
  // for( i = 0; i < sizeof(aiSlots)/sizeof(int); i++) {
    double dRet;
    dRet = divop(adOrigs[i], aiSlots[i]);
    printf("----> %f div by %d is %f\n\n", adOrigs[i], aiSlots[i], dRet);
  // }
}
