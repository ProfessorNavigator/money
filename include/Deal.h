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
#ifndef DEAL_H
#define DEAL_H

#include <cstdint>
#include <gmpxx.h>
#include <string>

class Deal
{
public:
  Deal();

  Deal(const Deal &other);

  Deal(Deal &&other);

  Deal &
  operator=(const Deal &other);

  Deal &
  operator=(Deal &&other);

  std::string instrument_id;
  std::string board_id;
  double deal_time = 0.0;
  uint64_t trade_num = 0;
  uint64_t quantity = 0;
  double price = 0.0;
  double money_volume = 0.0;
  mpf_class ppm = 0.0;
  mpf_class ppm_price = 0.0;
  mpf_class average_quan = 0.0;
  mpf_class average_money = 0.0;
};

#endif // DEAL_H
