#include <Wire.h>
#include <INA219.h>
#include <AD53xx.h>



// cc chargeing regardless of load
// also Vmax limiter

INA219 INA(0x40);
AD53xx My_DAC(0x0C, 2, 8, 2470, &Wire);


uint16_t chargeing_current  = 1000;  // mA
uint16_t Estimated_cell_ESR =  200;  // mOHM
uint16_t charge_target      = 4100;  // mV
uint16_t cell_min_voltage   = 3000;  // mV
uint16_t Vmax_charger = charge_target * 1.05f ;
bool only_load = true; 


uint16_t cell_voltage ;
uint16_t cell_current ;
float  last_voltage_set;
float  delta_current = 0;


uint16_t yy;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void   Load_mode_CC()
{
  float Load_ESR;


  yy= My_DAC.DAC_Voltage_to_Value( 1);    // calculate value needed to get 1V at the output
  My_DAC.DAC_Write_2ch(0x0C , 1, 0, 1, yy );
  digitalWrite(36, 1);  // turn SMPS on
  delay(100);
  cell_current = INA.getCurrent_mA();
  Load_ESR = 1000 / cell_current;  // in OHM
  last_voltage_set =  Load_ESR *chargeing_current /1000;
  yy= My_DAC.DAC_Voltage_to_Value( last_voltage_set );    
  My_DAC.DAC_Write_2ch(0x0C , 1, 0, 1, yy );


}

void   Load_mode_CC_regulator()
{
  
  cell_current = INA.getCurrent_mA();
  Serial.print("Load current in mA: ");
  Serial.println( cell_current  );
  Serial.println();
  delta_current = chargeing_current - cell_current;
  Serial.print("delta_current mA:");
  Serial.println( delta_current );
  

  last_voltage_set= last_voltage_set * ( 1 + ( delta_current *0.75 )/   chargeing_current  );  
  yy= My_DAC.DAC_Voltage_to_Value( last_voltage_set );    
  My_DAC.DAC_Write_2ch(0x0C , 1, 0, 1, yy );
  Serial.print("Set bus voltage:");
  Serial.println( last_voltage_set );

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void   Charge_cell_CC()
{
  float Load_ESR;
  float BussVoltage = INA.getBusVoltage() ;
  if ( BussVoltage >= cell_min_voltage & )
  {
    if ((BussVoltage >= charge_target))
    {
        Serial.print("Cell charged or overcharged");
        // pause*
    }
    else
    {
        Serial.print("Cell is ok for chargeing");
    }
  }
  else
  {
      Serial.print("Cell very discharge and proably bad");
      // pause*
  }

  Serial.print("Bus voltage:  ");
  Serial.println(BussVoltage, 3);


  yy= My_DAC.DAC_Voltage_to_Value( BussVoltage + 0.2 );    // calculate value needed to get Vcell+0.25V at the output
  My_DAC.DAC_Write_2ch(0x0C , 1, 0, 1, yy );
  digitalWrite(36, 1);  // turn SMPS on
  delay(100);
  cell_current = INA.getCurrent_mA();
  Load_ESR = 250 / cell_current;  // in OHM
  last_voltage_set = BussVoltage+  Load_ESR *chargeing_current /1000;
  yy= My_DAC.DAC_Voltage_to_Value( last_voltage_set );    
  My_DAC.DAC_Write_2ch(0x0C , 1, 0, 1, yy );
}


void   Charge_cell_CC_regulator()
{

  //keep Vcell under 4.1v and current under 1A
  cell_current = INA.getCurrent_mA();
  Serial.print("Chargeing current in mA: ");
  Serial.println( cell_current  );
  Serial.println();

  delta_current = chargeing_current - cell_current;
  Serial.print("delta_current mA:");
  Serial.println( delta_current );
  

  last_voltage_set= last_voltage_set * ( 1 + ( delta_current *0.75 )/   chargeing_current  );  
  if( last_voltage_set > Vmax_charger )  
   // poate cumva pun un counter aici si daca trece de n ori prin el atunci trac in alta functie si nu mai am calculele de mai sus
  {
    last_voltage_set = Vmax_charger;
    Serial.print("In CC mode:");
  }
  yy= My_DAC.DAC_Voltage_to_Value( last_voltage_set );    
  My_DAC.DAC_Write_2ch(0x0C , 1, 0, 1, yy );
  Serial.print("Set bus voltage:");
  Serial.println( last_voltage_set );

  if ( cell_current < 50 ) // 50 mA
  {
    // end function
      digitalWrite(36, 0);  // turn SMPS off
  }

}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  // put your setup code here, to run once:
Serial.begin(115200);

pinMode(36, OUTPUT);
//digitalWrite(36, 1);

Wire.begin(2,1);



// test if DAC is detected
byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");


/*
      Serial.println(__FILE__);
  Serial.print("INA219_LIB_VERSION: ");
  Serial.println(INA219_LIB_VERSION);
*/
  if (!INA.begin() )
  {
    Serial.println("could not connect. Fix and Reboot");
  }
 // INA.setBusVoltageRange(16);

  INA.setMaxCurrentShunt(5, 0.05);
  INA.setGain(2);
  INA.setShuntResolution(12);
  INA.setBusResolution(12);
  INA.setShuntSamples(5);
    


cell_voltage = INA.getBusVoltage_mV();
cell_current = INA.getCurrent_mA();


if( cell_voltage <500 )
{
  // load only mode
  only_load = true;
}

else 
{

  if( cell_voltage <6000 && cell_voltage> 2000 )
  {
    //digitalWrite(36, 1);  // turn SMPS on
    only_load =false;
  }
  else
  {
    Serial.print(" more then 1 cell teceted or there is another problem");
  }
}


if (only_load)
{
  Load_mode_CC();
}
else
{
  Charge_cell_CC();
}

}





////////////////////////////////////////////////////////////////////////////////////////////////////////////



void loop() 
{
  // put your main code here, to run repeatedly:



  //  these two can be read without further configuration.
  Serial.print("Bus voltage:  ");
  Serial.println(INA.getBusVoltage(), 3);
  //Serial.print("\t");
  //Serial.print(INA.getShuntVoltage_mV(), 3);
  //Serial.println();
  Serial.print("Load current in A: ");
  Serial.println( INA.getCurrent() ,3 );
  Serial.println();


if (only_load)
{
  Load_mode_CC_regulator();
}



  delay(1000);
}































