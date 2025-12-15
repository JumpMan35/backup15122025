

#include <HardwareSerial.h>
#include "Definitions/Hardware.h"
#include "Definitions/Structures.h"
#include "Definitions/Probes.h"
#include "Definitions/Menus.h"
#include "Definitions/LCD.h"
#include "Definitions/keypad.h"
#include "RTClib.h"


RTC_DS1307 rtc;

byte          TOTAL_PROBES = 4; // Nombre total de sonde de mesure de température
char          CursorLine;
char          ItemsToDisplay;
char          CurrentVersion[] = "version 1.0";
bool          EditingItems =  false;
bool          ValidationOngoing = false;  
DateTime      now;

bool ValidationChoice = true; // true = YES  by default

Menu_t*       pGlobalMenu;
unsigned int  CurrentIndexMenu;
unsigned char StoreCurrentValue;
unsigned char StoreItemsToDisplay;
unsigned char StoreCurrentIndex;
byte          CurrentValueProbeEditing;

//bool          val_ECHKM2= true; // remove ???
bool          val_ECC   = false;
bool          val_SCORF = true;
bool          val_SODC  = false;

StructProbe*  pSondes;

// proposition le chat

bool isEditingDateTime = false;
byte editPosition = 0; // Position du curseur pour l'édition
byte tempDay, tempMonth, tempYear, tempHour, tempMinute, tempSecond;

//unsigned long lastDebounceDelay = 0;

//unsigned long debounceDelay = 50; // Délai de débounce en millisecondes

//volatile bool isHandlingInterrupt = false; // Variable pour bloquer les autres interruptions

// Fonction à mettre dans test_backup2.ino ou dans une source adaptée
void createDynamicMenu(Menu_t * PointeurMenu, unsigned char IndexTabMenu, char** ListMenu, int LevelMenu, char NbItemsMenu)
{
    // Alloue le tableau de pointeurs pour les sous-items dynamiques par ligne
    PointeurMenu->DynValuesDisplayed = (SubItemDynMenu_t**) malloc(NbItemsMenu * sizeof(SubItemDynMenu_t*));
    // Alloue le tableau de pointeurs pour les lignes de chaînes du menu
    PointeurMenu->ItemsMenu = (char**)malloc(NbItemsMenu * sizeof(char*));

    // Buffer temporaire pour stocker les sous-items dynamiques avant copie
    SubItemDynMenu_t** localDynMenu = (SubItemDynMenu_t**) malloc(NbItemsMenu * sizeof(SubItemDynMenu_t*));

    for(int i = 0; i < NbItemsMenu; i++) {
        // Allocation pour la string de la ligne du menu (+1 pour le \0)
        PointeurMenu->ItemsMenu[i] = (char*)malloc(strlen(ListMenu[i]) + 1);

        // On recopie la ligne de menu dans ItemsMenu
        strcpy(PointeurMenu->ItemsMenu[i], ListMenu[i]);

        // PRECOMPTE des groupes de # sur la ligne i
        unsigned char count = 0, in_group = 0;
        for (unsigned int cp = 0; cp < strlen(ListMenu[i]); cp++) {
            if(ListMenu[i][cp] == '#') {
                if(!in_group) { in_group = 1; count++; }
            } else {
                if(in_group) in_group = 0;
            }
        }

        localDynMenu[i] = NULL;
        if(count > 0) {
            localDynMenu[i] = (SubItemDynMenu_t*) malloc(count * sizeof(SubItemDynMenu_t));
        }

        // Extraction des sous-items dynamiques
        unsigned char dyn_idx = 0;
        in_group = 0;
        unsigned char cur_len = 0;
        unsigned char cur_pos = 0;
        for (unsigned int cp = 0; cp < strlen(ListMenu[i]); cp++) {
            if(ListMenu[i][cp] == '#') {
                if(!in_group) {
                    in_group = 1;
                    cur_pos = cp;
                    cur_len = 1;
                } else {
                    cur_len++;
                }
            } else {
                if(in_group) {
                    // Fin d'un sous-item dynamique, on l'enregistre
                    if(dyn_idx < count) {
                        localDynMenu[i][dyn_idx].posSubItemsMenu    = cur_pos;
                        localDynMenu[i][dyn_idx].LenghtSubItemsMenu = cur_len;
                        localDynMenu[i][dyn_idx].NbSubItemDynMenu   = count;
                        localDynMenu[i][dyn_idx].Unity = 0;
                        localDynMenu[i][dyn_idx].PointerValue = NULL;
                    }
                    dyn_idx++;
                    in_group = 0;
                    cur_len = 0;
                }
            }
        }
        // Cas d'un sous-item dynamique qui finit la ligne
        if(in_group && dyn_idx < count) {
            localDynMenu[i][dyn_idx].posSubItemsMenu    = cur_pos;
            localDynMenu[i][dyn_idx].LenghtSubItemsMenu = cur_len;
            localDynMenu[i][dyn_idx].NbSubItemDynMenu   = count;
            localDynMenu[i][dyn_idx].Unity = 0;
            localDynMenu[i][dyn_idx].PointerValue = NULL;
            dyn_idx++;
        }

        // Allocation réelle pour la ligne i dans DynValuesDisplayed
        if(count > 0) {
            PointeurMenu->DynValuesDisplayed[i] = (SubItemDynMenu_t*)malloc(count * sizeof(SubItemDynMenu_t));
            for(unsigned char c = 0; c < count; ++c) {
                PointeurMenu->DynValuesDisplayed[i][c] = localDynMenu[i][c];
            }
        } else {
            PointeurMenu->DynValuesDisplayed[i] = NULL;
        }
        // Terminaison correcte de la chaîne menu (optionnel car strcpy le fait déjà)
        PointeurMenu->ItemsMenu[i][strlen(ListMenu[i])] = '\0';
    }

    // Finalisation du Menu
    PointeurMenu->LevelMenu   = LevelMenu;
    PointeurMenu->NbItemsMenu = NbItemsMenu;
    PointeurMenu->IndexTabMenu= IndexTabMenu;

    // Libération de la mémoire annexe temporaire
    for(int i = 0; i < NbItemsMenu; i++) {
        if(localDynMenu[i]) free(localDynMenu[i]);
    }
    free(localDynMenu);

    Serial.print("\n debut Debug \n");
Serial.print( "PointeurMenu nav up");
Serial.print("\t");
Serial.print( PointeurMenu->nav.up );
Serial.print("\n");
Serial.print( "PointeurMenu nav down"); 
Serial.print("\t");
Serial.print( PointeurMenu->nav.down );
Serial.print("\n");
Serial.print( "PointeurMenu nav left");
Serial.print("\t");
Serial.print( PointeurMenu->nav.left );
Serial.print("\n");
Serial.print( "PointeurMenu nav right");
Serial.print("\t");
Serial.print( PointeurMenu->nav.right );  
Serial.print("\n");
Serial.print( "PointeurMenu nav valid");
Serial.print("\t");
Serial.print( PointeurMenu->nav.valid );
Serial.print("\n");
Serial.print( "PointeurMenu nav exceptionFlag");
Serial.print("\t");
Serial.print( PointeurMenu->nav.exceptionFlag );
Serial.print("\n");
Serial.print("\n fin Debug \n");

    
}

