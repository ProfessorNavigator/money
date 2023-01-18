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

#ifndef INCLUDE_GUI_ANOMALYSEARCH_H_
#define INCLUDE_GUI_ANOMALYSEARCH_H_

#include <gtkmm.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <thread>
#include <libintl.h>
#include "DownloadMenu.h"
#include "GraphicWidget.h"
#include "AuxFunc.h"

class AnomalySearch
{
public:
  AnomalySearch(Gtk::Window *par_win);
  virtual
  ~AnomalySearch();
  void
  windowFunc();
  void
  searchWin(Gtk::Window *win, Gtk::Entry *day_ent, Gtk::Entry *month_ent,
	    Gtk::Entry *year_ent, Gtk::Entry *sens_ent, Gtk::Entry *depth_ent);
  void
  searchFunc(std::vector<std::tuple<std::string, std::string>> *result,
	     double jd, int depth, int sens, int *cancel);
  void
  errDialog(int variant, Gtk::Window *win);
  void
  resultWin(Gtk::Window *win,
	    std::vector<std::tuple<std::string, std::string>> *result);
private:
  sigc::signal<void
  (double)> signal_progr;
  sigc::signal<void
  ()> signal_serch_compl;

  Gtk::Window *par_win = nullptr;
};

#endif /* INCLUDE_GUI_ANOMALYSEARCH_H_ */
