#include <WProgram.h>

#include "util.h"
#include "robot.h"

int i;
int speed;
int turn = 0;
int back = 0;

// Battery level
int bat;

// Current wheel speeds
int speed_l, speed_r;

// Sensor readings
int sharp_front, sharp_left, sharp_right;

// Last dominant reading
Side sharp_last = NONE;

// Robot state
State state = SEARCH;

// Timer keeping track of how long we've been in LOST state
unsigned long lost_time;


/**
 * Set wheel speeds
 */
void setSpeed(int left, int right) {
    digitalWrite(LEFT_DIR, (left < 0) ? BACKWARD : FORWARD);
    digitalWrite(RIGHT_DIR, (right < 0) ? BACKWARD : FORWARD);
    
    analogWrite(LEFT_PWM, (unsigned) abs(left));
    analogWrite(RIGHT_PWM, (unsigned) abs(right));
}

/**
 * Set LED color
 */
void setColor(int r, int g, int b) {
    analogWrite(LED_RED_PIN, r);
    analogWrite(LED_GREEN_PIN, g);
    analogWrite(LED_BLUE_PIN, b);
}

/**
 * Serial debug info
 */
void debug()
{
    unsigned long t;
    
    switch(state) {
    case SEARCH:
        Serial.print("SEARCH");
        break;
    case FOLLOW:
        Serial.print("FOLLOW");
        break;
    case FOLLOW_L:
        Serial.print("FOLLOW_L");
        break;
    case FOLLOW_R:
        Serial.print("FOLLOW_R");
        break;
    case LOST:
        Serial.print("LOST");
        t = millis();
        Serial.print(" (");
        Serial.print(t - lost_time);
        Serial.print("ms)");
        break;
    case LOST_L:
        Serial.print("LOST_L");
        break;
    case LOST_R:
        Serial.print("LOST_R");
        break;
    }
    
    Serial.print("\tSens:");
    Serial.print("\tL: ");
    Serial.print(sharp_left);
    Serial.print("\tF: ");
    Serial.print(sharp_front);
    Serial.print("\tR: ");
    Serial.print(sharp_right);
    
    Serial.print("\tSpeed:");
    Serial.print("\tL: ");
    Serial.print(speed_l);
    Serial.print("\tR: ");
    Serial.print(speed_r);
    
    Serial.print("\tBat:");
    Serial.print(bat);
    
    Serial.println();
}

/**
 * Check voltage of battery
 */
void battery_check() {
    bat = analogRead(BAT_PIN);
    
    if (bat <= BAT_LOW) {
        // Low battery, stop motors and signal with LED
        setSpeed(0, 0);
        
        while (1) {
            Serial.println("LOW BAT!");
            setColor(255, 0, 0);
            delay(500);
            setColor(0, 0, 0);
            delay(500);
        }
    }
}


/**
 * Search mode
 */
void search() {
    /*if (speed_l == 0 && speed_r == 0) {
        speed_l = SPEED_L;
        speed_r = SPEED_R;
    }
    
    speed_l += random(-5, 6);
    speed_r += random(-5, 6);
    
    speed_l = constrain(speed_l, -SPEED_L, SPEED_L);
    speed_r = constrain(speed_r, -SPEED_R, SPEED_R);
    
    Serial.print(speed_l);
    Serial.print(' ');
    Serial.print(speed_r);
    Serial.println();
    
    setSpeed(speed_l, speed_r);*/
    
    setColor(255, 255, 0);

    // Look for any objects
    if (sharp_front > SHARP_THRESH || sharp_left > SHARP_THRESH || sharp_right > SHARP_THRESH) {
        state = FOLLOW;
    } else {
        // No objects found, continue search
        speed_l = SPEED_L;
        speed_r = SPEED_R;
    }
}

/**
 * In the follow wall state
 */
void follow() {

    if (sharp_front > SHARP_THRESH) {
        // Object detected in front
        setColor(255, 255, 255);
        
        speed = map(sharp_front, SHARP_THRESH, SHARP_MAX, 150, 500);
        
        if (sharp_right > sharp_left) {
            // turn left
            speed_l = SPEED_L - speed;
            speed_r = SPEED_R;
        } else {
            // turn right
            speed_l = SPEED_L;
            speed_r = SPEED_R - speed;
        }
        
        sharp_last = FRONT;
        
    } else if (sharp_right > SHARP_THRESH || sharp_left > SHARP_THRESH) {
        if (sharp_right > sharp_left) {
            // Object detected to the right
            setColor(0, 255, 0);
            
            if (sharp_right < SHARP_FOLLOW) {
                // Turn towards object
                speed_l = SPEED_L;
                speed_r = map(sharp_right, SHARP_THRESH, SHARP_FOLLOW, 100, SPEED_R);
            } else {
                // Turn away from object
                speed_l = map(sharp_right, SHARP_FOLLOW, SHARP_MAX, SPEED_L, 100);
                speed_r = SPEED_R;
            }
            
            sharp_last = RIGHT;
            
        } else {
            // Object detected to the left
            setColor(255, 0, 0);
            
            if (sharp_left < SHARP_FOLLOW) {
                // Turn towards object
                speed_l = map(sharp_left, SHARP_THRESH, SHARP_FOLLOW, 100, SPEED_L);
                speed_r = SPEED_R;
            } else {
                // Turn away from object
                speed_l = SPEED_L;
                speed_r = map(sharp_left, SHARP_FOLLOW, SHARP_MAX, SPEED_R, 100);
            }
            
            sharp_last = LEFT;
        }
        
    } else {
        state = LOST;
        lost_time = millis();
    }
}

/**
 * Object lost
 */
void lost()
{
    unsigned long t;
    
    if (sharp_front > SHARP_THRESH || sharp_left > SHARP_THRESH || sharp_right > SHARP_THRESH) {
        state = FOLLOW;
        return;
    }
    
    // Check for timeout
    t = millis();
    if (t - lost_time > LOST_TIMEOUT) {
        state = SEARCH;
        return;
    }
    
    if (sharp_last == RIGHT) {
        // Last seen to the right => turn right
        setColor(0, 255, 255);
        
        speed_l = SPEED_L;
        speed_r = 60;
        
    } else if (sharp_last == LEFT) {
        // Last seen to the left => turn left
        setColor(255, 0, 255);
        
        speed_l = 60;
        speed_r = SPEED_R;
    
    } else {
        // Start searching...
        state = SEARCH;
    }
}

void setup() {
    Serial.begin(9600);
    
    pinMode(RIGHT_PWM, OUTPUT);  //Set control pins to be outputs
    pinMode(RIGHT_DIR, OUTPUT);
    pinMode(LEFT_PWM, OUTPUT);
    pinMode(LEFT_DIR, OUTPUT);
    
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_BLUE_PIN, OUTPUT);
    
    // Set speeds
    speed_l = 0;
    speed_r = 0;
    
    setSpeed(0, 0);
    
    // Initialize state
    state = SEARCH;
}

void loop() {
    // Check battery
    battery_check();
    
    // Read sensors
    sharp_front = analogReadMedian(SHARP_FRONT_PIN, SHARP_SAMPLES);
    sharp_left = analogReadMedian(SHARP_LEFT_PIN, SHARP_SAMPLES);
    sharp_right = analogReadMedian(SHARP_RIGHT_PIN, SHARP_SAMPLES);
    
    switch (state) {
    case FOLLOW:
        follow();
        break;
    case LOST:
        lost();
        break;
    default:
        search();
        break;
    }
    
    setSpeed(speed_l, speed_r);
    
    debug();
    
    delay(10);
}

