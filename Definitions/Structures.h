
typedef struct {
  unsigned char   posSubItemsMenu;        // Position of dynamic menu
  unsigned char   LenghtSubItemsMenu;     // Lenght of dynamic menu
  unsigned char   NbSubItemDynMenu;       // Number of dynamic menu ( /!\ Same value in the same Menu line item  )
  char            Unity;                  // Unity none or from value set (°C, Volt, Ampere, etc ...)
  void *          PointerValue;           // Pointer to variable ( Probe Temperature, Probe state etc .. int,float, bool, double types)
} SubItemDynMenu_t;

typedef struct {
  unsigned char up;
  unsigned char down;
  unsigned char left;
  unsigned char right;
  unsigned char valid;
  unsigned char exceptionFlag; // 0 = normal, 1 = saisie, 2 = validation, etc.
} MenuNavigation_t;


typedef struct {
  unsigned char       IndexTabMenu;       // Index Tab Menu
  unsigned int        LevelMenu;          // Level of Menu and sub items
  char**              ItemsMenu;          // Tab of char with the different lines menu
  char                NbItemsMenu;        // Number of lines for given menu
  SubItemDynMenu_t**  DynValuesDisplayed; // Dynamic Menu built-in the lines --> Detected automatically by "##" included the Unity if available
  MenuNavigation_t    nav;
} Menu_t;

// Values of Linear CTN function in order to convert Volt to Degre
typedef struct {
    int val_const;
    float val_coef;
} CTNfunction;


/// Structures Probes temperature
struct StructProbe{
   unsigned char Number;
   unsigned char Threshold;
   unsigned char DeltaOn;
   unsigned char DeltaOff;
   CTNfunction ValuesFunctions;
   float CurrentTemperature;
   bool Status;
};




