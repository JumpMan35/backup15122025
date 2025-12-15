


// #define Button_High   18      // Manage the interruption request
// #define Button_Low    19      // Manage the interruption request
// #define Button_Right  2       // Manage the interruption request
// #define Button_Left   3       // Manage the interruption request



/// Sonde de température de chaleur sur en entrée analogique
#define SA1           54       // Sonde Zentra tuyau solaire à résistance 20 KOhms
#define SA2           55       // Sonde Honey well Ballon tampon 1000 litres  10 KOhms
#define ProbeSpare1   56       // Sonde sur tuyau solaire Marque de la sonde ????????????????????? et résistance
#define ProbeSpare2   57       // Sonde sur tuyau solaire Marque de la sonde ????????????????????? et résistance
#define bouton        A4       // Bouton pousssoir monter en série avec Haut bas, gauche droite validation


const unsigned char pinSonde[] = {SA1, SA2, ProbeSpare1, ProbeSpare2};

/// Entrées analogiques 
#define ECHKM2        2        // Entrée logique 220 V -> Alimentation 5V, commande HKM2 initialement sur Circulateur de rehausse chauffage fioul (circulateur vert)
#define ECC           3        // Entrée logique 220 V -> Alimentation 5V, commande circulateur de départ (chaudière Fioul chauffage (prise hiver déclencher avec Thermostat maison) (circulateur rouge)
#define InputSpare1   4
#define InputSpare2   5

/// Sorties logiques vers relais 220 V 
#define Relay_SCORF   10         // Sortie  de circulation  optimisée rehausse fioul (temporisée par le HKM2) -> prise relais  220 V
#define Relay_SODC    11         // Sortie  de circulation départ chauffage  optimisée et temporisée
#define Relay_Spare1  12
#define Relay_Spare2  13

