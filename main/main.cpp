// file: main.c or main.cpp
#include "Arduino.h"
#include "src/IRrecv.h"
#include "src/IRsend.h"
#include "IRutils.h"
const uint16_t kRecvPin = 18;

// GPIO to use to control the IR LED circuit. Recommended: 4 (D2).
const uint16_t kIrLedPin = 19;

// The Serial connection baud rate.
const uint32_t kBaudRate = 115200;

// As this program is a special purpose capture/resender, let's use a larger
// than expected buffer so we can handle very large IR messages.
const uint16_t kCaptureBufferSize = 1024; // 1024 == ~511 bits

// kTimeout is the Nr. of milli-Seconds of no-more-data before we consider a
// message ended.
const uint8_t kTimeout = 50; // Milli-Seconds

// kFrequency is the modulation frequency all UNKNOWN messages will be sent at.
const uint16_t kFrequency = 38000; // in Hz. e.g. 38kHz.

// The IR transmitter.
IRsend irsend(kIrLedPin);
// The IR receiver.
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, false);
// Somewhere to store the captured message.
decode_results results;

extern "C" void app_main()
{
  initArduino();

  irrecv.enableIRIn(); // Start up the IR receiver.
  irsend.begin();      // Start up the IR sender.

  Serial.begin(kBaudRate);
  while (!Serial) // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();

  Serial.print("SmartIRRepeater is now running and waiting for IR input "
               "on Pin ");
  Serial.println(kRecvPin);
  Serial.print("and will retransmit it on Pin ");
  Serial.println(kIrLedPin);

  // Arduino-like loop()
  while (true)
  {
    // Serial.println("loop");
    // Check if an IR message has been received.
    if (irrecv.decode(&results))
    { // We have captured something.
      // The capture has stopped at this point.
      decode_type_t protocol = results.decode_type;
      uint16_t size = results.bits;
      bool success = true;
      // Is it a protocol we don't understand?
      if (protocol == decode_type_t::UNKNOWN)
      { // Yes.
        // Convert the results into an array suitable for sendRaw().
        // resultToRawArray() allocates the memory we need for the array.
        uint16_t *raw_array = resultToRawArray(&results);
        // Find out how many elements are in the array.
        size = getCorrectedRawLength(&results);
#if SEND_RAW
        // Send it out via the IR LED circuit.
        irsend.sendRaw(raw_array, size, kFrequency);
#endif // SEND_RAW
        // Deallocate the memory allocated by resultToRawArray().
        delete[] raw_array;
      }
      else if (hasACState(protocol))
      { // Does the message require a state[]?
        // It does, so send with bytes instead.
        success = irsend.send(protocol, results.state, size / 8);
      }
      else
      { // Anything else must be a simple message protocol. ie. <= 64 bits
        success = irsend.send(protocol, results.value, size);
      }
      // Resume capturing IR messages. It was not restarted until after we sent
      // the message so we didn't capture our own message.
      irrecv.resume();

      // Display a crude timestamp & notification.
      uint32_t now = millis();
      Serial.printf(
          "%06lu.%03lu: A %d-bit %s message was %ssuccessfully retransmitted.\n",
          now / 1000, now % 1000, size, typeToString(protocol).c_str(),
          success ? "" : "un");
    }
    yield(); // Or delay(milliseconds); This ensures the ESP doesn't WDT reset.
  }
}