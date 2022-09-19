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

#ifndef DOWNLOADALL_H_
#define DOWNLOADALL_H_

#include <filesystem>
#include <gmpxx.h>
#include <vector>
#include <glibmm/ustring.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sigc++/sigc++.h>
#include <unistd.h>
#include "AuxFunc.h"

class DownloadAll
{
public:
  DownloadAll (
      int *cancel,
      std::vector<std::pair<Glib::ustring, Glib::ustring>> *instruments,
      std::vector<std::pair<Glib::ustring, Glib::ustring>> *instrumentsF,
      std::vector<std::pair<Glib::ustring, Glib::ustring>> *boards, int db);
  virtual
  ~DownloadAll ();
  void
  downloadAll ();
  sigc::signal<void
  ()> globError;
  sigc::signal<void
  (double)> progress;
  sigc::signal<void
  ()> pulse;
  sigc::signal<void
  ()> canceled;
  sigc::signal<void
  ()> netError;
  sigc::signal<void
  (int)> saveDate;
  sigc::signal<void
  ()> finished;
private:
  void
  saveRes (std::vector<std::string> &source, Glib::ustring instrname,
	   Glib::ustring boardid, std::tuple<int, int, int, int, int> tupforsave);
  int *Cancel;
  int Db;
  std::vector<std::pair<Glib::ustring, Glib::ustring>> Instruments;
  std::vector<std::pair<Glib::ustring, Glib::ustring>> InstrumentsF;
  std::vector<std::pair<Glib::ustring, Glib::ustring>> Boards;
};

#endif /* DOWNLOADALL_H_ */
