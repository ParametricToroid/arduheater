/**
 * Arduheater - An intelligent dew buster for astronomy
 * Copyright (C) 2016-2017 João Brázio [joao@brazio.org]
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>
#include <stdlib.h>

#include <avr/interrupt.h>

#include "macro.h"
#include "console.h"

class Analog
{
  /**
   * Disable the creation of an instance of this object.
   * This class should be used as a static class.
   */
  private:
     Analog() {;}
    ~Analog() {;}

  public:
    /**
     * @brief TODO
     * @details
     *
     */
    typedef void (*callback_t)(const uint8_t&, const uint16_t&);
    static callback_t s_callback;

  public:
    // Being a bit lazy here, this buffer should be private to the class
    // and have a set of wrappers around it.. adding it to the TODO list.
    static struct buffer_t
    {
      uint8_t  n;
      uint8_t  chan;
      uint16_t raw[128];
    } s_buffer;

  public:
    /**
     * @brief TODO
     * @details
     *
     */
    static void read(const uint8_t&, const callback_t);

    /**
     * @brief TODO
     * @details
     *
     */
    static void setup();
};

#endif
