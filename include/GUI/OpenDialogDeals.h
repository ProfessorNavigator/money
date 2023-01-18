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

#ifndef OPENDIALOGDEALS_H_
#define OPENDIALOGDEALS_H_

#include <gtkmm.h>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <tuple>
#include <libintl.h>
#include "GraphicWidget.h"
#include "AuxFunc.h"
#include "DownloadMenu.h"

class OpenDialogDeals
{
public:
  OpenDialogDeals(Gtk::Window *mwin);
  virtual
  ~OpenDialogDeals();
  void
  createDialog(int variant, Gdk::Rectangle rec);
private:
  void
  dateSelection(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		Gtk::MenuButton *date_men, Gtk::TreeView *view,
		Gtk::Window *win);
  void
  boardsSelection(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		  Gtk::MenuButton *boards_men, Gtk::TreeView *view,
		  Gtk::Window *win);
  void
  sortSelection(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		Gtk::MenuButton *sort_men, Gtk::TreeView *view,
		Gtk::Window *win);
  void
  instrSelection(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		 Gtk::MenuButton *instr_men, Gtk::TreeView *view,
		 Gtk::Window *win);
  void
  refreshBoards(Gtk::TreeView *tv_boards, Gtk::TreeView *tv_dates,
		Gtk::MenuButton *boards_men, Gtk::Window *win);
  void
  refreshSort(Gtk::TreeView *tv_sort, Gtk::MenuButton *men_sort);
  void
  refreshInstr(Gtk::TreeView *tv_instr, Gtk::TreeView *tv_dates,
	       Gtk::TreeView *tv_boards, Gtk::TreeView *tv_sort,
	       Gtk::MenuButton *men_instr, Gtk::Window *win);
  void
  openFile(Gtk::Window *win, Gdk::Rectangle rec, int variant,
	   Gtk::TreeView *tv_dates, Gtk::TreeView *tv_instr);
  Gtk::Window *Mwin = nullptr;
};

#endif /* OPENDIALOGDEALS_H_ */