//void createDynamicMenu(Menu_t * PointeurMenu, unsigned char IndexTabMenu, char** ListMenu, int LevelMenu, char NbItemsMenu)
//{
//  PointeurMenu->DynValuesDisplayed = NULL;
//  SubItemDynMenu_t** localDynMenu ;
//  unsigned char NbSubItemDynMenu = 0;
//  bool debut = false;
//
//  PointeurMenu->DynValuesDisplayed = (SubItemDynMenu_t**) malloc( NbItemsMenu * sizeof(SubItemDynMenu_t*));
//
//  localDynMenu = (SubItemDynMenu_t**) malloc( NbItemsMenu * sizeof(SubItemDynMenu_t*)); //////// attention detruire à la fin \\\\\\\\\\\\\\\\\\\

//  for(int i=0 ; i<NbItemsMenu; i++){
//    localDynMenu[i] = (SubItemDynMenu_t*) malloc( 4 * sizeof(SubItemDynMenu_t));  /// nb sub item par ligne de menu
//  }
//
//  PointeurMenu->ItemsMenu = (char **)malloc(NbItemsMenu*sizeof(char*));
//  for(int i=0 ; i<NbItemsMenu; i++){
//    PointeurMenu->ItemsMenu[i] = (char*)malloc(strlen(ListMenu[i])*sizeof(char)+1);
//
//    
//    NbSubItemDynMenu = 0;
//    debut = false;
//    localDynMenu[i][NbSubItemDynMenu].LenghtSubItemsMenu = 0;
//    localDynMenu[i][NbSubItemDynMenu].posSubItemsMenu = 0;
//    for(int cp=0;cp<strlen(ListMenu[i]);cp++)
//    {
//       PointeurMenu->ItemsMenu[i][cp]=ListMenu[i][cp];
//       if( ListMenu[i][cp] == '#'){
//         if(debut == false) {
//           debut = true;
//           localDynMenu[i][NbSubItemDynMenu].posSubItemsMenu = cp;
//           
//         }
//         //localDynMenu[i][NbSubItemDynMenu].SubItemsDynMenu[localDynMenu[i][NbSubItemDynMenu].LenghtSubItemsMenu] = ListMenu[i][cp];
//         localDynMenu[i][NbSubItemDynMenu].LenghtSubItemsMenu ++;
//        }  
//        else{ // fin  du #?
//          if ( debut == true){ // fin du mot ### ?     
//            //localDynMenu[i][NbSubItemDynMenu].SubItemsDynMenu[localDynMenu[i][NbSubItemDynMenu].LenghtSubItemsMenu]  = '\0';
//            NbSubItemDynMenu ++;
//            debut = false;
//            localDynMenu[i][NbSubItemDynMenu].LenghtSubItemsMenu = 0;
//            localDynMenu[i][NbSubItemDynMenu].posSubItemsMenu = 0;
//          }
//        }
//        
//    }
//
//    /// gère la recherche sur la fin de ligne
//    if ( debut == true){ // fin du mot ### ?     
//     //localDynMenu[i][NbSubItemDynMenu].SubItemsDynMenu[localDynMenu[i][NbSubItemDynMenu].LenghtSubItemsMenu]  = '\0';
//     NbSubItemDynMenu ++;     
//    }
//    
//    if(NbSubItemDynMenu > 0){  // recopie les SubItemDynMenu dans Pointeur menu    
//      /*
//      Serial.print(" Debug\n NbSubItemDynMenu\t LevelMenu \n");
//      Serial.print( NbSubItemDynMenu );
//      Serial.print("\t");
//      Serial.print( LevelMenu );
//      Serial.print("\n fin Debug \n");
//      */
//      PointeurMenu->DynValuesDisplayed[i] = (SubItemDynMenu_t*) malloc( NbSubItemDynMenu * sizeof(SubItemDynMenu_t));  /// nb sub item par ligne de menu
//      for(int c=0;c<NbSubItemDynMenu;c++){
//        //strcpy(PointeurMenu->DynValuesDisplayed[i][c].SubItemsDynMenu, localDynMenu[i][c].SubItemsDynMenu); 
//        PointeurMenu->DynValuesDisplayed[i][c].LenghtSubItemsMenu   = localDynMenu[i][c].LenghtSubItemsMenu;
//        PointeurMenu->DynValuesDisplayed[i][c].posSubItemsMenu      = localDynMenu[i][c].posSubItemsMenu;
//        PointeurMenu->DynValuesDisplayed[i][c].NbSubItemDynMenu     = NbSubItemDynMenu ;
//      }
//      
//    }
//    else {
//      PointeurMenu->DynValuesDisplayed[i] = NULL;
//    }
//    PointeurMenu->ItemsMenu[i][strlen(ListMenu[i])] ='\0';
//  }
//  PointeurMenu->LevelMenu = LevelMenu;
//  PointeurMenu->NbItemsMenu = NbItemsMenu;
//  PointeurMenu->IndexTabMenu = IndexTabMenu ;
//
//  for(int i=0 ; i<NbItemsMenu; i++){
//    delete[] localDynMenu[i];
//  }
//  delete[] localDynMenu;
//
//
//  
//}




void substituteChars(char source[], char destination[], byte startAt, byte length)
{
  for (int pos = 0; pos < length; pos++)
  {
    destination[startAt + pos] = source[pos]; 
  } 
}

