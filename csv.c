#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>

//      RUN THIS FROM THE TERMINAL, IDEs WILL LAG LIKE HELL                                                        
//      USE FOR TESTING IDEAL COEFFICIENT VALUES AND TO GRAPH THE RESULTS FROM THE RESULTING CSV USING EXCEL       
//      CHANGE THE COEFFICIENTS AT THE "double PID()" FUNCTION BELOW
//      RUNNING THE PROGRAM AGAIN WILL OVERWRITE THE PREVIOUS CSV FILE                                              


volatile sig_atomic_t flag = 0; // Set flag to 0 so loop works

int main (int argc, char *argv[]) {

    uint64_t i = 0; // 64 bits is a safety measure to make sure overflow doesn't occur, the PID() goes through a tremendous amount of iterations

    signal(SIGINT, handle_sigint); // Checks for CTRL+C from the terminal and turns flag variable to 1 when it is pressed

    double *results = malloc(1 * sizeof(double)); // Holds each iteration of the PID output

    while(!flag){ // Infinite loop until CTRL+C is pressed in the terminal
        results[i] = PID(); // Writing the value to the nth element
        results = realloc(results, i * sizeof(*results)); // Allocating more memory to the array, per iteration
        i++;
    }

    writefinal(i, results); // Final write to csv file

    return 0;
}

void writefinal(const int rows, const double *results){

    int i;

    FILE *csv;
    csv = fopen("PID.csv", "w+");

    for(i = 0;  rows > i; i++) {
        fprintf(csv,"%d, %lf \n", i, results[i]);
    }

    fclose(csv);

    free(results); // Redundant, but good practice... Not like it'll help...

    printf("DONE!!!\n");
}

void handle_sigint(int sig) { // Turns flag to 1 depending on the int provided
    flag = 1;
}

double PID(){

    double PIDoutput;
    double integral_sum;
    double previous_error = 0;
    uint64_t i = 0;

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

        if(i % 50 == 0){ // Reduce terminal clutter, doesn't affect end result
            printf("PID = %lf\n", PIDoutput);
        }
        i++;
    }

return PIDoutput;

}
