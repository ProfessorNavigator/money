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

#include <TradeDay.h>

TradeDay::TradeDay()
{
}

TradeDay::TradeDay(const TradeDay &other)
{
  instrument_id = other.instrument_id;
  board_id = other.board_id;
  date = other.date;
  money_turnover = other.money_turnover;
  turnover = other.turnover;
  first_price = other.first_price;
  last_price = other.last_price;
  wa_price = other.wa_price;
  currency_id = other.currency_id;
  ppm = other.ppm;
  daily_ppm = other.daily_ppm;
  ppm_price = other.ppm_price;
  average_turnover = other.average_turnover;
  average_money_turnover = other.average_money_turnover;
}

TradeDay::TradeDay(TradeDay &&other)
{
  instrument_id = std::move(other.instrument_id);
  board_id = std::move(other.board_id);
  date = std::move(other.date);
  money_turnover = std::move(other.money_turnover);
  turnover = std::move(other.turnover);
  first_price = std::move(other.first_price);
  last_price = std::move(other.last_price);
  wa_price = std::move(other.wa_price);
  currency_id = std::move(other.currency_id);
  ppm = std::move(other.ppm);
  daily_ppm = std::move(other.daily_ppm);
  ppm_price = std::move(other.ppm_price);
  average_turnover = std::move(other.average_turnover);
  average_money_turnover = std::move(other.average_money_turnover);
}

TradeDay &
TradeDay::operator=(const TradeDay &other)
{
  if(this != &other)
    {
      instrument_id = other.instrument_id;
      board_id = other.board_id;
      date = other.date;
      money_turnover = other.money_turnover;
      turnover = other.turnover;
      first_price = other.first_price;
      last_price = other.last_price;
      wa_price = other.wa_price;
      currency_id = other.currency_id;
      ppm = other.ppm;
      daily_ppm = other.daily_ppm;
      ppm_price = other.ppm_price;
      average_turnover = other.average_turnover;
      average_money_turnover = other.average_money_turnover;
    }
  return *this;
}

TradeDay &
TradeDay::operator=(TradeDay &&other)
{
  if(this != &other)
    {
      instrument_id = std::move(other.instrument_id);
      board_id = std::move(other.board_id);
      date = std::move(other.date);
      money_turnover = std::move(other.money_turnover);
      turnover = std::move(other.turnover);
      first_price = std::move(other.first_price);
      last_price = std::move(other.last_price);
      wa_price = std::move(other.wa_price);
      currency_id = std::move(other.currency_id);
      ppm = std::move(other.ppm);
      daily_ppm = std::move(other.daily_ppm);
      ppm_price = std::move(other.ppm_price);
      average_turnover = std::move(other.average_turnover);
      average_money_turnover = std::move(other.average_money_turnover);
    }
  return *this;
}
