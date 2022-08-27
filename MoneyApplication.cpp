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

#include "MoneyApplication.h"

MoneyApplication::MoneyApplication () :
    Gtk::Application ("ru.mail.bobilev_yury.Money")
{
  // TODO Auto-generated constructor stub

}

MoneyApplication::~MoneyApplication ()
{
  // TODO Auto-generated destructor stub
}

Glib::RefPtr<MoneyApplication>
MoneyApplication::create ()
{
  return Glib::make_refptr_for_instance<MoneyApplication> (
      new MoneyApplication ());
}

MainWindow*
MoneyApplication::create_appwindow ()
{
  MainWindow *mw = new MainWindow;
  this->add_window (*mw);
  mw->signal_hide ().connect ( [mw, this]
  {
    std::vector<Gtk::Window*> wv;
    wv = this->get_windows ();
    for (size_t i = 0; i < wv.size (); i++)
      {
	Gtk::Window *win = wv[i];
	if (win != mw)
	  {
	    win->hide ();
	    delete win;
	  }
      }
    delete mw;
  });

  return mw;
}

void
MoneyApplication::on_activate ()
{
  std::vector<Gtk::Window*> winv;
  winv = this->get_windows ();
  if (winv.size () == 0)
    {
      auto appwin = create_appwindow ();
      appwin->present ();
      Glib::PropertyProxy<bool> pr = this->property_register_session ();
      pr.set_value (true);
    }
}
