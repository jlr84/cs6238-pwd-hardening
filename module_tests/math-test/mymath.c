#include "../../utilities.h"
#include "../../utilities.c"

//Compile with gcc ./mymath.c -lm

int main() {

    int numf = 14;
    int fea1[] = {16,11,7,20,9,4,1,13,4,21,26,16,27,16};
    int fea2[] = {18,11,-1,20,9,5,3,10,9,14,26,17,30,15};
    int fea3[] = {20,14,2,18,10,5,5,7,11,19,27,18,29,7};
    int fea4[] = {13,14,5,19,11,4,-2,9,6,20,28,17,29,19};
    int fea5[] = {18,10,-2,20,10,4,2,8,6,17,23,20,27,7};
    float sdevsP[numf];
    float sdevsS[numf];
    float means[numf];

    calculate_sdev_mean(fea1, fea2, fea3, fea4, fea5, sdevsP, means, numf);
    calculate_sdev_mean2(fea1, fea2, fea3, fea4, fea5, sdevsS, means, numf);
    int i;
    for ( i = 0; i < numf; i++ ) {
        printf("\n#%2d\n",i+1);
        printf("%d, %d, %d, %d, %d\n",fea1[i],fea2[i],fea3[i],fea4[i],fea5[i]);
        printf("SDev(Pop): %.3f\nSDev(Sam): %.3f\nMean: %.3f\n", sdevsP[i], sdevsS[i], means[i]);
    }
    return 0;
}

