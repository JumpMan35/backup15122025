
//LCD

void InitInstructionLCD(){
  Serial3.write(0xFE);
}

static void MoveXCursor(unsigned char position , char CursorLine ){
  InitInstructionLCD();  
  Serial3.write(0x80 + CursorLine * 64  + position);
}

static void PrintWord(char *MyWord,unsigned char position, char CursorLine ){
 MoveXCursor(position,CursorLine);
 Serial3.print(MyWord);
}

static void PrintDegre(unsigned char position, char CursorLine  ){
 MoveXCursor(position,CursorLine );
 Serial3.print(0,HEX);
}



void ClearScreen(){
  InitInstructionLCD();  
  Serial3.write(0x01);
  
  
}

void TurnOffScreen(){
  InitInstructionLCD();  
  Serial3.write(0x08);
}

void TurnOnScreen(){
  InitInstructionLCD();  
  Serial3.write(0x0C);
}


void BlinkCursor(char position, char CursorLine){
  InitInstructionLCD();  
  Serial3.write(0x0D);
  MoveXCursor(position, CursorLine);
}
void MoveCursorLeft(){
  InitInstructionLCD();  
  Serial3.write(0x10);
}

void BlinkOff(){
  InitInstructionLCD(); 
  Serial3.write(0x0C);
}

void BlinkOn(){
  InitInstructionLCD(); 
  Serial3.write(0x0D);
}

void DisplayCursor(unsigned char position, char CursorLine){
  char tab[]= "->";
  PrintWord(tab,position , CursorLine );
  
}

void Custom_UpAndDown(unsigned char position,unsigned char line){
    //ClearScreen();
    InitInstructionLCD();
    Serial3.write(0x40);

    Serial3.write(0x04);
    delay(1);
    Serial3.write(0x0e);
    delay(1);
    Serial3.write(0x1f);
    delay(1);
    Serial3.write(0x00);
    delay(1);
    Serial3.write(0x1f);
    delay(1);
    Serial3.write(0x0e);
    delay(1);
    Serial3.write(0x04);
    delay(1);
    Serial3.write(0x00);
    delay(1);

  
  InitInstructionLCD();
    
  Serial3.write(0x80 + position + 64* line);
  Serial3.write(0x00);
}

void Custom_DegreC(unsigned char position,unsigned char line){
    //ClearScreen();
    InitInstructionLCD();
    Serial3.write(0x40 +1);

    Serial3.write(0x18);
    delay(1);
    Serial3.write(0x18);
    delay(1);
    Serial3.write(0x03);
    delay(1);
    Serial3.write(0x04);
    delay(1);
    Serial3.write(0x04);
    delay(1);
    Serial3.write(0x04);
    delay(1);
    Serial3.write(0x03);
    delay(1);
    Serial3.write(0x00);
    delay(1);

  
  InitInstructionLCD();  
  Serial3.write(0x80 + position + 64* line);
  Serial3.write(0x01);
  
}

void PartialLCDRefresh(char* chaine, char posXline, unsigned char Line) {

  BlinkOff();
  PrintWord(chaine,posXline,Line);
  BlinkOn();
  Serial.print("\n ------------------Partial Refresh --------------- \n");
  Serial.print(chaine);
  Serial.print("\t");
  Serial.print(posXline,DEC);
  Serial.print("\n ----------------FIN Partial Refresh--------------- \n");
}


void DisplayLCDMenu(Menu_t*pMenu, char ItemToDisplay, char CursorLine) {

  char ItemToDisplayFirstLine  = ItemToDisplay;
  char ItemToDisplaySecondLine = ItemToDisplay+1;  
  if(ItemToDisplay >= pMenu->NbItemsMenu-1)
  {
    ItemToDisplaySecondLine =0;
  }

  ClearScreen();
  PrintWord(pMenu->ItemsMenu[ItemToDisplayFirstLine],0,0);
  PrintWord(pMenu->ItemsMenu[ItemToDisplaySecondLine],0,1);
  Serial.print("\n ------------------DisplayLCDMenu --------------- \n");
  Serial.print(pMenu->ItemsMenu[ItemToDisplayFirstLine]);
  Serial.print(pMenu->ItemsMenu[ItemToDisplaySecondLine]);
  Serial.print("\n ----------------FIN DisplayLCDMenu --------------- \n");

  //Serial.print(pMenu->Position[ItemToDisplayFirstLine]);
  if( pMenu->LevelMenu == 0 || pMenu->LevelMenu == 20  )
    DisplayCursor(0,0);    
}
