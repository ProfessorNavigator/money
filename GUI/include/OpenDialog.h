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

#ifndef OPENDIALOG_H_
#define OPENDIALOG_H_
#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <string>
#include <fstream>
#include <iostream>
#include <libintl.h>
#include "AuxFunc.h"

class OpenDialog
{
public:
  OpenDialog (Glib::RefPtr<Gtk::Application> app, Gtk::Window *mwin);
  virtual
  ~OpenDialog ();
  sigc::signal<void
  (std::string)> allfilepath;
  sigc::signal<void
  ()> deleteSign;
private:
  void
  creatDialogAll ();
  void
  expandScroll (Gtk::Expander *exp, Gtk::Window *window, Gtk::Grid *grid);
  void
  boardSelection (const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		  Glib::RefPtr<Gtk::ListStore> model,
		  Gtk::TreeModelColumn<unsigned int> id, Gtk::Expander *exp);
  void
  instrSelection (const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column,
		  Gtk::Expander *instr, Gtk::TreeView *boxi);
  void
  refreshInstr (Gtk::TreeView *boxi, Gtk::TreeViewColumn *columni,
		Gtk::Expander *instr);
  void
  emitallFilepath ();
  std::vector<std::pair<Glib::ustring, Glib::ustring>> boards;
  size_t selectedboard;
  std::vector<std::string> instruments;
  size_t instrsel = 0;
  sigc::signal<void
  ()> refreshi;
  Gtk::TreeModelColumn<unsigned int> idii;
  Gtk::TreeModelColumn<Glib::ustring> instrumentii;
  Glib::RefPtr<Gtk::CssProvider> css_provider;
  Glib::RefPtr<Gtk::Application> App;
  Gtk::Window *Mwin;
};

#endif /* OPENDIALOG_H_ */
