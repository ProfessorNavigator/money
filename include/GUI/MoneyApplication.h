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

#ifndef INCLUDE_GUI_MONEYAPPLICATION_H_
#define INCLUDE_GUI_MONEYAPPLICATION_H_

#include <gtkmm.h>
#include "MainWindow.h"

class MoneyApplication : public Gtk::Application
{
protected:
  MoneyApplication();
public:
  static Glib::RefPtr<MoneyApplication>
  create();
  virtual
  ~MoneyApplication();
private:
  MainWindow*
  create_appwindow();
protected:
  void
  on_activate() override;
};

#endif /* INCLUDE_GUI_MONEYAPPLICATION_H_ */
