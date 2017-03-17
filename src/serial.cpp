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

// RX/TX buffer structure
serial_buffer_t serial::buffer;

bool serial::available() {
  return (! serial::buffer.rx.empty());
}

uint8_t serial::read() {
  return (buffer.rx.empty()) ? SERIAL_NO_DATA : buffer.rx.dequeue();
}

void serial::write(const uint8_t& c) {
  // wait until there is space in the buffer
  while (!buffer.tx.enqueue(c)) {
    // at this point interrupts are disabled so we need to
    // manually poll the data register empty flag
    if (bit_is_clear(SREG, SREG_I)) {
      if (bit_is_set(UCSR0A, UDRE0)) {
        // send a byte from the buffer
        UDR0    = buffer.tx.dequeue();
        UCSR0A |= bit(TXC0);

        // turn off Data Register Empty Interrupt
        // to stop tx-streaming if this concludes the transfer
        if (buffer.tx.empty()) { UCSR0B &= ~bit(UDRIE0); }
      }
    }
  }

  // Enable Data Register Empty Interrupt
  // to make sure tx-streaming is running
  UCSR0B |= bit(UDRIE0);
}


void serial::banner() {
  serial::println::PGM(PSTR("Arduheater " ARDUHEATER_VERSION " ['$' for help]"));
}

void serial::process() {
  static size_t pos = 0;
  static char   buffer[COMMAND_BUFFER_SIZE];

  while(serial::available()) {
    char c = serial::read();
    switch(c) {
      case '\r':
      case '\n':
        if (!pos) break;

        buffer[pos] = 0x00; // null byte

        serial::print::string("ok: ");
        serial::println::string(buffer);

        memset(&buffer, 0, COMMAND_BUFFER_SIZE);
        pos = 0;
        break;

      default:
        buffer[pos] = c;
        pos = (pos + 1) % COMMAND_BUFFER_SIZE;
        break;
    }
  }
}
