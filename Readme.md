# Key Mapping for Infinity Foot Pedal

Reads foot pedal events from `/dev/usb/hiddev0` and translates them into `/dev/uinput` keyboard events.

# Requirements

* uinput kernel support - In Ubuntu quantal it is compile in

## Motivation

I bought a footpedal on Ebay an to my disappointment it was difficult to get it to work under linux or windows.

I tried the fantastic [footpedal](http://code.google.com/p/footpedal/), and it worked fine, but I had trouble going through 100s of lines of code and wasn't able to figure out how to enable key repeating.

Therefore, I wrote my own little tool that reads the HID events and triggers uinput keyboard events.

## License

Johannes Ahlmann, 2012-12-21
Released under MIT Expat License

## Resources

Based on [1](http://www.einfochips.com/download/dash%5Fjan%5Ftip.pdf) and [2](http://www.linuxquestions.org/questions/linux-hardware-18/installing-a-usb-scale-503125/)

## Other Solutions

[footpedal](http://code.google.com/p/footpedal/)