void RefreshValueDynamicMenu200( unsigned char IndexTabMenu ){

    /*
    char strvalue [] = "    "; 
    substituteChars( 
    *(char **)pGlobalMenu[IndexTabMenu].DynValuesDisplayed[0][0].PointerValue ,
    pGlobalMenu[IndexTabMenu].ItemsMenu[0] , 
    pGlobalMenu[IndexTabMenu].DynValuesDisplayed[0][0].posSubItemsMenu , 
    pGlobalMenu[IndexTabMenu].DynValuesDisplayed[0][0].LenghtSubItemsMenu );
    */
    
    char strvalue [] = "   "; 
    Serial.print("\n TESTS ************************************************* \n");
    Serial.print( dtostrf( *(byte*)pGlobalMenu[IndexTabMenu].DynValuesDisplayed[0][0].PointerValue ,2,0,strvalue) );
    Serial.print("\n TESTS ************************************************* \n");
    
  for(byte i=0;i< pGlobalMenu[IndexTabMenu].NbItemsMenu ;i++  ){
    char strvalue [] = "  "; 
    pGlobalMenu[IndexTabMenu].DynValuesDisplayed[i][0].LenghtSubItemsMenu = strlen(strvalue);
    substituteChars( 
    itoa(*(byte*)pGlobalMenu[IndexTabMenu].DynValuesDisplayed[i][0].PointerValue,strvalue,10) ,
    pGlobalMenu[IndexTabMenu].ItemsMenu[i] , 
    pGlobalMenu[IndexTabMenu].DynValuesDisplayed[i][0].posSubItemsMenu , 
    pGlobalMenu[IndexTabMenu].DynValuesDisplayed[i][0].LenghtSubItemsMenu );
    
    Serial.print("\n TESTS ************************************************* \n");
    //Serial.print( dtostrf( *(byte*)pGlobalMenu[IndexTabMenu].DynValuesDisplayed[i][0].PointerValue ,2,0,strvalue) );
    Serial.print( itoa(*(byte*)pGlobalMenu[IndexTabMenu].DynValuesDisplayed[i][0].PointerValue,strvalue,10) );
    Serial.print("\n");
  }

}

/*
void RefreshValueDynamicMenu200( unsigned char IndexTabMenu ){
    
  char strvalue[]={"    "};

  pGlobalMenu[IndexTabMenu].DynValuesDisplayed[0][0].LenghtSubItemsMenu = strlen( dtostrf( IndexCorProbe[IndexTabMenu],2,0,strvalue));
  
  substituteChars( 
  dtostrf( IndexCorProbe[IndexTabMenu],2,0,strvalue) ,
  pGlobalMenu[IndexTabMenu].ItemsMenu[0] , 
  pGlobalMenu[IndexTabMenu].DynValuesDisplayed[0][0].posSubItemsMenu , 
  pGlobalMenu[IndexTabMenu].DynValuesDisplayed[0][0].LenghtSubItemsMenu );

  unsigned char tab[3];
  tab[0] = pSondes[IndexCorProbe[IndexTabMenu]].Threshold;
  tab[1] = pSondes[IndexCorProbe[IndexTabMenu]].DeltaOn;
  tab[2] = pSondes[IndexCorProbe[IndexTabMenu]].DeltaOff;

  for(char i=1; i< pGlobalMenu[IndexTabMenu].NbItemsMenu; i++)
  {
    char strvalue1[]={"   "};
    pGlobalMenu[IndexTabMenu].DynValuesDisplayed[i][0].LenghtSubItemsMenu = strlen( dtostrf( IndexCorProbe[IndexTabMenu],3,0,strvalue1));
    substituteChars(
    dtostrf(tab[i-1],3,0,strvalue1),
    pGlobalMenu[IndexTabMenu].ItemsMenu[i] , 
    pGlobalMenu[IndexTabMenu].DynValuesDisplayed[i][0].posSubItemsMenu , 
    pGlobalMenu[IndexTabMenu].DynValuesDisplayed[i][0].LenghtSubItemsMenu );
  }
  
  //substituteChars(dtostrf(pSondes[0].DeltaOn,3,0,strvalue),pGlobalMenu[IndexTabMenu].ItemsMenu[2] , pGlobalMenu[IndexTabMenu].DynValuesDisplayed[2][0].posSubItemsMenu , strlen(strvalue) );
  //substituteChars(dtostrf(pSondes[0].DeltaOff,3,0,strvalue),pGlobalMenu[IndexTabMenu].ItemsMenu[3] , pGlobalMenu[IndexTabMenu].DynValuesDisplayed[3][0].posSubItemsMenu , strlen(strvalue) );
  
}
*/

void RefreshDynamicMenu10(){
  
  char strvalue[]={"    "};

  for(unsigned char cp = 0; cp < pGlobalMenu[1].DynValuesDisplayed[0][0].NbSubItemDynMenu; cp ++)
  {
    if(pSondes[cp].CurrentTemperature>=0){
      dtostrf(pSondes[cp].CurrentTemperature,3,1,strvalue);
    }
    else
    {
      strncpy(strvalue,"    ",4);
    }
    substituteChars(strvalue,pGlobalMenu[1].ItemsMenu[0] , pGlobalMenu[1].DynValuesDisplayed[0][cp].posSubItemsMenu , strlen(strvalue) );
  //substituteChars(strvalueS2,pGlobalMenu[1].ItemsMenu[0] , pGlobalMenu[1].DynValuesDisplayed[0][1].posSubItemsMenu , strlen(pGlobalMenu[1].DynValuesDisplayed[0][1].SubItemsDynMenu));
  }

  

}

