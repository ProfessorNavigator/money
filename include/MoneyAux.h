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
#ifndef MONEYAUX_H
#define MONEYAUX_H

#include <filesystem>
#include <string>
#include <vector>

class MoneyAux
{
public:
  MoneyAux();

  static std::vector<std::string>
  detectEncoding(const std::string &src);

  static std::string
  convertToEncodign(const std::string &src, const std::string &from,
                    const std::string &to);

  static double
  dateToJulianDay(int day, int month, int year, int hour, int minut,
                  double sec);

  static void
  julianDateToDate(const double &JDN, int &day, int &month, int &year,
                   int &hour, int &minut, double &second);

  template <typename T>
  static T
  randomNumber();

  static std::filesystem::path
  randomFileName();
};

#endif // MONEYAUX_H
