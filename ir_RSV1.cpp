/*
Assuming the protocol we are adding is for the (imaginary) manufacturer:  Shuzu

Our fantasy protocol is a standard protocol, so we can use this standard
template without too much work. Some protocols are quite unique and will require
considerably more work in this file! It is way beyond the scope of this text to
explain how to reverse engineer "unusual" IR protocols. But, unless you own an
oscilloscope, the starting point is probably to use the rawDump.ino sketch and
try to spot the pattern!

Before you start, make sure the IR library is working OK:
  # Open up the Arduino IDE
  # Load up the rawDump.ino example sketch
  # Run it

Now we can start to add our new protocol...

1. Copy this file to : ir_Shuzu.cpp

2. Replace all occurrences of "Shuzu" with the name of your protocol.

3. Tweak the #defines to suit your protocol.

4. If you're lucky, tweaking the #defines will make the default send() function
   work.

5. Again, if you're lucky, tweaking the #defines will have made the default
   decode() function work.

You have written the code to support your new protocol!

Now you must do a few things to add it to the IRremote system:

1. Open IRremote.h and make the following changes:
   REMEMEBER to change occurences of "SHUZU" with the name of your protocol

   A. At the top, in the section "Supported Protocols", add:
      #define DECODE_SHUZU  1
      #define SEND_SHUZU    1

   B. In the section "enumerated list of all supported formats", add:
      SHUZU,
      to the end of the list (notice there is a comma after the protocol name)

   C. Further down in "Main class for receiving IR", add:
      //......................................................................
      #if DECODE_SHUZU
          bool  decodeShuzu (decode_results *results) ;
      #endif

   D. Further down in "Main class for sending IR", add:
      //......................................................................
      #if SEND_SHUZU
          void  sendShuzu (unsigned long data,  int nbits) ;
      #endif

   E. Save your changes and close the file

2. Now open irRecv.cpp and make the following change:

   A. In the function IRrecv::decode(), add:
      #ifdef DECODE_NEC
          DBG_PRINTLN("Attempting Shuzu decode");
          if (decodeShuzu(results))  return true ;
      #endif

   B. Save your changes and close the file

You will probably want to add your new protocol to the example sketch

3. Open MyDocuments\Arduino\libraries\IRremote\examples\IRrecvDumpV2.ino

   A. In the encoding() function, add:
      case SHUZU:    Serial.print("SHUZU");     break ;

Now open the Arduino IDE, load up the rawDump.ino sketch, and run it.
Hopefully it will compile and upload.
If it doesn't, you've done something wrong. Check your work.
If you can't get it to work - seek help from somewhere.

If you get this far, I will assume you have successfully added your new protocol
There is one last thing to do.

1. Delete this giant instructional comment.

2. Send a copy of your work to us so we can include it in the library and
   others may benefit from your hard work and maybe even write a song about how
   great you are for helping them! :)

Regards,
  BlueChip
*/

#include "IRremote.h"
#include "IRremoteInt.h"

//==============================================================================
//
//
//                              S H U Z U
//
//
//==============================================================================

// Robosapien V1 Remote

#define BITS          8  // The number of bits in the command

#define HDR_MARK 6500
#define HDR_SPACE 3550	// As 1 as well but always in same position
#define BIT_MARK 800
#define ONE_SPACE 3550
#define ZERO_SPACE 950

#define ERR 0
#define DECODED 1

// Utility function to get the bit position number in byte
int getBit(int number, int position) {
    unsigned int bitmask = 1 << position;
   return (number & bitmask) ? 1 : 0;
}

//+=============================================================================
//
#if SEND_RSV1
// Send Robosapien V1 sequence
void IRsend::sendRSV1(unsigned long data)
{

unsigned char b8=(unsigned char)data;
int nbits=BITS;

#ifdef DEBUG

	Serial.print("Sending RS: ");
	Serial.print(b8, BIN);
	Serial.print(" : ");
	Serial.print(" : ");
	Serial.println(b8, DEC);
	
	Serial.print("Left to right decode: ");
	
	for (int i = 0; i < nbits; i++) {
	
		Serial.print(getBit(b8, i));
	
	}
	Serial.println("");
	Serial.println("decoded ");
	  
#endif
    
	// This sequence is quite simple - 8 bits - intial pulse seems to vary a little but does not affect the seq
	// Example m6600 s3550 m750 s950 m800 s950 m800 s950 m850 s3550 m750 s3600 m750 s3600 m750 s950 m800
	
	enableIROut(38);
  
	// mark the inital header sequence
	mark(HDR_MARK);
	space(HDR_SPACE);  
  
	// Loop and get the reverse bit sequence.
	for (int i = nbits-2; i>=0; i--) {
    
		int bit = getBit(b8, i);
		mark(BIT_MARK);
	 
		if ( bit == 1) {
     
			space(ONE_SPACE);
		}
		else {
		
			space(ZERO_SPACE);  
		}
    
	}
      
	// Mark end 
	mark(BIT_MARK);
  
	// Close out
	space(0);
  
}
#endif

//+=============================================================================
//
#if DECODE_RSV1
// Robosapien V1 Remote
//   Decoded pattern from iRecord sketch
//   8 Bit - 18 rawbuff 
//	 Offset 0 - ignore
//	 Offset 1 - Value
//
//Pattern  m6800 s3600 m700 s1000 m750 s1000 m750 s1000 m800 s3600 m700 s950 m800 s1000 m750 s1050 m700
//Offset   1     2     3    4     5    6     7    8     9    10    11   12    13   14    15   16   17

bool IRrecv::decodeRSV1(decode_results *results) {
  unsigned char data = 1;
  int offset = 2; // Skip first space and value marker
  	
  //Serial.print("DecodeRS: ");
  //Serial.println(irparams.rawlen);
  
  // Initial mark - Offset = 2
  if (!MATCH_MARK(results->rawbuf[offset], HDR_SPACE)) {
    return ERR;
  }
   
  // Check raw length valuues
  if (irparams.rawlen < 2 * BITS + 2) {
    return ERR;
  }
  // Initial header space after the variable bit mark
  offset=3;
  //data <<= 1;
  
  for (int i = 0; i < BITS-1; i++) {
  
    if (!MATCH_MARK(results->rawbuf[offset], BIT_MARK)) {
      return ERR;
    }
    offset++;
    if (MATCH_SPACE(results->rawbuf[offset], ONE_SPACE)) {
      data = (data << 1) | 1;
	
    } 
    else if (MATCH_SPACE(results->rawbuf[offset], ZERO_SPACE)) {
      data <<= 1;
	
	  
    } 
    else {
      return ERR;
    }
    offset++;
  }
  // Success
  results->bits = BITS;
  results->value = data;
  results->decode_type = RSV1;
  
  return DECODED;
}

#endif


