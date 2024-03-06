#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#pragma GCC diagnostic error "-Wall"
#pragma GCC diagnostic error "-Wpedantic"

// torque limit 300 nm, dist 75m, 15 secs

// Lift coefficient for different parts of the car
const double SCLValues[5] = {1.1851,1.1801,0.3839,0.3839,0.4142};

// Drag coefficient for different parts of the car
const double SCDValues[5] = {0.3033, 0.6881, 0.1206, 0.1206, 0.1242};

const double forceX[5] = {-1.3819, 0.8295, -0.0666, -0.0666, -0.1444};
const double forceY[5] = {-0.4491, 0.2147, -0.4596, -0.4596, -0.466};
const double AIR_DENS = 1.225;
const double g = 9.81;
const int mass = 150;
const double dynamicMu = 0.7;
const double wheelRad = 7 * 2.54 * 0.01;
const double idealSlip = 0.1;
const double timeStep = 0.02;

double calculateLiftForcesRaRbAdd(double v){ //gives Ra + Rb
    double total = 0;
    double valAdd = (0.5 * (v * v) * AIR_DENS);
    total = ((SCLValues[0] * valAdd) + (SCLValues[1] * valAdd) + (SCLValues[2] * valAdd) + (SCLValues[3] * valAdd) + (valAdd * SCLValues[4])); // calc downforce
    total += g * mass; // add weight
    return total;
}

double calculateLiftMomentRB(double v){ // gives Ra - 0.5Rb
    double total_lift = 0;
    double valAdd = (0.5 * (v * v) * AIR_DENS);
    total_lift = ((valAdd * SCLValues[0] * forceX[0]) - (valAdd * SCLValues[1] * forceX[1]) + (valAdd * SCLValues[4] * forceX[4]) + (2 * valAdd * SCLValues[2] * forceX[2])); // + (valAdd * SCDValues[0] * forceY[0]) - (valAdd * SCDValues[1] * forceY[1]) + (valAdd * SCDValues[4] * forceY[4]) +  (2 * valAdd * SCDValues[2] * forceY[2]));
    return total_lift;
}

double PC = 0.4816;  // Proportional coefficient
double IC = 12;  // Integral coefficientss
double DC = 0.349; // Derivative coefficient
double integralSum = 0;
double previousError = 0;
double error = 0;

double PIDForTorque(double currSlip){ // error based PID, compares desires slip ratio to current getting SLIP
    error = idealSlip - currSlip; // calc error
    printf("ideal (%lf) - current (%lf)\n", idealSlip, currSlip);
    printf("Error: %lf\n", error);
    double P = PC * error; // calc P
    integralSum += error * timeStep;
    double I = IC * integralSum; // calc I
    double diffInError = (error - previousError) / timeStep;
    double D = DC * diffInError; // calc D

    previousError = error;

    return (P + I + D);  
}

