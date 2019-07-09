# Overwatch-Reaper-Guns
Design files for Reaper's cosplay guns from Overwatch

---

This repo is my first for an actual "product" I've designed. It's a step forward in my exploration of embedded systems and circuit design, expanding on both my previous [Light Sensor](https://github.com/christian-kramer/Light-Sensor) and [STM32F407 Familiarization](https://github.com/christian-kramer/STM32F407-Familiarization) projects.

This project came about due to a need from a friend of some electronics and 3D models for a custom costume she was making: Reaper, from the videogame Overwatch.

![Reaper](https://vignette.wikia.nocookie.net/overwatch/images/f/f4/Reaper_Concept.jpg/revision/latest?cb=20160502182652)
![Reaper](https://vignette.wikia.nocookie.net/overwatch/images/e/ee/Reaper_portrait.png/revision/latest?cb=20160429041404)

Specifically, Reaper's dual shotguns. The requirements were:


1. All the red lights on the guns *(two spots underneath the barrel, one on the side-switch, one on the front underneath the barrels, and one near the top of the gun facing backwards towards Reaper)* need to be illuminated with a lighting effect accurate to the in-game look.
2. The barrels need to illuminate when the trigger is pulled, to mimic shots being fired.
3. Sound effects of a shot firing need to play every time the trigger is pulled, with no more than 4 shots per gun firing before the "reload" sound effect is played. Both guns need to reload at the same time, so if one is empty before the other, it needs to wait for the last shot to be fired from the other gun. Additionally, voice lines Reaper says in the video game after a kill need to play during the reload... making sure to play a different voice line each time.
4. Power is to be provided by two AA batteries per gun, due to space constraints in the handle.

So... Logically, thinking about this, I know I need some lights, a speaker, a radio, a microcontroller fast enough to handle the task of producing sound somehow, and a circuit board to hold everything together.

Cool! Sounds like a fun project. Let's start by figuring out what kind of lighting looks good.

LEDs are an obvious first thought, since they're so cheap and ubiquitous. But they're also very bright, and has a point-like look, even when diffused. Even after throwing everything I could think of at a standard 5mm red LED, I just couldn't get it to achieve the same look as the game. I tried thinking of other things that might produce a much deeper, richer red... A while ago, I had seen something cool on Adafruit called EL Wire, that might work. It's got a really, really nice effect:

![EL Wire](https://cdn-shop.adafruit.com/970x728/403-00.jpg)

But it requires a clunky inverter, and I'm dubious about how it'll show up in bright light.

I started looking around for other solutions, and stumbled upon this:

![Side-glow Fiber](https://images-na.ssl-images-amazon.com/images/I/61J891DcQsL._SX425_.jpg)

["Side-glow Fiber Optic Cable."](https://www.amazon.com/light-fiber-source-illuminator-Yellow/dp/B01M68WEUS) It looks great, and it's not electrical in nature! You can cut it to length, and whatever color LED you put into it is what you'll get out. So I ordered some, to try it out with the red LED. Unfortunately, it just wasn't bright enough. Too much light was being emitted from the sides of the LED, and not enough was going directly into the fiber optic cable. I was certain that if I could just get the LED to focus all of its energy into it, it would be bright enough. The first thing that comes to mind when trying to get light into a single point is a laser, so I started researching red laser diode modules and how to use them.

Turns out, all they need is 3 volts and a constant current supply of around 20 milliamps, and you can get [10 for $6.00 on Amazon](https://www.amazon.com/Laser-Diode-Module-650nm-10PCS/dp/B0166JFLES/ref=sr_1_3?keywords=laser+diode+module&qid=1562638198&s=gateway&sr=8-3) (with Prime, even!).

I ordered some, and I was blown away at how great it looked with the fiber optic cable!

![Laser 1](https://i.imgur.com/6lUHG9p.jpg)
![Laser 2](https://i.imgur.com/T2757rs.jpg)

My phone's camera doesn't do it justice. To the human eye, the light is perceived as very intense red. Not blindingly so, but it definitely looks a whole lot like what the video game shows for the lighting effects! I guess we're using lasers in our project.

My next step was to rip an existing solid 3D gun model off of thingiverse and hollow it out to find space for all the parts, and also figure out how to get the trigger to move instead of just being static like on the original model.

After much work figuring out how to make everything fit, and mount securely inside, I came up with this configuration:

![Gun Internals](https://i.imgur.com/WWzVhCf.png)
![Gun Internals](https://i.gyazo.com/fb137c49be7c38b8835679810bd42a45.gif)

In the handle are the two AA batteries, and behind the trigger is the microswitch I'm using to detect actuation and also provide the necessary "springiness" to give the feeling of a nice click when depressed. In blue is the speaker, positioned on a sliding tray above the decorative vent holes I drilled out to pass sound through to the outside world. Towards the front of the gun is the circuit board I still had yet to design, but I made it plenty big enough (8cm x 10cm) so that I should have no trouble fitting all the components I need onto it. On the circuit board, I modeled a little laser-holder to keep the side-glow fiber optic cable attached to the lasers, and the lasers attached to the board. Just a couple 3mm screw holes holding a couple clamshells together, with enough tension to make sure nothing moves. Simple, and easy to 3D print.

Speaking of circuit boards, I need to design one! But I need to figure out what to put on it first... I've got the lasers' chunk of real-estate dished out, but I still need to determine exactly what else I need to provision for. Well, I need a microcontroller to start. I think the ol' PIC10F220's out of the question for this one... I need something with a little more oomph. I remember really liking the STM32F407 when I was screwing around with my dev board, but that's probably a bit overkill for a project like this. I want a Goldilocks processor... so in the spirit of trying to use the bare minimum in my projects, and because I used a PIC in my last project, I'm going to look for a bottom-of-the-barrel STM32. Let's see what eBay's got.

![STM32F030C6T6](https://i.imgur.com/2hLZGfH.png)

I mean... that's nice and all, but that's still a lot of pins at a very high clock speed. I said bottom-of-the-barrel, darn it!

![STM32F030F4P6](https://i.imgur.com/4rG12cA.png)

Now we're talking! It's not even in a quad flat package, it's dual-inline with only 20 pins! Let's load up CubeMX and see what it's got for features. (probably not a whole lot)

![CubeMX](https://i.imgur.com/QxeYHbS.png)

Actually, not bad! It's got an ADC, support for *both* I2C and SPI, 5 general-purpose timers plus a watchdog timer, and a real-time clock. Not bad for a 20-pin part. I think we have a winner for our microcontroller.

Also, as a bonus, dev boards are dirt-cheap.

![Dev Board](https://i.imgur.com/KIK6ooC.png)

Alright, I think now's a good time to tackle the hard part: sound.

I know that I use file formats like WAV and MP3 to store sound on my PC, but is any of that readable to a microcontroller of this simplicity? I needed to learn a lot more about how these formats work under the hood, and how other people have gone about playing sound on devices like the Arduino.

Apparently, since a WAV file is uncompressed, it represents what is called "PCM Audio", or "Pulse-Code Modulation Audio". This means that the bytes in a WAV file sequentially represent amplitude data of the audio waveform - the thing you see when you open something in Audacity or the like. Depending on the sample rate and resolution, this can be as simple as one byte representing an aplitude of anywhere from 0 to 255, played one after the other, 8000 times per second for an 8-bit, 8kHz audio file.

Here's a good visualization:

![Sampling](https://i.imgur.com/vSnqeyg.png)

Each line is 1/8000 of a second apart (sample rate of 8kHz) and the value at each line can be anywhere from 0 to 255 (8-bit resolution). Turn those numbers into voltages that move a speaker cone, and baby, you've got audio.

Now how the heck do I get this thing to talk to a speaker? It certainly doesn't do so natively, like the STM32F407 does... gotta figure that one out on my own. Somehow, I need to get that waveform out to an amplifier. Most times, that's done with something called a Digital to Analog converter, or DAC, which you'll hear thrown around on various products' descriptions. But in the spirit of bare-minimum, I tried to see if I could use Pulse-Width Modulation combined with a filtering circuit to create the analog waves.

I'm just going to say right now, that I couldn't get that to work no matter how hard I tried. I'm going to have to bookmark that technique for later. For now, I'm saying "screw it" and using a DAC. But which DAC should I use?

A fairly popular one amongst the hobbyist community, produced in droves in China, is called the MCP4725. 

![MCP4725](https://i.imgur.com/9KLXux1.png)

It's a nifty little thing, in the same size package as the AAT1217 (and the PIC10F220, for that matter). 12-Bit Resolution, rail-to-rail output which means it'll give me the full swing of 0v to 3.3v, and it supports High-Speed I2C which is more than enough to cover the 8kHz sample rate plus I2C frame data. Perfect! Now I gotta find an amplifier, because I really don't trust this thing to drive a speaker the size of what I need.

![LM386](https://i.imgur.com/mErxf0R.png)

Now, every good EE has heard of the legendary LM386. Since 1969, it's been powering hobbyist and professional audio projects where "sounds good enough" reigns supreme. It's simple, yet versatile, and does what it's supposed to without a whole lot else. It takes an input, makes it louder, and gives you an output for an 8Ω speaker. If you need it even louder, put a capacitor between pins 1 and 8. Sound too staticy? Put a capacitor between pin 7 and ground. Boom, done. Loud. Happy? Good.

These chips are so common, even the knockoffs have knockoffs. You can buy buckets of them for next to nothing, which makes them great for this project and possibly future ones since I'll probably have plenty laying around.

One problem, though... It needs a minimum of 4v to work, and everything else I've got is 3.3v. Looks like I need two power rails... remember that, for later.

Wait, sorry, I see two problems... the LM386 input is expecting a voltage between 0 and 0.4v, not 0 and 3.3v like my DAC is putting out. Anything above 0.4v is going to get clipped, which means I need to figure out how to scale the audio. I want to make use of the full 12-bit resolution of the MCP4725, so I'm thinking maybe scaling the output of the DAC using some analog circuitry before it gets to the LM386 might be a good idea.

A common tactic for scaling an analog signal like this is to use two resistors in a configuration called a "voltage divider".

![LM386](https://upload.wikimedia.org/wikipedia/commons/thumb/3/31/Impedance_voltage_divider.svg/220px-Impedance_voltage_divider.svg.png)


If we want 3.3v (the upper limit of the DAC) to correspond with 0.4v (the upper limit of the amplifier input), and we start with a known resistor value of 150kΩ for Z1 (because that's a standard resistor value that I know is produced), we get a nominal Z2 value of 20.69kΩ, which is not a standard resistor that is produced. Luckily, 20kΩ is, and that's "close enough for government work."

I'd also like to explain why I'm choosing resistors in this range of values... the answer, is because I just need a voltage reference for the LM386. I don't want to waste unnecessary battery power heating up resistors, but I also don't want so much resistance that the voltage reference isn't responsive enough for my purposes. These are in a sweet spot between both.

So after all of my dev boards arrived in the mail, and I pulled an LM386 out of an old door alarm, I ended up building this:

![BreadBoard Prototype 1](https://i.imgur.com/020LDsc.png)

The STM32 communicates over High-Speed I2C to the MCP4725, which outputs the PCM WAV file to voltages anywhere between 0 to 3.3v depending on the wave amplitude at that "frame" of the sound file. That 0 to 3.3v signal gets coupled to the voltage divider in the reverse-"L" shape by a 10nF capacitor to help smooth out the "jumps" in the 8kHz audio, then finally gets sent to the LM386 input pin. The LM386 output pin gets coupled to the 8Ω speaker by a pretty big capacitor. Here, I'm using a 330µF electrolytic to remove the DC bias from the amplifier, so the speaker cone goes through its whole range of motion. Additionally, I put a high-pass filter on the output pin to chop off some unwanted low-frequency noise.

The sound file itself is stored on the STM32, compiled from a C character array that I just iterate through 8000 times per second in a for loop.

Unfortunately, due to how small the storage on-board the STM32 is, I found only one voice line of Reaper's that was short enough to fit on it! In order to store *all*  of them, I'm going to need some external storage.

I thought to myself... "Hm... flash storage has been getting pretty dense and cheap, lately. I wonder what kind of chips are out there that might store the amount of data I'm looking for."

I looked into Micro SD Cards, but even for a 2GB card it took over 100 milliamps! Holy cow, I don't have that kind of overhead with these two AA batteries! And 2GB is *way* overkill for what I need. I had to find something smaller, and lower-power.

Eventually, my friend ended up finding this:

![MX25L1606EM2I](https://i.imgur.com/L25Tetp.png)

The Macronix MX25L1606EM2I, a 2MB SPI flash chip capable of exchanging data at very high speeds, at very low power. The cool thing about SPI flash chips is that their form factor is pretty much standardized, as well as their pinout and (for the most part) their instruction set... so in the future, if I theoretically wanted to switch out this chip for a larger one, or a different manufacturer, it should be a drop-in replacement. Pretty convenient!

But unlike a Micro SD Card, I can't just plop this into my PC and drag files onto it... This is just bare memory, I needed to be able to talk to it somehow.

Looking around on eBay, I found a pretty highly-rated Flash IC programmer that supported both SPI and I2C chips called the CH341A.

![CH341A](https://i.imgur.com/Z1JP25S.png)

The MX25L1606EM2I can't plug directly into the ZFI socket on the programmer, so I made sure to find one that included this little "clamp" to hold onto the 8 pins of the flash chip.

The software for it looks like this:

![CH341A Software](https://i.imgur.com/Min93zT.png)

And I gotta say, after changing the language from Chinese, it was pretty much plug-and-play. The programmer talked to the chip just fine, and I could key-in values at whatever memory addresses I wanted.

So... I got the chip to talk to my computer, and store data successfully. But how about reading it from the microcontroller?

Reading into the datasheet, the command structure for reading from the chip is:

1. Pull the Chip-Select pin low.
2. Send it `0x03` in the first byte of data. This is to instruct the chip that we intend to read from it, and we are about to give it an address in the next 3 frames.
2. Send it the first byte of the 6-byte address.
3. Send it the second byte of the 6-byte address.
4. Send it the third byte of the 6-byte address. Note: after these are all sent, the chip will start reading off not only that byte, but every consecutive byte until it reaches the end.
5. Finally tell it to shut up by pulling the Chip-Select pin high.

And that, in theory, should work. There's two other pins on the chip that are of interest: Write-Protect, and Hold. Write-Protect does what you'd expect, and Hold stops the entire chip from doing anything. From my adventures diving into EE forums, I learned that those pins aren't necessary for normal operation, and can just be left unconnected.

But... when I tried reading a memory address on the flash chip from the STM32, it wouldn't respond! I was getting no data back at all!

So, to troubleshoot, I hooked the chip back up to the CH341A, but this time using DuPont jumper wires, and verified that it could talk to the PC. Then, I pulled the Write-Protect jumper out to see if that was the culprit... No dice, the flash chip was still acting normally. Next, I pulled the Hold jumper out... and bingo! I got a Read Error from the software saying that the chip wasn't responding.

So, instead of leaving Hold unconnected like I originally did, I tried tying it to ground with that jumper... And the read error went away! It was behaving normally again.

On the microcontroller side of things, I tried using another jumper to tie Hold to ground while leaving Write-Protect floating... and suddenly, I got the byte of data I was expecting!

My breadboard prototype now looked like this:

![BreadBoard Prototype 2](https://i.imgur.com/GSiPe29.jpg)
