#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <windows.h>
#include <unistd.h>

//      RUN THIS FROM THE TERMINAL, IDEs WILL LAG LIKE HELL                                                        
//      USE FOR TESTING IDEAL COEFFICIENT VALUES AND TO GRAPH THE RESULTS FROM THE RESULTING CSV USING EXCEL       
//      CHANGE THE COEFFICIENTS AT THE "double PID()" FUNCTION BELOW
//      RUNNING THE PROGRAM AGAIN WILL OVERWRITE THE PREVIOUS CSV FILE                                              

static volatile sig_atomic_t flag = 1; // Set flag to 1 so loop works

double idealratio = 0.1;

double dt = 0.001; // Time step

double PC = 0.77;    // Proportional coefficient    // These need to be ORDERS of magnitude smaller than the ideal ratio and definitely not larger
double IC = 10;     // Integral coefficient
double DC = 0.000105; // Derivative coefficient, VERY sensitive

double integral_sum = 0;

double previous_error = 0;

void writefinalresults(const unsigned long int rows, double *results);

double PID(double ratio);

static void signal_handler(int _);

int main (int argc, char *argv[]){

    unsigned long int i = 0;

    signal(SIGINT, signal_handler); // Checks for CTRL+C from the terminal and turns flag variable to 1 when it is pressed

    double *results = malloc(sizeof(double)); // Holds each iteration of the PID output
    results[0] = 0.2;

    LARGE_INTEGER frequency, start, end;

    double seconds;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    while(flag){ // Infinite loop until CTRL+C is pressed in the terminal
        double *temp = realloc(results, (i + 2) * sizeof(*results)); // Allocating more memory to the array, per iteration
        if(temp == NULL) {
            printf("Memory Allocation Failed! Fuck You!");
            exit(1);
        }
        results = temp;
        results[i + 1] = PID(results[i]); // Writing the value to the nth element
        i++;
    }

    QueryPerformanceCounter(&end);
    seconds = (double)(end.QuadPart - start.QuadPart) / (double)frequency.QuadPart; // Time measurement which divides by frequency using windows.h

    writefinalresults(i, results); // Final write to csv file

    printf("Total duration: %lfs\n", seconds);

    return 0;
}

void writefinalresults(const unsigned long int rows, double *results){

    unsigned long int i;

    FILE *csv;
    csv = fopen("PID.csv", "w+");

    for(i = 0;  rows > i; i++) {
        fprintf(csv,"%ld, %lf \n", i, results[i]);
    }

    fclose(csv);

    free(results); // Redundant, but good practice... Not like it'll help...

    printf("DONE!!!\n");
}

static void signal_handler(int _)
{
    (void)_;
    flag = 0;
}

double PID(double ratio){

    double PIDoutput;

    // Calculate the error
    double error = idealratio - ratio;

    // Calculate P, I, and D terms
    double P = PC * error; // P

    integral_sum += error * dt; // I
    double I = IC * integral_sum;

    double delta_error = (error - previous_error) / dt; // D
    double D = DC * delta_error;

    // Update the previous error
    previous_error = error;

    // Calculate the control output
    PIDoutput = P + I + D;

    printf("PID = %lf\n", PIDoutput);

    return PIDoutput;

}
