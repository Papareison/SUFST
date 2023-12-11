#define MAGIC_COEFFICIENT 2.25

double PID(double ratio){

    double PIDoutput;
    double integral_sum = 0;
    double previous_error = 0;

    double idealratio = 0.1;

    double usedratio = MAGIC_COEFFICIENT * idealratio; 
    
    double PC = 0.65;    // Proportional coefficient    // These need to be ORDERS of magnitude smaller than the ideal ratio and definitely not larger
    double IC = 20;     // Integral coefficient        
    double DC = 0.00013; // Derivative coefficient, VERY sensitive
    
    double dt = 0.001; // Time step

        // Calculate the error
        double error = usedratio - ratio;

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

        if(PIDoutput < 0){ // Slip ratio shouldn't take negative values
            PIDoutput = 0;
        }

        // printf("PID = %lf\n", PIDoutput);

return PIDoutput;

}



