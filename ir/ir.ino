#define FASTADC 1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup () {
  Serial.begin (115200);
  Serial.setTimeout (100);

  #if FASTADC
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  #endif
}

//long start, finish;
//int samples = 2000;
//int to_show = 0;

int *durations = malloc (50 * sizeof(int));
boolean is_read = false, previous;
int precision = 300;
long start, index;

void loop () {
  long now = micros ();
  boolean signal = analogRead (A0) >= 200;
  
  if (!signal && !is_read) {
    //Serial.println ("Transmittion is started");
    previous = signal;
    is_read = true;
    start = now;
    index = 0;
  }
  
  if (is_read && previous != signal) {
    durations [index++] = now - start;
    previous = signal;
    start = now;
  }

  if (is_read && now - start > 10000) {
    //Serial.println ("Transmittion is finished");
    is_read = false;

    for (int i = 0; i < index; i++) {
      //Serial.print (durations [i]);
      //Serial.print (" ");
      if (i > 0 && i % 8 == 0) {
          //Serial.print ("| ");
      }
    }
    //Serial.println ();
    
    for (int i = 0; i < index; i++) {
      //Serial.print (aligned_duration (durations [i]));
      //Serial.print (" ");
      if (i > 0 && i % 8 == 0) {
          //Serial.print ("| ");
      }
    }
    //Serial.println ();

    char *buffer = malloc ((index / 8) * sizeof (char) + 1);
    buffer [(index / 8) * sizeof (char)] = 0; // Stop bit
    for (int b = 0; b < index / 8; b++) {
      char c = 0;
      for (int i = 0; i < 8; i++) {
        c = (c << 1) | ((aligned_duration (durations [b * 8 + i]) / precision - 1) & 1);
      }

      //Serial.print ((int) c);
      //Serial.print (" ");
      buffer [b] = c;
    }
    //Serial.println ();

    Serial.println (String (buffer));
    free (buffer);
  }
  
  /*
  if (signal < 100 && to_show == 0) {
    Serial.println ("Transmittion is detected");
    start = micros ();
    to_show = samples;
  }

  if (to_show > 0) {
    to_show -= 1;

    if (to_show == 0) {
      finish = micros ();
      Serial.print (finish - start);
      Serial.print (" mus (");
      Serial.print ((finish - start) / samples);
      Serial.println (" mus per read)");
    }
  }
  */
}

long aligned_duration (long duration) {
  return (duration / precision + (duration % precision >= precision / 2 ? 1 : 0)) * precision;
}
