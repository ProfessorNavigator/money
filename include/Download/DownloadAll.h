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

#ifndef DOWNLOADALL_H_
#define DOWNLOADALL_H_

#include <filesystem>
#include <gmpxx.h>
#include <vector>
#include <functional>
#include <string>
#include <tuple>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include "AuxFunc.h"

class DownloadAll
{
public:
  DownloadAll(
      int *cancel,
      std::vector<std::tuple<std::string, std::string, std::string>> *instruments,
      std::vector<std::tuple<std::string, std::string, std::string>> *instrumentsF,
      std::vector<std::tuple<std::string, std::string>> *boards, double db);
  virtual
  ~DownloadAll();
  void
  downloadAll();
  std::function<void
  ()> globError;
  std::function<void
  (double)> progress;
  std::function<void
  ()> pulse;
  std::function<void
  ()> canceled;
  std::function<void
  ()> netError;
  std::function<void
  (int)> saveDate;
  std::function<void
  ()> finished;
private:
  void
  saveRes(std::vector<std::string> &source, std::string instrname,
	  std::string boardid, std::tuple<int, int, int, int, int> tupforsave);
  int *Cancel;
  double Db;
  std::vector<std::tuple<std::string, std::string, std::string>> Instruments;
  std::vector<std::tuple<std::string, std::string, std::string>> InstrumentsF;
  std::vector<std::tuple<std::string, std::string>> Boards;
};

#endif /* DOWNLOADALL_H_ */