void AssignMenusToProbes()
{
  // byte i;
  // for( byte indexMenu= ConstSonde200_MENU ; indexMenu <=  ConstSonde203_MENU; indexMenu++ )
  // {
  //     i = indexMenu -3;  ////////// A améliorer
  //     pGlobalMenu[indexMenu].DynValuesDisplayed[0][0].PointerValue = (byte*) &pSondes[i].Number;
  //     pGlobalMenu[indexMenu].DynValuesDisplayed[1][0].PointerValue = (byte*) &pSondes[i].Threshold;
  //     pGlobalMenu[indexMenu].DynValuesDisplayed[2][0].PointerValue = (byte*) &pSondes[i].DeltaOn;
  //     pGlobalMenu[indexMenu].DynValuesDisplayed[3][0].PointerValue = (byte*) &pSondes[i].DeltaOff;      

  //     pSondes[i].DeltaOn    = 1;
  //     pSondes[i].DeltaOff   = 5;
  //     pSondes[i].Threshold  = 19;  
  //     pSondes[i].Number     = i+1;
  //     pSondes[i].ValuesFunctions.val_const = ListCTNfunction[i].val_const;
  //     pSondes[i].ValuesFunctions.val_coef  = ListCTNfunction[i].val_coef;
  //     pSondes[i].CurrentTemperature = 0;
  //     pSondes[i].Status = 0;


  //       /*
  //       Serial.print("\n");
  //       Serial.print(pSondes[i].DeltaOn);
  //       Serial.print("\t");
  //       Serial.print(pSondes[i].DeltaOff);
  //       Serial.print("\t");
  //       Serial.print(*(byte*)&pSondes[i].Threshold);
  //       Serial.print("\n");
  //       */
      
  //     RefreshValueDynamicMenu200(indexMenu);
  // }

  for( byte i= 0 ; i< GLOBAL_SONDE_MENU_SIZE ; i++ )
  {
    pGlobalMenu[pIndexGlobalSondeMenu[i]].DynValuesDisplayed[0][0].PointerValue = (byte*) &pSondes[i].Number;
    pGlobalMenu[pIndexGlobalSondeMenu[i]].DynValuesDisplayed[1][0].PointerValue = (byte*) &pSondes[i].Threshold;
    pGlobalMenu[pIndexGlobalSondeMenu[i]].DynValuesDisplayed[2][0].PointerValue = (byte*) &pSondes[i].DeltaOn;
    pGlobalMenu[pIndexGlobalSondeMenu[i]].DynValuesDisplayed[3][0].PointerValue = (byte*) &pSondes[i].DeltaOff;      

    pSondes[i].DeltaOn    = 1;
    pSondes[i].DeltaOff   = 5;
    pSondes[i].Threshold  = 19;  
    pSondes[i].Number     = i+1;
    pSondes[i].ValuesFunctions.val_const = ListCTNfunction[i].val_const;
    pSondes[i].ValuesFunctions.val_coef  = ListCTNfunction[i].val_coef;
    pSondes[i].CurrentTemperature = 0;
    pSondes[i].Status = 0;
    RefreshValueDynamicMenu200(pIndexGlobalSondeMenu[i]);

    
  }
    
}

void setup() {
  Serial.begin(19200); /////// TEST //////////////////////////////////////////////////////////////////////////// 9600 

  
  // Set RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(60);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    rtc.adjust(DateTime(2025, 12, 7, 11, 34, 0));
  }
  

  
  /* Menu principal */
  pGlobalMenu = NULL;
  pSondes = NULL;
  

  pGlobalMenu     = (Menu_t*) malloc(20 * sizeof (Menu_t)); // 20 menus pour le moment
  pSondes         = (StructProbe*) malloc( TOTAL_PROBES * sizeof (StructProbe));
  
  

  CurrentIndexMenu = ConstIndexMAIN_MENU;
  Serial.print(" INIT MENU+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  createDynamicMenu(&pGlobalMenu[ConstIndexMAIN_MENU],ConstIndexMAIN_MENU, (char**)MAIN_MENU_ITEMS, MAIN_MENU.LevelMenu, MAIN_MENU.NbItemsMenu);
  createDynamicMenu(&pGlobalMenu[ConstIndexMeasure10MENU],ConstIndexMeasure10MENU, (char**)Measure10_MENU_ITEMS, Measure10_MENU.LevelMenu, Measure10_MENU.NbItemsMenu);
  createDynamicMenu(&pGlobalMenu[ConstIndexSonde20_MENU],ConstIndexSonde20_MENU, (char**)Sonde20_MENU_ITEMS, Sonde20_MENU.LevelMenu, Sonde20_MENU.NbItemsMenu);
  createDynamicMenu(&pGlobalMenu[ConstSonde200_MENU],ConstSonde200_MENU, (char**)Sonde200_MENU_ITEMS, Sonde200_MENU.LevelMenu, Sonde200_MENU.NbItemsMenu);
  createDynamicMenu(&pGlobalMenu[ConstSonde201_MENU],ConstSonde201_MENU, (char**)Sonde200_MENU_ITEMS, Sonde201_MENU.LevelMenu, Sonde201_MENU.NbItemsMenu);
  createDynamicMenu(&pGlobalMenu[ConstSonde202_MENU],ConstSonde202_MENU, (char**)Sonde200_MENU_ITEMS, Sonde202_MENU.LevelMenu, Sonde202_MENU.NbItemsMenu);
  createDynamicMenu(&pGlobalMenu[ConstSonde203_MENU],ConstSonde203_MENU, (char**)Sonde200_MENU_ITEMS, Sonde203_MENU.LevelMenu, Sonde203_MENU.NbItemsMenu);
  createDynamicMenu(&pGlobalMenu[ConstIndexValidMENU],ConstIndexValidMENU, (char**)Validation_MENU_ITEMS, Val_1000_MENU.LevelMenu, Val_1000_MENU.NbItemsMenu);

  createDynamicMenu(&pGlobalMenu[ConstIndexDate40_MENU],ConstIndexDate40_MENU, (char**)Date40_MENU_ITEMS,Date40_MENU.LevelMenu, Date40_MENU.NbItemsMenu);

  AssignMenusToProbes();
  AssignDateTime();
  
  Serial3.begin(9600);
  ClearScreen();
  delay(1000);
  TurnOnScreen();
  //PrintWord("Opti. solaire",0);
  //PrintWord(CurrentVersion,64);
  //BlinkCursor();
  
  // Init Probe
  pinMode(SA1,INPUT);
  pinMode(SA2,INPUT);
  pinMode(ProbeSpare1,INPUT);
  pinMode(ProbeSpare2,INPUT);

//  // Init Logic Input
//  pinMode(ECHKM2,INPUT); // remove
//  pinMode(ECC,INPUT); // from TA maison contact sec

  // Init Output Relay
  pinMode(Relay_SCORF,OUTPUT);
  pinMode(Relay_SODC,OUTPUT);

  CursorLine = 0;
  ItemsToDisplay=0;
  CurrentIndexMenu=ConstIndexMAIN_MENU;
  DisplayLCDMenu(pGlobalMenu,ItemsToDisplay,CursorLine);
}

unsigned char checkValue(unsigned char currentvalue, unsigned char min_values, unsigned char max_values )
{
  unsigned char tmp = currentvalue ;
  if( currentvalue > max_values )
  {
    tmp = min_values + 1;
  }
  else 
  if(currentvalue < min_values )
  {
    tmp = max_values - 1;
  }
  return tmp;
}


