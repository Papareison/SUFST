#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
// PID parameters
#define KP 0.77   // Proportional gain
#define KI 5 // Integral gain
#define KD 0.000105  // Derivative gain

// PID variables
float error_integral = 0;
float previous_error = 0;

// Function to calculate PID control signal
float calculate_pid(float setpoint, float process_variable) {
    // Calculate error
    float error = setpoint - process_variable;

    // Calculate proportional term
    float proportional = KP * error;

    // Calculate integral term
    error_integral += error;
    float integral = KI * error_integral;

    // Calculate derivative term
    float derivative = KD * (error - previous_error);
    previous_error = error;

    // Calculate PID control signal
    float control_signal = proportional + integral + derivative;

    return control_signal;
}

int main() {
    // Example usage
    float setpoint = 50;  // Desired setpoint
    float process_variable = 0;  // Current process variable
    FILE *fpt;
    fpt = fopen("PIDOUT.csv", "w+");
    fprintf(fpt,"Time, PIDout\n");
    // Simulate the process
    for (int i = 0; i < 100; i++) {
        fprintf(fpt,"%d, %f\n",i, process_variable);
        // Calculate PID control signal
        float control_signal = calculate_pid(setpoint, process_variable);

        // Apply control signal to the process (for example, update the process variable)
        process_variable += control_signal;

        // Print the control signal and process variable
        printf("Control Signal: %.2f, Process Variable: %.2f\n", control_signal, process_variable);
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
    return 0;
}