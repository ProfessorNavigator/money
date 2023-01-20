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

#ifndef INCLUDE_GUI_OPENDIALOG_H_
#define INCLUDE_GUI_OPENDIALOG_H_
#include <gtkmm.h>
#include <string>
#include <fstream>
#include <iostream>
#include <libintl.h>
#include "DownloadMenu.h"
#include "GraphicWidget.h"
#include "AuxFunc.h"

class OpenDialog
{
public:
  OpenDialog(Gtk::Window *mwin);
  virtual
  ~OpenDialog();
  void
  creatDialogAll(int variant, Gdk::Rectangle rec);
private:
  void
  boardSelection(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		 Glib::RefPtr<Gtk::TreeModel> model, Gtk::MenuButton *men_b);
  void
  instrSelection(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		 Glib::RefPtr<Gtk::TreeModel> model,
		 Gtk::MenuButton *men_instr);
  void
  refreshInstr(Gtk::TreeView *tv_instr, Gtk::TreeView *tv_boards,
	       Gtk::MenuButton *men_instr);
  void
  plotDiagram(Gtk::Window *win, Gtk::TreeView *tv_boards,
	      Gtk::TreeView *tv_instr, int variant, Gdk::Rectangle rec);
  Gtk::Window *Mwin;
};

#endif /* INCLUDE_GUI_OPENDIALOG_H_ */
