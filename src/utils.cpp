/**
 * Arduheater - Heat controller for astronomy usage
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

#include "arduheater.h"

// WARNING: max 12750ms as input
uint8_t utils::msectotick(const uint16_t& ms)
{
  return (ms / HEARTBEAT);
}

float utils::steinhart(const uint16_t& raw)
{
  float ret = constrain(raw, 1, 1022);
  ret  = THERMISTOR_SERIESRESISTOR / (1023.0 / ret - 1);  // convert raw to ohms
  ret  = ret / THERMISTOR_NOMINAL_VAL;                    // (R/Ro)
  ret  = log(ret);                                        // ln(R/Ro)
  ret /= THERMISTOR_BCOEFFICIENT;                         // 1/B * ln(R/Ro)
  ret += 1.0 / (THERMISTOR_NOMINAL_TEMP + 273.15);        // + (1/To)
  ret  = 1.0 / ret;                                       // invert
  ret -= 273.15;                                          // convert to K to C
  return ret;
}
