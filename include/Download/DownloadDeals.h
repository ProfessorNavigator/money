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

#ifndef DOWNLOADDEALS_H_
#define DOWNLOADDEALS_H_

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <functional>
#include "AuxFunc.h"

class DownloadDeals
{
  public:
    DownloadDeals(
      int *canceling,
      std::vector<std::tuple<std::string, std::string, std::string>> *instruments,
      std::vector<std::tuple<std::string, std::string, std::string>> *instrumentsF,
      std::vector<std::tuple<std::string, std::string>> *boards,
      int selector);
    virtual
    ~DownloadDeals();
    void
    downloadAll();
    std::function<void
    ()> noDeals;
    std::function<void
    ()> netError;
    std::function<void
    ()> canceled;
    std::function<void
    (double)> progr;
    std::function<void
    ()> pulse;
    std::function<void
    ()> finished;
  private:
    int
    checkTurnovers();
    int *Canceling;
    std::vector<std::tuple<std::string, std::string, std::string>> Instruments;
    std::vector<std::tuple<std::string, std::string, std::string>> InstrumentsF;
    std::vector<std::tuple<std::string, std::string>> Boards;
    int Selector;
};

#endif /* DOWNLOADDEALS_H_ */
