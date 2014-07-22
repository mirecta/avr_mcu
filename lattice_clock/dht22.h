#ifndef _DHT22_H_
#define _DHT22_H_
/*
 DHT22 library for ATMEGA328p/AVR-GCC based on
 
  DHT22.cpp - DHT22 sensor library
  Developed by Ben Adams - 2011

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */
#include <inttypes.h>
extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
}
#define DHT22_ERROR_VALUE -995

typedef enum {
    DHT_ERROR_NONE = 0,
    DHT_BUS_HUNG = 1,
    DHT_ERROR_NOT_PRESENT = 2,
    DHT_ERROR_ACK_TOO_LONG = 3,
    DHT_ERROR_SYNC_TIMEOUT = 4,
    DHT_ERROR_DATA_TIMEOUT = 5,
    DHT_ERROR_CHECKSUM = 6,
    DHT_ERROR_TOOQUICK = 7
} DHT22_ERROR_t;

class DHT22 {
private:
    short int _lastHumidity;
    short int _lastTemperature;
    int _pin;
    volatile uint8_t &_port, &_ddr, &_pint;
    DHT22_ERROR_t _lasterror;
public:
    DHT22(int pin, volatile uint8_t &port, volatile uint8_t &ddr, volatile uint8_t &pint);
    DHT22_ERROR_t read_data();
    short int get_humidity_int();
    short int get_temperature_c_int();
    void clock_reset();
#if !defined(DHT22_NO_FLOAT)
    float get_humidity();
    float get_temperature_c();

#endif
};



#endif /*_DHT22_H_*/
