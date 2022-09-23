INVERT-A-BIT Headphones Amp 2022 Update
By skate323k137 / AK-DJ

In this folder you will find the Fritzing project files, and Gerber archive, for an updated version of the Invert-a-bit headphones amp for HoustonTracker2. Updates to the original design were to expand the board adding I/O jacks and power options.

Updates 2022 March:
Added power switch ("Breadboard Compatible")
Added 9V DC "Breadboard Compatible" Barrel connector, accepts a standard 9V center negative power supply, i.e. from most guitar pedals.
Added solder terminals for optional 9V battery (DC Jack disconnects battery positive when inserted, so DC Jack power takes priority)
Note: The barrel connector must be installed for the 9v battery circuit to be completed.
Added 3.5mm barrel jacks for input and output (use a 2.5mm to 3.5mm headphones cable to connect the calc to the input)
Added spots for optional RCA jacks. This version will disconnect RCA output when a jack is present in the headphones output connector.
Original power terminals labelled TP1/TP2 can be used for a 2-wire voltmeter, which can be affixed (glued) to the DIP IC.

If normal PCB Standoff feet are attached to the design there should be just enough room underneath for a 9V battery, but don't expect amazing life. Still, this design fits well within the width of a TI-83 so if you design an enclosoure it may serve well for some travel.

Updates 2022 September:
Rerouted traces
Adjusted/increased trace sizes
Fixed positioning of PCB Feet holes
Fixed relevent silkscreens to be easier to read with headphones/power jacks installed.

NOTE:

The original design and gerbers from Snorkel can be found in the 'original' subdirectory with this document.
The original note from Snorkel (who designed the real logic of this unit, and to whom I owe many thanks for his kindness) is below:

--

Hello!

So you decided to build a headphone amp for your texas instruments calculator! Good choice!

This is not really a normal headphone amp. It will only work with 1-bit signals. If you manage to put "normal music" through the "amp"
it will just be turned in to a squarewave and probably sound pretty distorted.
This is because the heart of the thing is a cmos logic chip called CD4069 and all it does is take the squares
coming from the calculator, inverts their phase and buffer them so that it can drive a couple of headphones.
The PCB also includes a lowpass filter that tastfully cuts out a bit of the high frequency ripple coming from the calc.
But no too much! =)

This folder contains all the stuff you should need to build your own amp. Either with the fritzing files or if you wanna order your
own pcb just use the Gerber files at your favorite pcb-manufacturer.

I take no responsibility for this thing. If your stuff breaks when using or building this its your fault.
It worked well for me and its a pretty easy build so you should be fine. (FINGERS CROSSED)

Hope you'll have many fun hours on the train or where ever you wanna sit with your calc.
Enjoy!

/Niklas
--