void AssignDateTime(){
  now = rtc.now();

  Serial.print("assign Heure \n");
  //Serial.print((byte*)(pGlobalMenu[ConstIndexDate40_MENU].DynValuesDisplayed[ItemsToDisplay][0].PointerValue[0]));
  Serial.print(*(byte*)(pGlobalMenu[ConstIndexDate40_MENU].DynValuesDisplayed[ItemsToDisplay][0].PointerValue));
  Serial.print("assign Heure \n");
  
  
  //strcpy('ee', &pGlobalMenu[Date40_MENU_ITEMS],ConstIndexDate40_MENU. DynValuesDisplayed[0][0].PointerValue);
//  &pGlobalMenu[Date40_MENU_ITEMS],ConstIndexDate40_MENU. DynValuesDisplayed[1][0].PointerValue = (byte*) &now.month();
//  &pGlobalMenu[Date40_MENU_ITEMS],ConstIndexDate40_MENU. DynValuesDisplayed[2][0].PointerValue = (byte*) &now.year() - 2000;
//  &pGlobalMenu[Date40_MENU_ITEMS],ConstIndexDate40_MENU. DynValuesDisplayed[3][0].PointerValue = (byte*) &now.hour();
//  &pGlobalMenu[Date40_MENU_ITEMS],ConstIndexDate40_MENU. DynValuesDisplayed[4][0].PointerValue = (byte*) &now.minute();
//  &pGlobalMenu[Date40_MENU_ITEMS],ConstIndexDate40_MENU. DynValuesDisplayed[5][0].PointerValue = (byte*) &now.second();
  // tempDay    = now.day();
  // tempMonth  = now.month();
  // tempYear   = now.year() - 2000;
  // tempHour   = now.hour();
  // tempMinute = now.minute();
  // tempSecond = now.second();
}


void onEventButton_High(){

  
  Serial.print("onEventButton_High \n");
  Serial.print("ItemsToDisplay\t ");
  Serial.print(ItemsToDisplay,DEC);
  Serial.print("\n GlobalMenu[CurrentIndexMenu].LevelMen\t ");
  Serial.print(pGlobalMenu[CurrentIndexMenu].LevelMenu);
  Serial.print(" \n onEventButton_High \n");

     
    
  
  switch (pGlobalMenu[CurrentIndexMenu].LevelMenu ){
    case 200 ... 203:{
      Serial.print("case 200 à 203 \n");
      if ( ItemsToDisplay !=0 && EditingItems == true ){ // Titre non editable 
        byte value_tmp =0;
          
        value_tmp = * (byte*)pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].PointerValue ;
        value_tmp ++;
        if (ItemsToDisplay == 1)
        {        
           *(byte*)pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].PointerValue = checkValue (value_tmp, 0, 90);
        }
        else
        if ( ItemsToDisplay == 2 || ItemsToDisplay == 3 )
        {
          *(byte*)pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].PointerValue = checkValue (value_tmp, 0, 9);
          
        }
        
        char strvalue1[]={"   "};
        PartialLCDRefresh(itoa(*(byte*)pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].PointerValue,strvalue1,10), 
        pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].posSubItemsMenu, 0);
        Serial.print("\n PartialLCDRefresh \n");
        Serial.print(strvalue1);
        Serial.print("\n");
        RefreshValueDynamicMenu200(CurrentIndexMenu);

      }
      else
      {

        Serial.print("\n ItemsToDisplay \n");
        Serial.print(ItemsToDisplay,DEC);
        Serial.print("\n");

        
        ItemsToDisplay--;
        if(ItemsToDisplay < 0)
        {
          ItemsToDisplay = pGlobalMenu[CurrentIndexMenu].NbItemsMenu -1  ; 
        
        }
        Serial.print("\n after ItemsToDisplay \n");
        Serial.print(ItemsToDisplay,DEC);
        Serial.print("\n");
        
        DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
        //BlinkCursor(pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].posSubItemsMenu+ pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].LenghtSubItemsMenu, 0);
        //Custom_UpAndDown(pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].posSubItemsMenu+ pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].LenghtSubItemsMenu +2, 0);
      }
    }
    break; // gestion date et heure
    case 40:{
      Serial.print("case 40 HIGH --------------------- \n");
    }
    break;

    case 1000:
    break;

    default:
    {
      Serial.print("default HIGH --------------------- \n");
      ItemsToDisplay--;
      if(ItemsToDisplay < 0)
      {
        ItemsToDisplay = pGlobalMenu[CurrentIndexMenu].NbItemsMenu -1  ; 
        
      }
      DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);

    }
    break;
  }    

  
}


void onEventButton_Low(){
  Serial.print(" LOW LOW --------------------- \n");
  

  switch (pGlobalMenu[CurrentIndexMenu].LevelMenu ){

    case 200 ... 203:{
      Serial.print("case 200 à 203 \n");
      if ( ItemsToDisplay !=0 && EditingItems == true ){ // Titre non editable 
        byte value_tmp =0;
          
        value_tmp = * (byte*)pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].PointerValue ;
        value_tmp --;
        if (ItemsToDisplay == 1)
        {        
           *(byte*)pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].PointerValue = checkValue (value_tmp, 0, 90);
        }
        else
        if ( ItemsToDisplay == 2 || ItemsToDisplay == 3 )
        {
          *(byte*)pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].PointerValue = checkValue (value_tmp, 0, 10);
          
        }
        
        char strvalue1[]={"   "};
        PartialLCDRefresh(itoa(*(byte*)pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].PointerValue,strvalue1,10), 
        pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].posSubItemsMenu, 0);
        Serial.print("\n PartialLCDRefresh \n");
        Serial.print(strvalue1);
        Serial.print("\n");
        RefreshValueDynamicMenu200(CurrentIndexMenu);

      }
      else{
        
        ItemsToDisplay++;
        if(ItemsToDisplay > pGlobalMenu[CurrentIndexMenu].NbItemsMenu-1)
        {
          ItemsToDisplay =0; 
        }
        DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
      
      }
      break;
    
    case 1000:
    break;

    default:
    {
      ItemsToDisplay++;
      if(ItemsToDisplay > pGlobalMenu[CurrentIndexMenu].NbItemsMenu-1)
      {
        ItemsToDisplay =0; 
      }
      DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
    }      
    break;
    }
  } 

 
  
}

bool IsItInTable(char value, const char* t, unsigned char len){
    for(unsigned char i=0; i < len; ++i)
        if(value == t[i])
            return true;
    return false;
}


