#include <stdio.h>


int main(){

    double PIDoutput;
    double integral_sum = 0;
    double previous_error = 0;
    int i = 0;

    double idealratio = 0.1;
    double ratio = 0.5;
    
    double PC = 0.12;  // Proportional coefficient    // These need to be ORDERS of magnitude smaller than the ideal ratio and definitely not larger
    double IC = 0.005;  // Integral coefficient       // ***TODO***: Tune the coefficients to ensure optimal behavior
    double DC = 0.0001; // Derivative coefficient
    
    double dt = 0.001; // Time step

    for(;;) {
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

        ratio = PIDoutput;


        if(i % 10 == 0){
            printf("PID = %lf\n", PIDoutput);
        }
        i++;
    }

return 0;

}