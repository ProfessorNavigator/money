/*
 Copyright 2021-2022 Yury Bobylev <bobilev_yury@mail.ru>

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
#include <glibmm/ustring.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <sigc++/sigc++.h>
#include "AuxFunc.h"

class DownloadDeals
{
public:
  DownloadDeals (
      int *canceling,
      std::vector<std::pair<Glib::ustring, Glib::ustring>> *instruments,
      std::vector<std::pair<Glib::ustring, Glib::ustring>> *instrumentsF,
      std::vector<std::pair<Glib::ustring, Glib::ustring>> *boards,
      int selector);
  virtual
  ~DownloadDeals ();
  void
  downloadAll ();
  sigc::signal<void
  ()> noDeals;
  sigc::signal<void
  ()> netError;
  sigc::signal<void
  ()> canceled;
  sigc::signal<void
  (double)> progr;
  sigc::signal<void
  ()> pulse;
  sigc::signal<void
  ()> finished;
private:
  int
  checkTurnovers ();
  int *Canceling;
  std::vector<std::pair<Glib::ustring, Glib::ustring>> Instruments;
  std::vector<std::pair<Glib::ustring, Glib::ustring>> InstrumentsF;
  std::vector<std::pair<Glib::ustring, Glib::ustring>> Boards;
  int Selector;
};

#endif /* DOWNLOADDEALS_H_ */
