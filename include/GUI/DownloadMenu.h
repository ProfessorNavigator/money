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

#ifndef INCLUDE_GUI_DOWNLOADMENU_H_
#define INCLUDE_GUI_DOWNLOADMENU_H_

#include <gtkmm.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <string>
#include <libintl.h>
#include "AuxFunc.h"
#include "DownloadAll.h"
#include "CalcIndex.h"
#include "DownloadDeals.h"

class DownloadMenu
{
public:
  DownloadMenu(Gtk::Window *mwin);
  virtual
  ~DownloadMenu();
  void
  downloadMenu();
  std::vector<std::tuple<std::string, std::string>>
  formVectorBoards();
  std::vector<std::tuple<std::string, std::string, std::string>>
  formVectorInstr(int variant);
private:
  void
  instrSelection(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		 Glib::RefPtr<Gtk::ListStore> model, Gtk::MenuButton *mb);
  double
  dateHome();
  double
  dateServer();
  void
  errorMessage();
  void
  finishMessage();
  void
  saveDate(int dt);
  void
  calcIndex();
  void
  downloadAll(Gtk::Window *win);
  void
  downloadDeals(Gtk::Window *win);
  void
  downlodSinglInstrAll(Gtk::Window *win, Gtk::TreeView *tv);
  void
  downlodSinglInstrDeals(Gtk::Window *win, Gtk::TreeView *tv);
  void
  noDeals(int sel);
  void
  splitInstrument(Gtk::Window *win);
  void
  addSplitInstr(Gtk::Window *win, Gtk::TreeView *tv_split);
  Gtk::Window *Mwin = nullptr;
};

#endif /* INCLUDE_GUI_DOWNLOADMENU_H_ */
