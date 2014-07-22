

#include "dht22.h"
#include <util/delay.h>

#define DHT_Read_Pin(pint,pin)    bit_is_set(pint,pin) // Read the PIN
// This should be 40, but the sensor is adding an extra bit at the start
#define DHT22_DATA_BIT_COUNT 41

DHT22::DHT22(int pin, volatile uint8_t &port, volatile uint8_t &ddr, volatile uint8_t &pint)
: _pin(pin), _port(port), _ddr(ddr), _pint(pint) {
    _lastHumidity = DHT22_ERROR_VALUE;
    _lastTemperature = DHT22_ERROR_VALUE;
    /* Set PIN to HIGH*/
    cli();
    _ddr |= (1 << _pin); //Set pin Output
    _port |= (1 << _pin); //Pin High
    _delay_ms(250); //Wait for 250mS
    sei();
}


//
// Read the 40 bit data stream from the DHT 22
// Store the results in private member data to be read by public member functions
//

DHT22_ERROR_t DHT22::read_data() {
    uint8_t retryCount;
    uint8_t bitTimes[DHT22_DATA_BIT_COUNT];
    int currentHumidity;
    int currentTemperature;
    uint8_t checkSum, csPart1, csPart2, csPart3, csPart4;
    int i;
    currentHumidity = 0;
    currentTemperature = 0;
    checkSum = 0;
    //currentTime = millis();
    for (i = 0; i < DHT22_DATA_BIT_COUNT; i++) {
        bitTimes[i] = 0;
    }
    /*TODO: FIXME, or REFACTOREME using NTP
        if (currentTime - _lastReadTime < 2000) {
            // Caller needs to wait 2 seconds between each call to readData
            return DHT_ERROR_TOOQUICK;
        }
        _lastReadTime = currentTime;
     */
    /* Start the communication*/
    cli();
    _ddr |= (1 << _pin); //Set pin Output
    _port |= (1 << _pin); //Pin High
    _delay_ms(250); //Wait for 250mS
    sei();
    retryCount = 0;
    do {
        if (retryCount > 125) {
            _lasterror = DHT_BUS_HUNG;
            return DHT_BUS_HUNG;
        }
        retryCount++;
        _delay_us(2);
    } while (!DHT_Read_Pin(_pint, _pin));
    // Send the activate pulse
    cli();
    _port &= ~(1 << _pin); //Pin Low
    _delay_ms(10); //20ms Low 
    _port |= (1 << _pin); //Pin High
    sei();
    /* Set input, and wait for the sensor's response*/
    cli();
    _ddr &= ~(1 << _pin); //Set pin Input
    sei();
    // Find the start of the ACK Pulse
    retryCount = 0;
    do {
        if (retryCount > 25) //(Spec is 20 to 40 us, 25*2 == 50 us)
        {
            _lasterror = DHT_ERROR_NOT_PRESENT;
            return DHT_ERROR_NOT_PRESENT;
        }
        retryCount++;
        _delay_us(2);
    } while (!DHT_Read_Pin(_pint, _pin));
    // Find the end of the ACK Pulse
    retryCount = 0;
    do {
        if (retryCount > 60) //(Spec is 80 us, 50*2 == 100 us)
        {
            _lasterror = DHT_ERROR_ACK_TOO_LONG;
            return DHT_ERROR_ACK_TOO_LONG;
        }
        retryCount++;
        _delay_us(2);
    } while (DHT_Read_Pin(_pint, _pin));
    // Read the 40 bit data stream
    for (i = 0; i < DHT22_DATA_BIT_COUNT; i++) {
        // Find the start of the sync pulse
        retryCount = 0;
        do {
            if (retryCount > 35) //(Spec is 50 us, 35*2 == 70 us)
            {
                _lasterror = DHT_ERROR_SYNC_TIMEOUT;
                return DHT_ERROR_SYNC_TIMEOUT;
            }
            retryCount++;
            _delay_us(2);
        } while (!DHT_Read_Pin(_pint, _pin));
        // Measure the width of the data pulse
        retryCount = 0;
        do {
            if (retryCount > 50) //(Spec is 80 us, 50*2 == 100 us)
            {
                _lasterror = DHT_ERROR_DATA_TIMEOUT;
                return DHT_ERROR_DATA_TIMEOUT;
            }
            retryCount++;
            _delay_us(2);
        } while (DHT_Read_Pin(_pint, _pin));
        bitTimes[i] = retryCount;
    }
    // Now bitTimes have the number of retries (us *2)
    // that were needed to find the end of each data bit
    // Spec: 0 is 26 to 28 us
    // Spec: 1 is 70 us
    // bitTimes[x] <= 11 is a 0
    // bitTimes[x] >  11 is a 1
    // Note: the bits are offset by one from the data sheet, not sure why
    for (i = 0; i < 16; i++) {
        if (bitTimes[i + 1] > 11) {
            currentHumidity |= (1 << (15 - i));
        }
    }
    for (i = 0; i < 16; i++) {
        if (bitTimes[i + 17] > 11) {
            currentTemperature |= (1 << (15 - i));
        }
    }
    for (i = 0; i < 8; i++) {
        if (bitTimes[i + 33] > 11) {
            checkSum |= (1 << (7 - i));
        }
    }

    _lastHumidity = currentHumidity & 0x7FFF;
    if (currentTemperature & 0x8000) {
        // Below zero, non standard way of encoding negative numbers!
        // Convert to native negative format.
        _lastTemperature = -currentTemperature & 0x7FFF;
    } else {
        _lastTemperature = currentTemperature;
    }

    csPart1 = currentHumidity >> 8;
    csPart2 = currentHumidity & 0xFF;
    csPart3 = currentTemperature >> 8;
    csPart4 = currentTemperature & 0xFF;
    if (checkSum == ((csPart1 + csPart2 + csPart3 + csPart4) & 0xFF)) {
        _lasterror = DHT_ERROR_NONE;
        return DHT_ERROR_NONE;
    }
    _lasterror = DHT_ERROR_CHECKSUM;
    return DHT_ERROR_CHECKSUM;
}

// Report the humidity in .1 percent increments, such that 635 means 63.5% relative humidity
//
// Converts from the internal integer format on demand, so you might want
// to cache the result.

short int DHT22::get_humidity_int() {
    return _lastHumidity;
}

// Get the temperature in decidegrees C, such that 326 means 32.6 degrees C.
// The temperature may be negative, so be careful when handling the fractional part.

short int DHT22::get_temperature_c_int() {
    return _lastTemperature;
}

#if !defined(DHT22_NO_FLOAT)
// Return the percentage relative humidity in decimal form

inline float DHT22::get_humidity() {
    return float(_lastHumidity) / 10;
}
#endif
#if !defined(DHT22_NO_FLOAT)
// Return the percentage relative humidity in decimal form
//
// Converts from the internal integer format on demand, so you might want
// to cache the result.

inline float DHT22::get_temperature_c() {
    return float(_lastTemperature) / 10;
}
#endif //DHT22_SUPPORT_FLOAT

//
// This is used when the millis clock rolls over to zero
//TODO:FIXME

void DHT22::clock_reset() {
    //_lastReadTime = millis();
}

