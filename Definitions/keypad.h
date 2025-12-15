
volatile float voltage;
volatile int adc_key_in;
enum { NUM_KEYS = 5 };
int  adc_key_val[NUM_KEYS]={0, 147, 308, 495, 787 };
volatile int keypad;
char Bouton_val[NUM_KEYS] = {'L', 'H', 'B', 'R', 'V'};
volatile unsigned long interruptLockout = 50; // Délai de débounce en millisecondes



int get_key(int input)
{
  int k;  
  for (k = 0; k < NUM_KEYS ; k++)
  {
    if (input > adc_key_val[k]-20 &&  input < adc_key_val[k]+20)
        return k;      
  }
  if (k >= NUM_KEYS)
    k = -1;     // No valid key pressed 
  return k;
}