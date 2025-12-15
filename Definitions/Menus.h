#define ConstIndexMAIN_MENU             0      // Manage the interruption request
#define ConstIndexMeasure10MENU         1
#define ConstIndexSonde20_MENU          2
#define ConstSonde200_MENU              3
#define ConstSonde201_MENU              4
#define ConstSonde202_MENU              5
#define ConstSonde203_MENU              6
#define ConstIndexValidMENU             7
#define ConstIndexDebugSystem30_MENU    8
#define ConstIndexDate40_MENU           9 
#define ConstIndexSystemDebug300_MENU   10
#define ConstIndexSystemVersion301_MENU  11

#define INVALID_MENU                    'X'



#define LevelMenu_MAIN                0
#define LevelMenu_Measure10          10 
#define LevelMenu_Sonde20            20
#define LevelMenu_DebugSystem30      30  
#define LevelMenu_Sonde200           200
#define LevelMenu_Sonde201           201
#define LevelMenu_Sonde202           202
#define LevelMenu_Sonde203           203  
#define LevelMenu_Validation1000     1000
#define LevelMenu_SystemDebug        300
#define LevelMenu_SystemVersion      301  
#define LevelMenu_Date40             40

static const MenuNavigation_t MAIN_MENU_NAV = {ConstIndexMAIN_MENU, ConstIndexMAIN_MENU, ConstIndexMAIN_MENU, ConstIndexMeasure10MENU,INVALID_MENU, 0}; // up,down,left,right,validation, exceptionFlag ->OK verified
static const MenuNavigation_t Measure10_MENU_NAV = {ConstIndexMAIN_MENU, ConstIndexSonde20_MENU, ConstIndexMAIN_MENU, INVALID_MENU,INVALID_MENU, 0}; // up,down,left,right,validation, exceptionFlag -> OK verified
static const MenuNavigation_t Sonde20_MENU_NAV = {ConstIndexMeasure10MENU, ConstIndexDebugSystem30_MENU, ConstIndexMAIN_MENU, ConstSonde200_MENU,INVALID_MENU, 0}; // up,down,left,right,validation, exceptionFlag
static const MenuNavigation_t Sonde200_MENU_NAV = {ConstSonde203_MENU, ConstSonde201_MENU, ConstIndexSonde20_MENU, ConstSonde201_MENU,INVALID_MENU, 0}; // up,down,left,right,exceptionFlag
static const MenuNavigation_t Sonde201_MENU_NAV = {ConstSonde200_MENU, ConstSonde202_MENU, ConstIndexSonde20_MENU, ConstSonde202_MENU,INVALID_MENU, 0}; // up,down,left,right,exceptionFlag
static const MenuNavigation_t Sonde202_MENU_NAV = {ConstSonde201_MENU, ConstSonde202_MENU, ConstIndexSonde20_MENU, ConstSonde203_MENU,INVALID_MENU, 0}; // up,down,left,right,exceptionFlag
static const MenuNavigation_t Sonde203_MENU_NAV = {ConstSonde202_MENU, ConstSonde200_MENU, ConstIndexSonde20_MENU, ConstIndexDebugSystem30_MENU,INVALID_MENU, 0}; // up,down,left,right,exceptionFlag
static const MenuNavigation_t Validation1000_MENU_NAV = {ConstIndexMAIN_MENU, ConstIndexMAIN_MENU, ConstIndexMAIN_MENU, ConstIndexMAIN_MENU,INVALID_MENU, 2}; // up,down,left,right,exceptionFlag -> OK verified
static const MenuNavigation_t DebugSystem30_MENU_NAV = {ConstIndexMAIN_MENU, ConstIndexDate40_MENU, ConstIndexMAIN_MENU, ConstIndexSystemDebug300_MENU,INVALID_MENU, 0}; // up,down,left,right,exceptionFlag
static const MenuNavigation_t SystemDebug300_MENU_NAV = {ConstIndexDebugSystem30_MENU, ConstIndexSystemVersion301_MENU, ConstIndexDebugSystem30_MENU, ConstIndexSystemVersion301_MENU,INVALID_MENU, 0}; // up,down,left,right,exceptionFlag
static const MenuNavigation_t SystemVersion301_MENU_NAV = {ConstIndexDebugSystem30_MENU, ConstIndexSystemDebug300_MENU, ConstIndexDebugSystem30_MENU, ConstIndexDebugSystem30_MENU,INVALID_MENU, 0}; // up,down,left,right,exceptionFlag
static const MenuNavigation_t Date40_MENU_NAV = {ConstIndexDebugSystem30_MENU, ConstIndexMAIN_MENU, ConstIndexMAIN_MENU, ConstIndexMAIN_MENU,INVALID_MENU, 0}; // up,down,left,right,exceptionFlag;
static const MenuNavigation_t Val_1000_MENU_NAV = {ConstIndexMAIN_MENU, ConstIndexMAIN_MENU, ConstIndexMAIN_MENU, ConstIndexMAIN_MENU,INVALID_MENU, 2}; // up,down,left,right,exceptionFlag -> OK verified




static char pIndexGlobalMainMenu[] = {
  ConstIndexMeasure10MENU,
  ConstIndexSonde20_MENU,
  ConstIndexDebugSystem30_MENU,
  ConstIndexDate40_MENU
};

#define GLOBAL_MAIN_MENU_SIZE (sizeof(pIndexGlobalMainMenu)/sizeof(pIndexGlobalMainMenu[0]))

static char pIndexGlobalSondeMenu[] = {
  ConstSonde200_MENU,
  ConstSonde201_MENU,
  ConstSonde202_MENU,
  ConstSonde203_MENU
};