void onEventButton_Left() {
  ItemsToDisplay = 0;
  CursorLine = 0;
  Serial.print("---------------- LEFT \t"); 
  Serial.print("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<---------------\n");

  unsigned int level = pGlobalMenu[CurrentIndexMenu].LevelMenu;

  // 1. Cas "Validation" (on toggle OUI/NON au lieu de retour en arrière)
  if(level == 1000) {
    Serial.print("---------------- VALIDATION SELECT NO: case 1000 \t");
    ValidationChoice = !ValidationChoice;
    DisplayValidationProcess();
    return;
  }

  // 2. Si on est déjà dans un menu principal (autre que l'accueil), retour à MAIN_MENU
  if(IsItInTable(CurrentIndexMenu, pIndexGlobalMainMenu, GLOBAL_MAIN_MENU_SIZE)) {
    CurrentIndexMenu = ConstIndexMAIN_MENU;
    DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu], 0, CursorLine);
    Serial.print("---------------- LEFT retour au menu principal \n");
    return;
  }

  // 3. Autres cas classiques de navigation hiérarchique
  switch (level) {
    case LevelMenu_MAIN:
      // Si menu principal->"LEFT" retourne à la page de mesure
      CurrentIndexMenu = ConstIndexMeasure10MENU;
      DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu], 0, CursorLine);
      Serial.print("---------------- LEFT retour au menu de mesure \n");
      break;
    case LevelMenu_Measure10:
    case LevelMenu_Sonde20:
    case LevelMenu_DebugSystem30:
    case LevelMenu_Date40:
      CurrentIndexMenu = ConstIndexMAIN_MENU;
      DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu], 0, CursorLine);
      Serial.print("---------------- LEFT retour au menu principal \n");
      break;
    case LevelMenu_Sonde200:
    case LevelMenu_Sonde201:
    case LevelMenu_Sonde202:
    case LevelMenu_Sonde203:
      // gestion des sondes (retour à liste sondes si pas en édition)
      if (!EditingItems) {
        CurrentIndexMenu = ConstIndexSonde20_MENU;
        DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu], 0, CursorLine);
        Serial.print("---------------- LEFT retour à la liste des sondes \n");
      }
      break;
    default:
      Serial.print("---------------- LEFT - aucun comportement spécifique\n");
      break;
  }
}
//
//
//
//void onEventButton_Left(){
//  
//  // a verifier !!!!!!!!!!!
//  ItemsToDisplay =0;
//  CursorLine = 0;
//  Serial.print("---------------- LEFT \t"); 
//  Serial.print("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<---------------\n");
  // Gestion pour un retour au menu principal
//  if ( IsItInTable(pGlobalMenu[CurrentIndexMenu].LevelMenu, pIndexGlobalMainMenu, GLOBAL_MAIN_MENU_SIZE) == true )
//  {
//    CurrentIndexMenu = ConstIndexMAIN_MENU;
//    DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu], 0, CursorLine);
//  }

  
  
//  if (ItemsToDisplay < sizeof(pIndexGlobalMainMenu)) {
//      CursorLine = 0;
//      CurrentIndexMenu = ConstIndexMAIN_MENU;
//      DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu], 0, CursorLine);
//      ItemsToDisplay = 0;
//  }
//   
//  
//  switch (pGlobalMenu[CurrentIndexMenu].LevelMenu ){
//    case LevelMenu_MAIN:{ ///// BIZARRE à revoir voir à supprimer
//      CurrentIndexMenu = ConstIndexMeasure10MENU;
//      DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
//      Serial.print("---------------- LEFT  retour au menu de mesure \t");    
//    }
//    break;
//    case LevelMenu_Measure10:{
//      CurrentIndexMenu = ConstIndexMAIN_MENU;
//      DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
//      Serial.print("---------------- LEFT  retour au menu de menu principal \t");    
//    }
//    break;
//    case 20:{
//      CurrentIndexMenu = ConstIndexMAIN_MENU;
//      DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
//      Serial.print("---------------- LEFT  retour au menu de menu principal \t");    
//    }
//    break;
//    case 200 ... 203 :{ /// gestion des sondes
//      if (EditingItems == false)
//      {
//        CurrentIndexMenu = ConstIndexSonde20_MENU;
//        DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
//        Serial.print("---------------- LEFT  retour au menu de menu sonde \t");    
//      }
//    }
//    break;
//
//    case 30:{
//       CurrentIndexMenu = ConstIndexDebugSystem30_MENU;
//       Serial.print("---------------- LEFT  retour au menu de menu systeme \t");    
//      
//    }
//    break;
//
//    case 1000:{
//    /// menu validation
//    Serial.print("---------------- VALIDATION SELECT NO: case 1000 \t");    
//    ValidationChoice =! ValidationChoice ;
//    DisplayValidationProcess();
//    
//   } break;
//  }

    // case 1000:{ // a deplacer dans la gestion du bouton validation
    //     // cancel editing value & restore the value before the edition

    //     ItemsToDisplay = StoreItemsToDisplay ; // back to menu before validation 
    //     CurrentIndexMenu  = StoreCurrentIndex;
    //     Serial.print(StoreItemsToDisplay,DEC);
    //     if(pGlobalMenu[StoreCurrentIndex].LevelMenu == 200 || pGlobalMenu[StoreCurrentIndex].LevelMenu == 201 || pGlobalMenu[StoreCurrentIndex].LevelMenu== 202 || pGlobalMenu[StoreCurrentIndex].LevelMenu== 203   )
    //     {
    //       Serial.print("\nRestore good value \n");  
    //       *(byte*)pGlobalMenu[StoreCurrentIndex].DynValuesDisplayed[ItemsToDisplay][0].PointerValue = CurrentValueProbeEditing;
    //       RefreshValueDynamicMenu200(StoreCurrentIndex);
    //       ValidationOngoing = false ;
    //       EditingItems = false;
    //       BlinkOff();
        
    //     }
    //  DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,0); 
    // Serial.print("\n 1000 Cancel Edit & restore previous value \n");
    //}  
    //break;
    
  //}
  //isHandlingInterrupt = false; // Réactiver les interruptions
//  
//}


