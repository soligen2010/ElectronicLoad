# ElectronicLoad

Here is my DC Load project, inspired by Dave Jones, MJLorton, and Kibi.

I have this on a breadboard and it all seems to be working, but as I am a beginner, I would appreciate any feedback before I start soldering.

I built in an Arduino and 16 bit DAC (ADS1115) to monitor and display volts and amps without a multi meter. It also displays watts, and software enforces max watts. It will turn off the load if the watts or voltage exceeds the software limits.

Several break-out boards are referenced: Arduino Nano, 16x2 LCD, ADS1115, and my own negative 2.5 volt generator. I don’t have the schematic drawn for the -2.5 generator. Basically the PWM drives a totem pole and capacitors/diodes to get the negative voltage, then regulated with a TL431A. Even though the TLC272 is supposed to go to ground, I found it wasn’t very good near ground, so I added the negative. I probably only needed -.5V, but -2.5V is the TL431 reference voltage.

Power is supplied by an old camera battery charger that I hacked into a DC supply. I figure anything at around 9V should work fine. The heat sink fan is the only thing driven directly by the input voltage. Everything else either uses the Arduino’s built in 5V regulator or 7.5V regulated by VR1.

Output from IC1A is split with identical voltage dividers – one to set the input to IC1B and the other goes to the DAC for reading by the Arduino.

Q2 is Arduino controlled and turns the load on or off. R13 ensures the output of IC1B is low when the load is off by pulling the non-inverting input down to about -.6V. The LCD display will show either the “set” amps or the actual amps depending if it is in “set” or “run” mode (controlled by MODE_BUTTON).

Q3 is an HRF3205 that I salvaged from an old UPS. I had started out using an FQP30N06, but I fried several, so went to the HRF3205 which handles a lot more current. The only theory I have for FQP30N06 is the amp surge when the load turns on. I did a re-design to reduce the turn on surge time by making sure IC1B output is LOW when the load is off (Q2 and R13). But I still went with the HRF3205.

Stabilizing the feedback was my biggest challenge. I wish I would have bought an oscilloscope sooner . When I changed to the HRF3205, I had to change C4 from 10n to 100n, added R14 (and R5 for symmetry) and removed the 100R resistor I had between IC1B and Q3. C4 ended up bigger than I expected it should be, and I am a bit uncertain about the lack of a Q3 gate resistor, so comments on this are welcome.

I didn’t fully buffer the inputs to the DAC. When I had put them in, the buffer added error, so I decided to remove them to simplify the circuit and compensate for the low impedance DAC inputs in software. The software will handle all calibration and compensation for any mis-matched component values.

I tested with various amp settings at 3.3ish, 5ish, and 12ish volts using a converted ATX power supply. I also testing the low end with an AA cell that I abusively drained. I’d like to test with some higher voltages to see how it does, but first I’ll need to build something that can go higher than 12V.

 Copyright 2018 by Dennis Cabell
 KE8FZX