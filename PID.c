#include <stdio.h>


int main(){

    double PIDoutput;
    double integral_sum = 0;
    double previous_error = 0;
    double idealratio = 0.1;
    double ratio = 0.2;

    double PC = 0.75;  // Proportional coefficient    // These need to be ORDERS of magnitude smaller than the ideal ratio and definitely not larger
    double IC = 1;  // Integral coefficient
    double DC = 0.0001; // Derivative coefficient
    double dt = 0.001; // Time step

    double P, I, D;

    double error, delta_error;

    for(;;) {
        // Calculate the error
        error = idealratio - ratio;

        // Calculate P, I, and D terms
        P = PC * error; // P

        integral_sum += error * dt; // I
        I = IC * integral_sum;

        delta_error = (error - previous_error) / dt; // D
        D = DC * delta_error;

        // Update the previous error
        previous_error = error;

        // Calculate the control output
        PIDoutput = P + I + D;

        ratio = PIDoutput;

        printf("PID = %lf\n", PIDoutput);
    }

return 0;

}



