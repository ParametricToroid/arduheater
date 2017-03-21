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

// This PID lib is heavly based on the Arduino's PID library by Brett and his
// excelent tutorial published at http://brettbeauregard.com, thus some code
// is (c) 2004 Brett Beauregard <br3ttb@gmail.com>

#include "arduheater.h"

void pid::autotune() {
  serial::println::PGM(PSTR("PID autotune start"));

  bool running = m_running;
  if (running) m_running = false;

  millis_t t0 = 0;
  millis_t t1 = 0;
  millis_t peak1 = 0;
  millis_t peak2 = 0;

  float oStep = 30;
  float noiseBand = 0.5;

  float lastInputs[101];
  float peaks[10];

  // assuming 10sec lookback
  int16_t nLookBack = 40;
  int16_t sampleTime = 250;

  int16_t peakType = 0;
  int16_t peakCount = 0;
  bool justchanged = false;
  float absMax = m_input;
  float absMin = m_input;
  float setpoint = m_input;
  float outputStart = m_output;

  output(outputStart + oStep);

  for (;;) {
    if( peakCount > 9) {
      serial::println::PGM(PSTR("PID autotune timeout"));
      if (running) m_running = true;
      output(outputStart);
      return;
    }

    millis_t now = millis();

    if (now > t1) {
      t1 = now + 1000L;
      cmd::status();
    }

    if (now > t0) {
      t0 = now + sampleTime;
      //serial::print::PGM(PSTR("."));

      float refVal = m_input;

      if (refVal > absMax) { absMax = refVal; }
      if (refVal < absMin) { absMin = refVal; }

      //oscillate the output base on the input's relation to the setpoint
      if (refVal > setpoint + noiseBand) { output(outputStart - oStep); }
      else if (refVal < setpoint - noiseBand) {output(outputStart + oStep); }

      bool isMax = true;
      bool isMin = true;

      //id peaks
      for (int i = nLookBack - 1; i >= 0; i--) {
        float val = lastInputs[i];
        if (isMax) { isMax = (refVal > val); }
        if (isMin) { isMin = (refVal < val); }
        lastInputs[i+1] = lastInputs[i];
      }

      lastInputs[0] = refVal;

      if (nLookBack > 9) {
        if (isMax) {
          if (peakType == 0) { peakType = 1; }
          else if (peakType == -1) {
            peakType = 1;
            justchanged = true;
            peak2 = peak1;
          }

          peak1 = now;
          peaks[peakCount] = refVal;
        } else if (isMin) {
          if (peakType == 0) { peakType=-1; }
          else if (peakType == 1) {
            peakType = -1;
            peakCount++;
            justchanged = true;
          }

          if (peakCount < 10) peaks[peakCount] = refVal;
        }

        //we've transitioned.  check if we can autotune based on the last peaks
        if (justchanged && peakCount > 2) {
          float avgSeparation = (abs(peaks[peakCount-1] - peaks[peakCount-2])
            + abs(peaks[peakCount-2] - peaks[peakCount-3])) / 2.0;

          if (avgSeparation < 0.05 * (absMax-absMin)) {
            serial::println::PGM(PSTR("PID autotune finished"));

            output(outputStart);
            if (running) m_running = true;

            float Ku = 4 * (2 * oStep) / ((absMax-absMin) * M_PI);
            float Pu = (float) (peak1 - peak2) / 1000.0;

            //serial::print::pair::float32(PSTR("Ku"), Ku, 2);
            //serial::print::pair::float32(PSTR("Pu"), Pu, 2);

            float Wp = 0.6 * Ku;
            float Wi = 1.2 * Ku / Pu;
            float Wd = 0.075 * Ku * Pu;

            serial::print::pair::float32(PSTR("Wp"), Wp, 2);
            serial::print::pair::float32(PSTR("Wi"), Wi, 2);
            serial::print::pair::float32(PSTR("Wd"), Wd, 2);
            return;
          }
        }

        justchanged = false;
      }
    }
  }
}

void pid::irq(const bool& reset) {
  if (! m_running) { return; }

  static float s_last_input = 0;            // input value from last cycle
  float error  = m_setpoint - m_input;      // calculate current error
  float dInput = m_input - s_last_input;    // calculate input derivative

  static float s_error = 0;         // integration of error from 0 to present
  s_error += m_Ki * (error * m_dt); // adding the Ki term at this point will
                                    // allow a smooth curve when tuning while
                                    // running.

  if (reset) { s_error = m_output; dInput = 0; }  // reboot the algorithm

  if (s_error > m_max) { s_error = m_max; }       // cap the I term between
  else if (s_error < m_min) { s_error = m_min; }  // min and max values

  // evaluate the PID algorithm
  float u = (m_Kp * error) + s_error - (m_Kd * (dInput / m_dt));

  if (u > m_max) { u = m_max; }       // cap the output between
  else if (u < m_min) { u = m_min; }  // min and max values

  m_output = u;
  s_last_input = m_input;
}

void pid::limit(const float& min, const float& max) {
  if (min > max) { return; }
  m_min = min;
  m_max = max;
}

void pid::output(const float& lhs) {
  m_output = lhs;
  m_output = constrain(m_output, m_min, m_max);
}

void pid::tune(const float& Np, const float& Ni, const float& Nd) {
  // To use the PID into an inverted state i.e. output decreases with the
  // increase of error just provide negative paramaters.
  Kp(Np); Ki(Ni); Kd(Nd);
}
