// my Librarary by SendSay


//Work with EEPROM
    #define EEPROM_write(address, p) {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) EEPROM.write(address+i, pp[i]);}
    #define EEPROM_read(address, p)  {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) pp[i]=EEPROM.read(address+i);}

// print Deug data
    // #define DEBUG_ENABLE         // insert in H file

    // #ifdef DEBUG_ENABLE          // insert in setup 
    //     Serial.begin(9600);
    // #endif

    #ifdef DEBUG_ENABLE
    #define DEBUG(x) Serial.println(x)
    #else
    #define DEBUG(x)
    #endif