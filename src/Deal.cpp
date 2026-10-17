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

#include <Deal.h>

Deal::Deal()
{
}

Deal::Deal(const Deal &other)
{
  instrument_id = other.instrument_id;
  board_id = other.board_id;
  deal_time = other.deal_time;
  trade_num = other.trade_num;
  quantity = other.quantity;
  price = other.price;
  money_volume = other.money_volume;
  ppm = other.ppm;
  ppm_price = other.ppm_price;
  average_quan = other.average_quan;
  average_money = other.average_money;
}

Deal::Deal(Deal &&other)
{
  instrument_id = std::move(other.instrument_id);
  board_id = std::move(other.board_id);
  deal_time = std::move(other.deal_time);
  trade_num = std::move(other.trade_num);
  quantity = std::move(other.quantity);
  price = std::move(other.price);
  money_volume = std::move(other.money_volume);
  ppm = std::move(other.ppm);
  ppm_price = std::move(other.ppm_price);
  average_quan = std::move(other.average_quan);
  average_money = std::move(other.average_money);
}

Deal &
Deal::operator=(const Deal &other)
{
  if(this != &other)
    {
      instrument_id = other.instrument_id;
      board_id = other.board_id;
      deal_time = other.deal_time;
      trade_num = other.trade_num;
      quantity = other.quantity;
      price = other.price;
      money_volume = other.money_volume;
      ppm = other.ppm;
      ppm_price = other.ppm_price;
      average_quan = other.average_quan;
      average_money = other.average_money;
    }

  return *this;
}

Deal &
Deal::operator=(Deal &&other)
{
  if(this != &other)
    {
      instrument_id = std::move(other.instrument_id);
      board_id = std::move(other.board_id);
      deal_time = std::move(other.deal_time);
      trade_num = std::move(other.trade_num);
      quantity = std::move(other.quantity);
      price = std::move(other.price);
      money_volume = std::move(other.money_volume);
      ppm = std::move(other.ppm);
      ppm_price = std::move(other.ppm_price);
      average_quan = std::move(other.average_quan);
      average_money = std::move(other.average_money);
    }

  return *this;
}
