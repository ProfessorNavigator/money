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

#ifndef INCLUDE_DOWNLOAD_DOWNLOADTECHNICAL_H_
#define INCLUDE_DOWNLOAD_DOWNLOADTECHNICAL_H_

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include "AuxFunc.h"

class DownloadTechnical
{
public:
  DownloadTechnical(int *cancel);
  virtual
  ~DownloadTechnical();
  std::function<void
  (int)> techdwnld;
  void
  downloadTechnical();

private:
  int
  downloadBoards();
  int
  downloadListF();
  int
  downloadList();
  int
  availibaleDates();
  int
  availibaleDatesF();

  int *cancel = nullptr;
};

#endif /* INCLUDE_DOWNLOAD_DOWNLOADTECHNICAL_H_ */
