#include <stdio.h>
#include <math.h>

/* Copies string up to length 'length' from source to target; position within 
   string source can change depending upon with round, 'rnd'; position within 
   string target is always at the beginning of the string */
int copy_in(unsigned char *source, unsigned char *target, int rnd, int length) {
        int c = (rnd - 1) * length;
        int d = 0;
        int end = c + length;
        while (source[c] != '\0' && c < end ) {
                target[d] = source[c];
                c++;
                d++;
        }
        target[c] = '\0';
        return d;
}

//Copies string up to length outlength from source to target
void copy_out(unsigned char *source, unsigned char *target, int rnd, int length, int outlength) {
        int c = (rnd - 1) * length;
        int d = 0;
        int end = outlength;
        while ( source[d] != '\0' && d < end ) {
                target[c] = source[d];
                c++;
                d++;
        }
        target[c] = '\0';
}

/* Copies password from source to target, ignoring the newline character, thus 
   removing newline from the copied password returned in target */
int copy_pwd(char *target, char *source) {
        int c = 0;
        while ( source[c] != '\n' && source[c] != '\0' ) {
                target[c] = source[c];
                c++;
        }
        target[c] = '\0';
        return c;
}

/* Function to calculate the mean given an array of data
   and the number of points in the array */
float my_mean(float data[], int n) {
    float mean=0.0;
    int i;
    for ( i = 0; i < n; i++ ) {
        mean+=data[i];
    }
    mean=mean/n;
    return mean;
}

/* Function to calculate the standard deviation given an 
   array of data and the number of point in the array */
float standard_deviation(float data[], int n)
{
    float mean=0.0, sum_deviation=0.0;
    mean = my_mean(data,n);
    int i;
    for( i = 0; i < n; i++) {
        sum_deviation+=(data[i]-mean)*(data[i]-mean);
    }
    return sqrt(sum_deviation/n);
}

/* Function that calculates all the standard deviations and 
   and means for a set of data retrieved from the history
   file; returns sdevs[numfeatures] and means[numfeatures] */
void calculate_sdev_mean(int* features1, int* features2, int* features3, int* features4, int* features5, float* sdevs, float* means, int numfeatures) {
    int n = 5;
    int i;
    float data[n];
    for ( i = 0; i < numfeatures; i++ ) {
        data[0] = features1[i];
        data[1] = features2[i];
        data[2] = features3[i];
        data[3] = features4[i];
        data[4] = features5[i];

        sdevs[i] = standard_deviation(data,n);
        means[i] = my_mean(data,n);
        //printf("%d SDev: %.3f\nMean = %.3f\n", i+1,sdevs[i], means[i]);
    }
}

