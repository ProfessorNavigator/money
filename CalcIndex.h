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

#ifndef CALCINDEX_H_
#define CALCINDEX_H_

#include <vector>
#include <gmpxx.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <sigc++/sigc++.h>
#include <glibmm/ustring.h>
#include <tuple>
#include <libintl.h>
#include "AuxFunc.h"

class CalcIndex
{
public:
  CalcIndex (int *canceled);
  virtual
  ~CalcIndex ();
  void
  mainCalc ();
  sigc::signal<void
  (Glib::ustring)> operationName;
  sigc::signal<void
  (double)> progr;
  sigc::signal<void
  ()> allComplet;
  sigc::signal<void
  ()> canceled;
private:
  int *Canceled;
};

#endif /* CALCINDEX_H_ */
