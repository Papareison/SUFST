#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

float PC = 0.1;  // Proportional coefficient
float IC = 0;  // Integral coefficientss
float DC = 0; // Derivative coefficient
float integralSum = 0;
float previousError = 0;
float error = 0;
float idealSlip = 1.1;
float currentTime = 0.0;
float timeStep = 0.01;
float output = 0;

float PIDForTorque(float currSlip) { // error based PID, compares desires slip ratio to current getting SLIP
    error = idealSlip - currSlip; // calc error
    printf("ideal (%f) - current (%f)\n", idealSlip, currSlip);
    printf("Error: %f\n", error);
    float P = PC * error; // calc P
    integralSum += error * timeStep;
    float I = IC * integralSum; // calc I
    float diffInError = (error - previousError) / timeStep;
    float D = DC * diffInError; // calc D

    previousError = error;
    output = P + I + D;
    return (output);  
}

void main(){
    FILE *fpt;
    fpt = fopen("PIDOUT.csv", "w+");
    float slip= 0.0;
    fprintf(fpt,"Time, PIDout\n");
    while (currentTime<10)
    {
        slip = PIDForTorque(slip);
        fprintf(fpt,"%f, %f\n",currentTime, slip);
        currentTime+=timeStep;
    }
    fclose(fpt);
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
    if (gnuplotPipe) {
        fprintf(gnuplotPipe, "load 'outScript.plt'\n");
        fflush(gnuplotPipe);
        pclose(gnuplotPipe);
    } else {
        printf("Error opening gnuplot.\n");
    }
}