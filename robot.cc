/**
 * Nimbus robot - XBee controlled
 * 
 * Accepts input commands in the form:
 *   
 *   CMD\n
 *   CMD ARG1 ARG2 ...\n
 *
 * Available commands:
 * 
 *   STOP
 *   SPEED L R
 *   COLOR R G B
 */

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

// Input buffer
char buf[BUFSIZE];
char c;
unsigned int buf_pos = 0;

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
    
    t = (100 * (bat - BAT_LOW)) / (BAT_FULL - BAT_LOW);
    
    Serial.print("\tBat: ");
    Serial.print(t);
    Serial.print("%");
    
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
 * Parse command
 * 
 * Accepts input commands in the form:
 *   
 *   CMD\n
 *   CMD ARG1 ARG2 ...\n
 *
 * Available commands:
 * 
 *   STOP
 *   SPEED L R
 *   COLOR R G B
 *   STATUS
 */
void command_parse()
{
    char *cmd, *str, *args[CMD_MAX_ARGS];
    unsigned int i, n_args = 0;
    
    //Serial.print("Got command string: ");
    //Serial.println(buf);
    
    cmd = strtok(buf, " ");
    //Serial.print("Command: ");
    //Serial.println(cmd);
    
    for (i = 0; i < CMD_MAX_ARGS && NULL != (str = strtok(NULL, " ")); i++) {
        args[i] = str;
    }
    
    n_args = i;
    /*
    Serial.print("Arguments: ");
    Serial.println(n_args);
    
    for (i = 0; i < n_args; i++) {
        Serial.print("  ");
        Serial.println(args[i]);
    }
    */
    if (strcasecmp(cmd, "SPEED") == 0) {
        if (n_args < 2) {
            Serial.println("Usage: SPEED L R");
            return;
        }
        
        speed_l = atoi(args[0]);
        speed_r = atoi(args[1]);
    }
}

/**
 * Read command
 */
void command_read()
{
    if (Serial.available() > 0) {
        // Read the incoming byte
        buf[buf_pos] = Serial.read();
        
        if (buf[buf_pos] == '\n') {
            // End of command
            buf[buf_pos] = '\0';
            buf_pos = 0;
            
            // Parse it
            command_parse();
            
        } else {
            buf_pos++;
        }
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
    //battery_check();
    
    // Read sensors
    sharp_front = analogReadMedian(SHARP_FRONT_PIN, SHARP_SAMPLES);
    sharp_left = analogReadMedian(SHARP_LEFT_PIN, SHARP_SAMPLES);
    sharp_right = analogReadMedian(SHARP_RIGHT_PIN, SHARP_SAMPLES);
    
    // Read incoming command
    command_read();
    
    // Set speed
    setSpeed(speed_l, speed_r);
    
    //debug();
    
    delay(10);
}

