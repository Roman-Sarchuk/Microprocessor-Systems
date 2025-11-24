const int numeral[10] = {
  //ABCDEFGH
  B11111100,
  B01100000,
  B11011010,
  B11110010,
  B01100110,
  B10110110,
  B00111110,
  B11100000,
  B11111110,
  B11100110,
};

const int nbrSegments= 8;
const int nbrDigits= 4;

//	                                   H,G,F,E,D,C,B,A
const int segmentPins[nbrSegments] = {13,8,7,6,5,4,3,2}; 

//digital	                        0 1 2 3
const int digitPins[nbrDigits] = {9,10,11,12};

// RV
const int potPin = A0;


void setup()
{
  for(int i=0; i < 8; i++) { 
    pinMode(segmentPins[i], OUTPUT);
  }
  for(int i=0; i < nbrDigits; i++) { 
    pinMode(digitPins[i], OUTPUT);
  }
}


void loop()
{
  //int value = analogRead(0); 
  int value = analogRead(potPin); 
  showNumber(value);
}


void showNumber( int number)
{
  if(number == 0) {
    showDigit( 0, nbrDigits-1);
  }
  else {
    for( int digit = nbrDigits-1; digit >= 0; digit--){ 
      if(number > 0){
        showDigit( number % 10, digit) ; 
        number = number / 10;
      }
    }
  }
}


void showDigit( int number, int digit)
{
  digitalWrite( digitPins[digit], HIGH );
  for(int segment = 1; segment < nbrSegments; segment++) {
    boolean isBitSet = bitRead(numeral[number], segment); 
    digitalWrite( segmentPins[segment], isBitSet);
  }
  delay(5);
  digitalWrite( digitPins[digit], LOW );
}
