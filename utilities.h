#ifndef __UTILITIES_H_
#define __UTILITIES_H_


int copy_in(unsigned char *source, unsigned char *target, int rnd, int length);
void copy_out(unsigned char *source, unsigned char *target, int rnd, int length, int outlength);
int copy_pwd(char *target, char *source);

#endif

