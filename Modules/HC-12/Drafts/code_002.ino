The simple system in Example 2 will work well with a sympathetic human who does not try to mess it up. But if the computer or person sending the data cannot know when the Arduino is ready to receive there is a real risk that the Arduino will not know where the data starts.
If you would like to explore this, change the end marker in the previous program from '\n' to '>' so that you can include the end marker in your text for illustration purposes. (You can't manually enter a Newline character in the text you are sending from the Serial Monitor). And put the line ending back to "No line ending"
Now, with the revised code send "qwert>" and you will see that it behaves exactly the same as when you were using Newline as the end marker.
But if you try this "asdfg>zxcvb" you will only see the first part "asdfg". And then if you send "qwert>" you will see "zxcvbqwert" because the Arduino has become confused and cannot know that it should have ignored "zxcvb".
The answer to this problem is to include a start-marker as well as an end-marker.


// Example 3 - Receive with start- and end-markers

const byte numChars = 32;
char receivedChars[numChars];

boolean newData = false;

void setup() {
    Serial.begin(9600);
    Serial.println("<Arduino is ready>");
}

void loop() {
    recvWithStartEndMarkers();
    showNewData();
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
}
