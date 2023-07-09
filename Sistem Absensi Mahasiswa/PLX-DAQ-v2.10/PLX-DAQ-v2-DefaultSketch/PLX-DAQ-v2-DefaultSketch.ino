/*
 * very basic sketch for PLX DAQ test
 * for new "Version 2" of PLX DAQ
 */

int i = 0;

void setup() {

  // open serial connection
    Serial.begin(9600);

    //Serial.println("CLEARDATA"); // clears starting at row 2
    Serial.println("CLEARSHEET"); // clears starting at row 1
    
  // define 5 columns named "Date", "Time", "Timer", "Counter" and "millis"
    Serial.println("LABEL,Date,Time,Timer,Counter,millis");

  // set the names for the 3 checkboxes
    Serial.println("CUSTOMBOX1,LABEL,Stop logging at 250?");
    Serial.println("CUSTOMBOX2,LABEL,Resume log at 350?");
    Serial.println("CUSTOMBOX3,LABEL,Quit at 450?");

  // check 2 of the 3 checkboxes (first two to true, third to false)
    Serial.println("CUSTOMBOX1,SET,1");
    Serial.println("CUSTOMBOX2,SET,1");
    Serial.println("CUSTOMBOX3,SET,0");
}

void loop() {

    // simple print out of number and millis
    // output "DATA,DATE,TIME,TIMER,4711,13374"
      Serial.print("DATA,DATE,TIME,TIMER,");
      Serial.print(i++); Serial.print(",");
      Serial.println(millis());

    // clear some cells in Excel (rectangle range from B10 to D20)
      if(i==100)
        Serial.println("ClearRange,B,10,D,20");

    // do a simple beep in Excel on PC and read value from named sheet
      if(i==150)
      {
        Serial.println("BEEP");
        //Serial.println("CELL,GET,E4"); ==> active sheet in Excel
        Serial.println("CELL,GET,FROMSHEET,Simple Data,E,4"); // ==> named sheet
        int readvalue = Serial.parseInt();
        Serial.print("Value of cell E4 is: "); // result displayed in Excel DirectDebugWindow to double check
        Serial.println(readvalue);
      }

    // check value of custombox1 on PLX DAQ in Excel and if
    // checkbox is checked then send the command to pause logging
      if(i==250)
      {
        Serial.println("CUSTOMBOX1,GET");
        int stoplogging = Serial.parseInt();
        // this information can be seen in the 
        // direct debug window on PLX DAQ in Excel
        Serial.print("Value of stoplogging/checkbox is: ");
        Serial.println(stoplogging);
        if(stoplogging) {
          Serial.println("PAUSELOGGING");
        }
      }

    // same thing for resume logging
      if(i==350)
      {
        Serial.println("CUSTOMBOX2,GET");
        int resumelogging = Serial.parseInt();
        if(resumelogging) {
          Serial.println("RESUMELOGGING");
        }
      }   

    // post to specific cells on defaul sheet and named sheet
      if(i==400)
      {
        Serial.println("CELL,SET,G10,400 test 1 string"); // default sheet active in PLX DAQ Excel
        Serial.println("CELL,SET,ONSHEET,Simple Data,G,11,400 test 2 string"); // named sheet available in PLX DAQ Excel
      }
       
    // and for forced quit of Excel with saving the file first
      if(i==450)
      {
        Serial.println("CUSTOMBOX3,GET");
        if(Serial.parseInt()) {
          Serial.println("SAVEWORKBOOKAS,450-Lines-File");
          Serial.println("FORCEEXCELQUIT");
        }
      }
}