void onEventButton_Right(){

  Serial.print("---------------->>>>>>\n");
  
  switch (pGlobalMenu[CurrentIndexMenu].LevelMenu )
  {
    
  case LevelMenu_MAIN:{
      Serial.print("----------------  case 0 MAIN MENU >>>>>>\n");
      if (ItemsToDisplay < GLOBAL_MAIN_MENU_SIZE ) {
          CursorLine = 0;
          CurrentIndexMenu = pIndexGlobalMainMenu[ItemsToDisplay];
          DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu], 0, CursorLine);
          ItemsToDisplay = 0;
      }

//      switch (ItemsToDisplay) {
//        case 0:{
//          CursorLine = 0;
//          CurrentIndexMenu = ConstIndexMeasure10MENU;
//          //DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
//          Serial.print("----------------  MESURE case 0 >>>>>>\n");
//        }
//        break;
//        case 1:{
//          CursorLine        = 0;
//          CurrentIndexMenu  = ConstIndexSonde20_MENU;
//          ItemsToDisplay    = 0;
//          //DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
//          Serial.print("----------------  MESURE case 1 >>>>>>\n");       
//        }
//        break;
//        case 2:{
//          CursorLine        = 0;
//          CurrentIndexMenu  = ConstIndexDate40_MENU;
//          ItemsToDisplay    = 0;
//          //DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
//          Serial.print("----------------  MESURE case 2 >>>>>>\n");       
//        }
//        break;
//        default:{
//      }break;
//      
//      }        
    }
    break;
  case 10:{
      ItemsToDisplay =0;
      CurrentIndexMenu  = 0;
      DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
      Serial.print("----------------   MESURE  case 10 >>>>>>\n");
  }
  break;
  case 20:{
      Serial.print("----------------  case 20  SONDE >>>>>>\n");
      switch (ItemsToDisplay) {
        case 0:{
          CurrentIndexMenu  = 3;
          ItemsToDisplay    = 0;
          DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
          
        }
        break;
        case 1:{
          CurrentIndexMenu  = 4;
          ItemsToDisplay    = 0;
          DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
        }
        break;
        case 2:{
          CurrentIndexMenu  = 5;
          ItemsToDisplay    = 0;
          DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
        }
        break;
        case 3:{
          CurrentIndexMenu  = 6;
          ItemsToDisplay    = 0;
          DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
        }
        break;
      }
      
      
      //Serial.print("---------------- case 20 >>>>>>\n");    
          
  }
   break;
   case 1000:{
    /// menu validation
    Serial.print("---------------- VALIDATION SELECT NO: case 1000 >>>>>>\t");    
    ValidationChoice =! ValidationChoice ;
    DisplayValidationProcess();
    
   } break;
   
 case 40:{
    if (ItemsToDisplay != 0) // Titre non editable sur la ligne 0
    {
        if (EditingItems == false)
        { // start editing
            EditingItems = true;
            Serial.print("---------------- case 40 START EDITION DATE >>>>>>\n");
            editPosition = 2; // positionne sur la date ou l'heure
            BlinkCursor(editPosition, 0);
        }
        else
        { // deplace le curseur clignotant de manière circulaire sur la date ou heure
            char tmp_position = editPosition;
            tmp_position++;
            if (ItemsToDisplay==1){     // date
               switch (tmp_position ){
                case 4:{
                  editPosition=5;
                  Serial.print("---------------- case 40 START EDITION DATE  5                   >>>>>>\n");
                }
                break;
                case 7:{
                  editPosition=8;
                  Serial.print("---------------- case 40 START EDITION DATE  7                   >>>>>>\n");
                }
                break;
                case 12:{
                  editPosition=2;
                   Serial.print("---------------- case 40 START EDITION DATE  12 to 2                   >>>>>>\n");
                }
                break;
                default:{
                  editPosition = tmp_position ;
                   Serial.print("---------------- case 40 START EDITION DATE  DEFAULT             >>>>>>\n");
                }
                break;
               } 
              BlinkCursor(editPosition, 0);
            }
            if (ItemsToDisplay==2){ // heure
                Serial.print("---------------- case 40 START EDITION DATE >>>>>>\n");
                editPosition = 2; // positionne sur la date ou l'heure
                BlinkCursor(editPosition, 0);
            }
        }
     }
    }
    break;
  
 }

}



void onEventButton_Valid(){
  Serial.print("---- VALIDATION ---- \n");
  
  switch (pGlobalMenu[CurrentIndexMenu].LevelMenu ){ // Selection sur la sonde de température
    case 200 ... 203:{   
      Serial.print(pGlobalMenu[CurrentIndexMenu].LevelMenu,DEC);    
      Serial.print("\t---------------- RECORD case 200 >>>>>>\n");    
      if ( ItemsToDisplay !=0) // Titre non editable
      {
        if ( EditingItems == false){ // start editing
          EditingItems = true;
          StoreItemsToDisplay = ItemsToDisplay;
          StoreCurrentValue = *(byte*)pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].PointerValue;
          BlinkCursor(pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].posSubItemsMenu + 2 ,0 );
          // Store the current value
          CurrentValueProbeEditing = * (byte*)pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].PointerValue ;                
        }
        else // Record a proposal value ?
        {
          
          if (ValidationOngoing == false)
          {
            StoreCurrentIndex = CurrentIndexMenu ;
            ValidationOngoing = true;
            DisplayValidationProcess();
          }
          
        }
        
      }
      //DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
      Serial.print("---------------- case 200 >>>>>>\n");     
  }
   break;

  case 1000 :{
    Serial.print("\n ---------------- case 1000 >>>>>>\n");    
        
    if (ValidationOngoing == true  ) // YES selected
    {
      ItemsToDisplay = StoreItemsToDisplay ; // back to menu before validation 
      CurrentIndexMenu  = StoreCurrentIndex;
      
      if(ValidationChoice == true ){
          Serial.print("\n ---------------- VALIDATION DE LA SAISIE 2000 \n");    
          Serial.print(CurrentIndexMenu,DEC);
          Serial.print("\n ---------------- VALIDATION DE LA SAISIE 2000 \n"); 
          Serial.print(StoreItemsToDisplay,DEC);
          ItemsToDisplay = StoreItemsToDisplay ; // back to menu before validation 
          CurrentIndexMenu  = StoreCurrentIndex;
          Serial.print(StoreItemsToDisplay,DEC);
          if(pGlobalMenu[StoreCurrentIndex].LevelMenu == 200 || pGlobalMenu[StoreCurrentIndex].LevelMenu == 201 || pGlobalMenu[StoreCurrentIndex].LevelMenu== 202 || pGlobalMenu[StoreCurrentIndex].LevelMenu== 203   )
          {
            Serial.print("\n ---------------- case 3000 >>>>>>\n");  
            RefreshValueDynamicMenu200(StoreCurrentIndex);
            
          }
          DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,0);
          //BlinkCursor(pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].posSubItemsMenu+ pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[ItemsToDisplay][0].LenghtSubItemsMenu,0 );
          ValidationOngoing = false ;
          EditingItems = false;
          BlinkOff();
      }
      else {
        Serial.print("\n ----------------  ON NE VALIDE PAS LA SAISIE et on SORT de la validation et ne fait rien \n");
         // a deplacer dans la gestion du bouton validation
          //    //     // cancel editing value & restore the value before the edition
        ItemsToDisplay = StoreItemsToDisplay ; // back to menu before validation 
        CurrentIndexMenu  = StoreCurrentIndex;
        Serial.print(StoreItemsToDisplay,DEC);
        if(pGlobalMenu[StoreCurrentIndex].LevelMenu == 200 || pGlobalMenu[StoreCurrentIndex].LevelMenu == 201 || pGlobalMenu[StoreCurrentIndex].LevelMenu== 202 || pGlobalMenu[StoreCurrentIndex].LevelMenu== 203   )
        {
            Serial.print("\nRestore good value \n");  
            *(byte*)pGlobalMenu[StoreCurrentIndex].DynValuesDisplayed[ItemsToDisplay][0].PointerValue = CurrentValueProbeEditing;
            RefreshValueDynamicMenu200(StoreCurrentIndex);
            ValidationOngoing = false ;
            EditingItems = false;
            BlinkOff();
       }     
     }
   }       
  }break;
 }
}




