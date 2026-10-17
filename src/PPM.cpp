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

#include <PPM.h>

PPM::PPM()
{
}

PPM::PPM(const PPM &other)
{
  date = other.date;
  turnover = other.turnover;
  money_turnover = other.money_turnover;
  current_ppm = other.current_ppm;
  total_ppm = other.total_ppm;
}

PPM::PPM(PPM &&other)
{
  date = std::move(other.date);
  turnover = std::move(other.turnover);
  money_turnover = std::move(other.money_turnover);
  current_ppm = std::move(other.current_ppm);
  total_ppm = std::move(other.total_ppm);
}

PPM &
PPM::operator=(const PPM &other)
{
  if(this != &other)
    {
      date = other.date;
      turnover = other.turnover;
      money_turnover = other.money_turnover;
      current_ppm = other.current_ppm;
      total_ppm = other.total_ppm;
    }
  return *this;
}

PPM &
PPM::operator=(PPM &&other)
{
  if(this != &other)
    {
      date = std::move(other.date);
      turnover = std::move(other.turnover);
      money_turnover = std::move(other.money_turnover);
      current_ppm = std::move(other.current_ppm);
      total_ppm = std::move(other.total_ppm);
    }
  return *this;
}

void
PPM::update(const PPM &other)
{
  turnover += other.turnover;
  money_turnover += other.money_turnover;
  current_ppm = turnover / money_turnover;
}

void
PPM::set(const mpf_class &turnover, const mpf_class &money_turnover)
{
  this->turnover = turnover;
  this->money_turnover = money_turnover;
  if(money_turnover != 0.0)
    {
      current_ppm = turnover / money_turnover;
    }
}