#define GLOBAL_SONDE_MENU_SIZE (sizeof(pIndexGlobalSondeMenu)/sizeof(pIndexGlobalSondeMenu[0]))



static const char* MAIN_MENU_ITEMS[] = {
  "  1 - MESURES",
  "  2 - SONDES",
  "  3 - SYSTEME",
  "  4 - DATE",
  "  5 - RESEAUX"};

static const Menu_t MAIN_MENU = {
  ConstIndexMAIN_MENU,  
  LevelMenu_MAIN,
  MAIN_MENU_ITEMS,
  5,
  NULL,
  MAIN_MENU_NAV
};

static const char* Measure10_MENU_ITEMS[] = {
  "S1 ####  S2 ####", 
  "E-HKM2 # E-CC # ", /// OFF -> X  //// ON -> O ///// position7 1er etat ,position14 2eme etat
  "FIOUL # CHAUF # "}; /// OFF -> X  //// ON -> O ///// position7 1er etat ,position14 2eme etat


 
 static const Menu_t Measure10_MENU = {
  ConstIndexMeasure10MENU,   
  LevelMenu_Measure10,
  Measure10_MENU_ITEMS,
  3,
  NULL,
  Measure10_MENU_NAV
};


 
static const char* Sonde20_MENU_ITEMS[] = { // Sonde 1  à sonde N 
  "  SONDE 1:Tuyau", 
  "  SONDE 2:Ballon", 
  "  SONDE 3 ", 
  "  SONDE 4 "}; 

 static const Menu_t Sonde20_MENU = {
  ConstIndexSonde20_MENU,
  LevelMenu_Sonde20,
  Sonde20_MENU_ITEMS,
  4,
  NULL,
  Sonde20_MENU_NAV
};

 

 static const char* Sonde200_MENU_ITEMS[] = { // Sonde 1  à sonde N 
  "Sonde   ##  ", 
  "Seuil   ## C",
  "Del.on  ## C", 
  "Del.off ## C"
  }; 

static const char* Sonde201_MENU_ITEMS[] = { // Sonde 1  à sonde N 
  "Sonde   ##  ", 
  "Seuil   ## C",
  "Del.on  ## C", 
  "Del.off ## C"
  }; 
  
  static const char* Sonde202_MENU_ITEMS[] = { // Sonde 1  à sonde N 
  "Sonde   ##  ", 
  "Seuil   ## C",
  "Del.on  ## C", 
  "Del.off ## C"
  }; 
  
  static const char* Sonde203_MENU_ITEMS[] = { // Sonde 1  à sonde N 
  "Sonde   ##  ", 
  "Seuil   ## C",
  "Del.on  ## C", 
  "Del.off ## C"
  }; 


 static const Menu_t Sonde200_MENU = {
  ConstSonde200_MENU,
  LevelMenu_Sonde200,
  Sonde200_MENU_ITEMS,
  4,
  NULL,
  Sonde200_MENU_NAV
};


static const Menu_t Sonde201_MENU = {
  ConstSonde201_MENU,
  LevelMenu_Sonde201,
  Sonde201_MENU_ITEMS,
  4,
  NULL,
  Sonde201_MENU_NAV
};

static const Menu_t Sonde202_MENU = {
  ConstSonde202_MENU,
  LevelMenu_Sonde202,
  Sonde202_MENU_ITEMS,
  4,
  NULL,
  Sonde202_MENU_NAV
};

static const Menu_t Sonde203_MENU = {
  ConstSonde203_MENU,
  LevelMenu_Sonde203,
  Sonde203_MENU_ITEMS,
  4,
  NULL,
  Sonde203_MENU_NAV
};


static const char* Validation_MENU_ITEMS[] = {
  "Validation", 
  "  OUI #  NON # "
};


static const Menu_t Val_1000_MENU = {
  ConstIndexValidMENU,
  LevelMenu_Validation1000,
  Validation_MENU_ITEMS,
  2,
  NULL,
  Val_1000_MENU_NAV
};

static const char* SYSTEM_MENU_ITEMS[] = {
  "VERSION",
  "DEBUG"
  };


static const Menu_t SYSTEM_MENU = {
  ConstIndexDebugSystem30_MENU,
  LevelMenu_DebugSystem30,
  SYSTEM_MENU_ITEMS,
  2,
  NULL,
  DebugSystem30_MENU_NAV
};

static const char* SystemDebug300_MENU_ITEMS[] = {
  " Debuggage Serie ",
  "  OUI #  NON #   "
  }; 

 static const Menu_t SystemDebug300_MENU = {
  ConstIndexSystemDebug300_MENU,
  LevelMenu_SystemDebug,
  SystemDebug300_MENU_ITEMS,
  2,
  NULL,
  SystemDebug300_MENU_NAV
};


static const char* SystemVersion301_MENU_ITEMS[] = { 
  " Version Système ",
  "  v1.0.0         " /// to update dynamically
  }; 

 static const Menu_t SystemVersion301_MENU = {
  ConstIndexSystemVersion301_MENU,
  LevelMenu_SystemVersion,
  SystemVersion301_MENU_ITEMS,
  2,
  NULL,
  SystemVersion301_MENU_NAV
};




static const char* Date40_MENU_ITEMS[] = { // Date et heure
  "  DATE/HEURE ", 
  "  10-12-2022 ", 
  "  12:00:00   "
  }; 

 static const Menu_t Date40_MENU = {
  ConstIndexDate40_MENU,
  40,
  Date40_MENU_ITEMS,
  3,
  NULL,
  Date40_MENU_NAV
};
