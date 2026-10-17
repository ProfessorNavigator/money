/*
 * Copyright (C) 2026 Yury Bobylev <bobilev_yury@mail.ru>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef TRADDAY_H
#define TRADDAY_H

#include <cstdint>
#include <gmpxx.h>
#include <string>

class TradeDay
{
public:
  TradeDay();

  TradeDay(const TradeDay &other);

  TradeDay(TradeDay &&other);

  TradeDay &
  operator=(const TradeDay &other);

  TradeDay &
  operator=(TradeDay &&other);

  std::string instrument_id;
  std::string board_id;
  double date = 0.0;
  double money_turnover = 0.0;
  uint64_t turnover = 0;
  double first_price = 0.0;
  double last_price = 0.0;
  double wa_price = 0.0;
  std::string currency_id;
  mpf_class ppm = 0.0;
  double daily_ppm = 0.0;
  mpf_class ppm_price = 0.0;
  mpf_class average_turnover = 0.0;
  mpf_class average_money_turnover = 0.0;
};

#endif // TRADDAY_H
