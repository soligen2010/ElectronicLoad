
#ifndef __ElectronicLoadCalibration_h__
#define __ElectronicLoadCalibration_h__

#define MAX_VOLTS    25.0F   // Load turns off if Max Volts is exceeded
#define MAX_WATTS    40.0F   // Load turns off if Max Watts is exceeded.  Adjust based on how good your cooling is
#define MAX_AMPS      6.0F   // Load turns off if Max Amps is exceeded


// Calibration procedure
   
/*  Voltage Reading Calibration
 *  
 *  1) Connect a voltage and multi meter to the unit.  Voltage should ideally be around 19V
 *     assuming your multimeter has a 0 - 20V range. 
 *  2) Adjust VOLT_SET_MULTIPLIER and re-flash until the multi meter and display match.
 *     Note: Disconnect the input while re-flashing
 *  3) Connect a low voltage.  The lower the better as long as it is stable. Lower your
 *     multi meter range accordingly for better accuracy
 *  4) Adjust VOLT_SET_OFFSET and re-flash until the multi meter and display match.
 *     Note: Disconnect the input while re-flashing
 *  5) Repeat all steps until no adjustmets are required.
 */
#define VOLT_SET_MULTIPLIER       22.635F       // Multiply ADS reading my this to get input voltage
#define VOLT_SET_OFFSET            0.0F         // Add to voltage reading to correct any offset

/*  Actual Amp Reading Calibration
 *  
 *  1) Connect multi meter between source an load to to read the actual amps being drawn.
 *     set multi meter to a range that can measure 6A or better
 *  2) Adjust the amp set pot to near the highest possible value
 *  3) Connect a 5V source capable of providing at least 6A
 *  4) Turn on the load for a moment and note the difference in readings between 
 *     the load and the multi meter. If the reading on the multi meter is not steady,
 *     turn down the pot until the reading is steady.
 *  5) Make an adjustment to SHUNT_RESISTOR and re-flash
 *     Note: Disconnect the input while re-flashing
 *  6) Repeat steps 4 & 5 until the displays match
 *  7) Adjust the amp set pot alomost all the way down, but not quite the lowest
 *     and set your multi meter accordingly to a low range 
 *  8) Turn on the load for a moment and note the difference in readings between 
 *     the load and the multi meter.
 *  9) Make an adjustment to SHUNT_OFFSET and re-flash
 *     Note: Disconnect the input while re-flashing
 *  10) Repeat steps 8 & 9 until the displays match
 *  11) Repeat all steps until no adjustmets are required. Repeat in other 
 *      multi meter ranges to cross check. The high amp range may not be very
 *      accurate
 */
#define SHUNT_RESISTOR              .213F         // Shunt resistor in ohms
#define SHUNT_OFFSET                -.0005F         // Add to amp reading to correct any offset

/*  Set Amp Reading Calibration
 *   (Multi meter not needed)
 *  
 *  1) Adjust the amp set pot to near the highest possible value
 *  2) Connect a 5V source capable of providing at least 6A
 *  3) Turn on the load for a moment and note the difference between the amp reading
 *     before and after turning onthe load.
 *  4) Make an adjustment to AMP_SET_ADJUST and re-flash
 *     Note: Disconnect the input while re-flashing
 *  5) Repeat steps 3 & 4 until the displays match
 *  6) Adjust the amp set pot alomost all the way down, but not quite the lowest
 *  7) Turn on the load for a moment and note the difference between the amp reading
 *     before and after turning onthe load.
 *  8) Make an adjustment to AMP_SET_OFFSET and re-flash
 *     Note: Disconnect the input while re-flashing
 *  9) Repeat steps 7 & 8 until the displays match
 *  10) Repeat all steps until no adjustmets are required.
 */
#define AMP_SET_ADJUST                0.99955F         
#define AMP_SET_OFFSET                -.0005F        // Add to amp set reading to correct any offset

#endif

