#ifndef __UTILITIES_H_
#define __UTILITIES_H_


int copy_in(unsigned char *source, unsigned char *target, int rnd, int length);
void copy_out(unsigned char *source, unsigned char *target, int rnd, int length, int outlength);
int copy_pwd(char *target, char *source);
float my_mean(float data[], int n);
float standard_deviation(float data[], int n);
void calculate_sdev_mean(int* features1, int* features2, int* features3, int* features4, int* features5, float* sdevs, float* means, int numfeatures);



#endif

