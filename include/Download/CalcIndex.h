/*
 Copyright 2021-2023 Yury Bobylev <bobilev_yury@mail.ru>

 This file is part of Money.
 Money is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation, either version 3 of
 the License, or (at your option) any later version.
 Money is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with Money. If not,
 see <https://www.gnu.org/licenses/>.
 */

#ifndef INCLUDE_DOWNLOAD_CALCINDEX_H_
#define INCLUDE_DOWNLOAD_CALCINDEX_H_

#include <vector>
#include <gmpxx.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <functional>
#include <tuple>
#include <libintl.h>
#include "AuxFunc.h"

class CalcIndex
{
public:
  CalcIndex(int *canceled);
  virtual
  ~CalcIndex();
  void
  mainCalc();
  std::function<void
  (std::string)> operationName;
  std::function<void
  (double)> progr;
  std::function<void
  ()> allComplet;
  std::function<void
  ()> canceled;
private:
  int *Canceled;
};

#endif /* INCLUDE_DOWNLOAD_CALCINDEX_H_ */