void DisplayValidationProcess(){
  CurrentIndexMenu  = ConstIndexValidMENU;
  char tabX[]        ="X";
  char tabE[]        =" ";

  if (ValidationChoice == true) { // Yes choice selectedValidationChoice = !ValidationChoice; // toggle choice Yes / No

          Serial.print("\n ---------------- DisplayValidationProcess >>>>>>\n");   
          ItemsToDisplay    = 0;
          
          substituteChars(tabX,
          pGlobalMenu[CurrentIndexMenu].ItemsMenu[1] , 
          pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[1][0].posSubItemsMenu , 
          pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[1][0].LenghtSubItemsMenu );
          substituteChars(tabE,
          pGlobalMenu[CurrentIndexMenu].ItemsMenu[1] , 
          pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[1][1].posSubItemsMenu , 
          pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[1][1].LenghtSubItemsMenu );
          DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine); 
          BlinkCursor(pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[1][0].posSubItemsMenu, 1 ); // Yes to default
  }
  else
  {
          Serial.print("\n ---------------- DisplayValidationProcess NO >>>>>>\n");   
          ItemsToDisplay    = 0;
          CurrentIndexMenu  = ConstIndexValidMENU;
          substituteChars(tabX,
          pGlobalMenu[CurrentIndexMenu].ItemsMenu[1] , 
          pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[1][1].posSubItemsMenu, 
          pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[1][1].LenghtSubItemsMenu );
          substituteChars(tabE,
          pGlobalMenu[CurrentIndexMenu].ItemsMenu[1] , 
          pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[1][0].posSubItemsMenu , 
          pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[1][0].LenghtSubItemsMenu );
          DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine); 
          BlinkCursor(pGlobalMenu[CurrentIndexMenu].DynValuesDisplayed[1][1].posSubItemsMenu, 1 ); // No to default
  }
          
          
}

void ReadTemperature(){
  
  for(unsigned char i =0; i< TOTAL_PROBES ;i++ )
  {
    pSondes[i].CurrentTemperature =(analogRead(pinSonde[i])- pSondes[i].ValuesFunctions.val_const )/ pSondes[i].ValuesFunctions.val_coef; /// zentra // mesure le capteur sur tuyau solaire CTN zentra y = -7,8431x + 718,86
    if(pSondes[i].CurrentTemperature >100)
      pSondes[i].CurrentTemperature = 99.9;
    // hysteresis function
    if (pSondes[i].CurrentTemperature > float(pSondes[i].Threshold + pSondes[i].DeltaOn )) {
       pSondes[i].Status = 100;
    }
    else
    {
      if (pSondes[i].CurrentTemperature < float(pSondes[i].Threshold - pSondes[i].DeltaOff )) {
         pSondes[i].Status = 0;
      }
    }

    //Serial.println("--------Debut----------- ");  
    
    //Serial.print(now.year(), DEC);
    //Serial.print('-');
    //Serial.print(now.month(), DEC);
    //Serial.print('-');
    //Serial.print(now.day(), DEC);
    //Serial.print('\t');
    //Serial.print(now.hour(), DEC);
    //Serial.print(':');
    //Serial.print(now.minute(), DEC);
    //Serial.print(':');
    //Serial.print(now.second(), DEC);
    //Serial.println();
    
    //Serial.println(i);
    //Serial.println(pSondes[i].CurrentTemperature);
    //Serial.println(pSondes[i].Threshold);
    //Serial.println(pSondes[i].DeltaOn);
    //Serial.println(pSondes[i].Status);
    //Serial.println("--------Fin----------- ");  
    
  }
}

//
//void ReadStateInput(){
//  digitalRead(ECHKM2); // revoir remove 
//}

void OptimizeCompute(){
  delay(100);
}




void managekeypad(){
  char valButtons;
  adc_key_in=analogRead(bouton);                                      // conversion AN  
  char bottonpressed [1] = {'*'}; 
  keypad = -1;
           
  voltage = map(adc_key_in, 0, 1023, 0, 5000);                   // on calcule la tension
  voltage=voltage/1000;
  keypad = get_key(adc_key_in);
  if(keypad>=0 && ( millis() - interruptLockout ) > 10)
  {
    bottonpressed[0] = Bouton_val[keypad];
    interruptLockout = millis();
    Serial.print("\n ************************************************* \n");  
    Serial.print( "adc_key_in: \t");
    Serial.println(adc_key_in);
    Serial.print("\n ************************************************* \n");

    switch (bottonpressed[0]){
      case 'H':{
        Serial.print(" Button UP pressed \n");
        onEventButton_High();
      }
      break;
      case 'B':{
        Serial.print(" Button DOWN pressed \n");
        onEventButton_Low();
      }
      break;
      case 'L':{
        Serial.print(" Button LEFT pressed \n");
        onEventButton_Left();
      }
      break;
      case 'R':{
        Serial.print(" Button RIGHT pressed \n");
        onEventButton_Right();
      }
      break;
      case 'V':{
        Serial.print(" Button VALID pressed \n");
        onEventButton_Valid();
      }
      default:
       Serial.print("Unknown button\n");
      break;
    }
  }
  
}

void loop() {

  now = rtc.now();
  ReadTemperature();
  managekeypad();


  //hystThresh(value_SondeTemp);
  //ReadStateInput();
  //OptimizeCompute();


  switch (pGlobalMenu[CurrentIndexMenu].LevelMenu ){
    // Measure Menu 
    case 10:{ 
        Serial.print("\n ---------------- Loop case 10 >>>>>>\n");    
        ItemsToDisplay =0;
        CursorLine = 0;
        CurrentIndexMenu = ConstIndexMeasure10MENU;
        DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
        RefreshDynamicMenu10();
      }
      break;
      
    case 200 ... 203 :{ 
        if (EditingItems == false){
          Serial.print("\n ---------------- Loop case 200 >>>>>>\n");    
          DisplayLCDMenu(&pGlobalMenu[CurrentIndexMenu],ItemsToDisplay,CursorLine);
        }
      }
      break;  
    
    }
  delay(100);
}