int main(){
    double massMomentOfInertia = (mass * (wheelRad * wheelRad));
    double carVelocity = 0;
    double angularVel = 0;
    double dist = 0;
    double torque = 0;
    double force = 0;
    double desiredTorque = 230;
    double slipRatio = 0;
    double currTime = 0;
    double acc = 0;
    double tractiveLimit = 0;
    double angularAcc = 0;
    double fx = 0;
    double wheelVelocity = 0;
    double PIDval = 0;
    FILE *fpt;
    fpt = fopen("MyFile.csv", "w+");
    fprintf(fpt,"Time, TractLim, Alpha, WheelVel, Force, Acc, CarVel, slip, torque, PID, error, dist\n");
    
    while (dist < 75 && currTime < 5){
        fprintf(fpt,"%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf\n", currTime, tractiveLimit, angularAcc, wheelVelocity, force, acc, carVelocity, slipRatio, torque, PIDval, error, dist);
        printf("-----------------------\n");

        printf("Current time: %lf\n", currTime);
        printf("Tractive limit: %lf\n", tractiveLimit);
        printf("Force: %lf\n", force);
        printf("Torque: %lf\n",torque);
        printf("dist: %lf\n", dist);
        printf("Car velocity: %lf\n", carVelocity);
        printf("Wheel Vel: %lf\n", wheelVelocity);
        printf("Slip: %lf\n",slipRatio);
        printf("PID output: %lf\n", PIDval);

        double Rb = (calculateLiftForcesRaRbAdd(carVelocity) - calculateLiftMomentRB(carVelocity)) / 1.5; // rear axle reactant force ***SUS!!!!!!!!!!!!!!!!!!!!!!!!!!***
        // printf("Rb: %lf\n", Rb);
    
        tractiveLimit = fabs(Rb * dynamicMu);

        force = torque / wheelRad;
        // printf("Force: %lf\n", force);
        
        //swapped around
        if (force < tractiveLimit){ 
            //wont slip
            fx = force;
            acc = fx / mass;
            angularAcc = acc / wheelRad;
        }
        // else if(time == 0){
        //     slipRatio = 0;
        // }
        else{
            //will slip
            //slipRatio = 0;
            fx = tractiveLimit;
            acc = fx / mass;
            angularAcc = (torque - (tractiveLimit * wheelRad)) / massMomentOfInertia;
        }
        // printf("Torque: %lf\n",torque);
    
        // calculate distance with suvat
        dist += (carVelocity * timeStep) + 0.5 * (acc * (timeStep * timeStep));
        // printf("dist: %lf\n", dist);

        carVelocity += acc * timeStep;
        currTime += timeStep;
        // printf("Car velocity: %lf\n", carVelocity);

        angularVel = angularAcc * timeStep;
        wheelVelocity += angularVel * wheelRad;
        // printf("Wheel Vel: %lf\n", wheelVelocity);
        
        slipRatio = (wheelVelocity - carVelocity)/wheelVelocity;
        // force defined
        if (isnan(slipRatio)){
             slipRatio = 0.0001;
        }

        // printf("Slip: %lf\n",slipRatio);
        
        PIDval = PIDForTorque(slipRatio);
        torque = PIDval * desiredTorque;
        
        // force torque to stay below cap
        if (torque > desiredTorque){
            torque = desiredTorque;
        }
        // printf("PID output: %lf\n", PIDval);
    }
    fclose(fpt);
    printf("Total time = %lf", currTime);
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
    if (gnuplotPipe) {
        fprintf(gnuplotPipe, "load 'plotScript.plt'\n");
        fflush(gnuplotPipe);
        //printf("Press enter to exit...");
        //getchar(); // Wait for user to press enter
        pclose(gnuplotPipe);
    } else{
        printf("Error opening gnuplot.\n");
    }

    return 0;
}



// double calculateLiftForcesRaRbFor(double v) { // Ra + Rb
//     double total = 0;
//     double valAdd = (0.5*(v*v)*AIR_DENS);
//     for(int SCL = 0; SCL< 5; SCL++){
//         total += (0.5*(v*v)*AIR_DENS*SCLValues[SCL]);
//     }
//     //total += ((SCLValues[0]*valAdd) + (SCLValues[1]* valAdd) + (SCLValues[2]* valAdd) + (SCLValues[3] * valAdd)+(valAdd*SCLValues[4])); // calc downforce
//     total += g*MASS; // add weight
//     return total;
// }

// void compareResForceCalc(double velocity) {
//     clock_t begin = clock();
//     printf("%lf", calculateLiftForcesRaRbAdd(velocity) );
//     clock_t end = clock();
//     double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
//     printf("Elapsed: %lf seconds\n", time_spent);
//     clock_t begin2 = clock();
//     printf("%lf", calculateLiftForcesRaRbFor(velocity) );
//     clock_t end2 = clock();
//     time_spent = (double)(end2 - begin2) / CLOCKS_PER_SEC;
//     printf("Elapsed: %lf seconds\n", time_spent);
// }