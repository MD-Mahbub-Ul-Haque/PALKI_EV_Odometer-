

int Htime;              //integer for storing high time

int Ltime;                //integer for storing low time

float Ttime;            // integer for storing total time of a cycle

float frequency;        //storing frequency


void setup()

{

    Serial.begin(9600);
    pinMode(8,INPUT);

    

}

void loop()

{
    Serial.print("Frequency of signal : ");


    Htime=pulseIn(8,HIGH);      //read high time

    Ltime=pulseIn(8,LOW);        //read low time

    

    Ttime = Htime+Ltime;

     Serial.print(Ttime);
     Serial.println(" time");


    frequency=1000000/Ttime;    //getting frequency with Ttime is in Micro seconds

     Serial.print(frequency);
     Serial.println(" Hz");
    

    delay(500);

}
