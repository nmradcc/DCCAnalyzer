# DCCAnalyzer

DCC Parser extensions. Intended to assist NMRA DCC conformance testing.

Currently supports 3 modes of operation: Command Station, Decoder and Service Mode.
These modes set the limits of acceptance/ decoding of bits and half bits according to NMRA DCC S-9.1, for command stations and decoders.
The need for different limits is a result of conformance testing requirements. Command station generated signal limits are more restrictive
than the required acceptance limits for decoders. Hence, Decoder mode allows testing with limit signals that would be rejected in 
command station conformance tests, to ensure decoders conform and accept those signals.
  Service Mode is included to allow testing S-9.2.3 commands and service mode programming, while having commands that overlap 
with main operation commands to be correctly parsed.
  An error correction parameter was added to compensate for timing errors discovered during evaluation of the Digital Logic Analyzer. 
  Analysis conducted by the testing team showed consistent clock frequency errors as well as sampling errors, affecting both normal and 
  stretched signal bits. the correction factor laxes or hardens both minimum and maximum bit and half-bit limits to compensate for the
  errors. A positive value extends the limits, while a negative value narrows them.  
  The extension supports mode selection and the relative error correction values expressed in [PPM] (Parts Per Million).
  Limits are calculated using the following formulas:

In Command station mode:
1 bit: 
half bit min. 55uSec * (1.0 – correction[ppm]/1,000,000.0)   max. 61uSec * (1.0 + correction[ppm]/1,000,000.0)
0 bit:
Half bit min. 95uSec * (1.0 – correction[ppm]/1,000,000.0)   max. 9900uSec * (1.0 + correction[ppm]/1,000,000.0)

In Decoder and other modes:  
1 bit: 
half bit min. 52uSec * (1.0 – correction[ppm]/1,000,000.0)   max. 64uSec * (1.0 + correction[ppm]/1,000,000.0)
0 bit:
Half bit min. 90uSec * (1.0 – correction[ppm]/1,000,000.0)   max. 10000uSec * (1.0 + correction[ppm]/1,000,000.0)

In addition there is a calculated max. length allowed for a 0 bit which is checked,

Max. bit = 12000uSec * (1.0 + correction[ppm]/1,000,000.0)

Sample counts are rounded to the closest integer using the standard C++ 11 round() function.

Starting with 1.0.0, RailCom Cutout Detection is supported. This feature just shows where the cutout happens--it cannot
decode any RailCom packets becuase there is no RailCom Detector. This was testing useing the Lenz LZV200 DCS.

