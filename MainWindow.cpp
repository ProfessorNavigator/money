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

#include "MainWindow.h"

MainWindow::MainWindow ()
{
  css_provider = Gtk::CssProvider::create ();
  AuxFunc af;
  std::filesystem::path p = std::filesystem::u8path (af.get_selfpath ());
  css_provider->load_from_path (
      Glib::ustring (
	  std::string (p.parent_path ().u8string ())
	      + "/../share/Money/mainWindow.css"));
  windowFunc ();
}

MainWindow::~MainWindow ()
{
  if (dispv != nullptr)
    {
      delete dispv;
    }
}

void
MainWindow::windowFunc ()
{
  std::string pathto;
  AuxFunc af;
  af.homePath (&pathto);
  pathto = pathto + "/.Money";
  std::filesystem::path path = std::filesystem::u8path (pathto);
  if (!std::filesystem::exists (path))
    {
      std::filesystem::create_directories (path);
    }
  pathto = path.u8string () + "/DateBegin";
  path = std::filesystem::u8path (pathto);
  if (!std::filesystem::exists (path))
    {
      std::fstream file;
      file.open (path, std::ios_base::out);
      std::string date = std::to_string (af.grigtojulian (23, 3, 1997));
      file.write (date.c_str (), date.size ());
      file.close ();
    }
  pathto = std::filesystem::temp_directory_path ().u8string ();
#ifdef __linux
  pathto = pathto + "/Money";
#endif
#ifdef _WIN32
  pathto = pathto + "Money";
#endif
  path = std::filesystem::u8path (pathto);
  if (std::filesystem::exists (path))
    {
      std::filesystem::remove_all (path);
    }

  this->set_title ("Money");
  this->set_name ("mainWindow");
  get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);

  auto grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_halign (Gtk::Align::CENTER);
  grid->set_margin (10);
  this->set_child (*grid);

  Gtk::Button *priceall = Gtk::make_managed<Gtk::Button> (gettext ("Price"));
  priceall->set_name ("pricallButton");
  priceall->set_margin (5);
  priceall->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  priceall->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::plotPriceall));
  grid->attach (*priceall, 0, 0);

  Gtk::Button *moneyprice = Gtk::make_managed<Gtk::Button> (
      gettext ("Purchasing power of money"));
  moneyprice->set_name ("moneypriceButton");
  moneyprice->set_margin (5);
  moneyprice->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  moneyprice->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::plotPSDall));
  grid->attach (*moneyprice, 0, 1);

  Gtk::Button *moneyvolume = Gtk::make_managed<Gtk::Button> (
      gettext ("Share's turnover"));
  moneyvolume->set_name ("moneyvolumeButton");
  moneyvolume->set_margin (5);
  moneyvolume->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  moneyvolume->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::plotVolumeAll));
  grid->attach (*moneyvolume, 0, 2);

  Gtk::Button *pricedeals = Gtk::make_managed<Gtk::Button> (
      gettext ("Prices for transactions"));
  pricedeals->set_name ("pricedealsButton");
  pricedeals->set_margin (5);
  pricedeals->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  pricedeals->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::plotPriceDeals));
  grid->attach (*pricedeals, 0, 3);

  Gtk::Button *dealsmoneypr = Gtk::make_managed<Gtk::Button> (
      gettext ("PPm for transactions"));
  dealsmoneypr->set_name ("dealsmoneyprButton");
  dealsmoneypr->set_margin (5);
  dealsmoneypr->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  dealsmoneypr->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::plotPSDDeals));
  grid->attach (*dealsmoneypr, 0, 4);

  Gtk::Button *dealsvolume = Gtk::make_managed<Gtk::Button> (
      gettext ("Share's turnover for transactions"));
  dealsvolume->set_name ("dealsvolumeButton");
  dealsvolume->set_margin (5);
  dealsvolume->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  dealsvolume->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::plotVolumeDeals));
  grid->attach (*dealsvolume, 0, 5);

  Gtk::Button *commongraphicall = Gtk::make_managed<Gtk::Button> (
      gettext ("Prediction plots (common)"));
  commongraphicall->set_name ("commongraphicallButton");
  commongraphicall->set_margin (5);
  commongraphicall->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  commongraphicall->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::plotAllCommon));
  grid->attach (*commongraphicall, 0, 6);

  Gtk::Button *commongraphicdeals = Gtk::make_managed<Gtk::Button> (
      gettext ("Prediction plots (daily)"));
  commongraphicdeals->set_name ("commongraphicdealsButton");
  commongraphicdeals->set_margin (5);
  commongraphicdeals->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  commongraphicdeals->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::plotDealsCommon));
  grid->attach (*commongraphicdeals, 0, 7);

  Gtk::Button *psd = Gtk::make_managed<Gtk::Button> (gettext ("PPm global"));
  psd->set_name ("psdButton");
  psd->set_margin (5);
  psd->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  psd->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::plotPSDGlobal));
  grid->attach (*psd, 0, 8);

  Gtk::Button *volumetotal = Gtk::make_managed<Gtk::Button> (
      gettext ("Shares' turnover global"));
  volumetotal->set_name ("volumetotalButton");
  volumetotal->set_margin (5);
  volumetotal->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  volumetotal->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::plotVolumeGlobal));
  grid->attach (*volumetotal, 0, 9);

  Gtk::Button *moneytotal = Gtk::make_managed<Gtk::Button> (
      gettext ("Money turnover global"));
  moneytotal->set_name ("moneytotalButton");
  moneytotal->set_margin (5);
  moneytotal->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  moneytotal->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::plotMoneyGlobal));
  grid->attach (*moneytotal, 0, 10);

  Gtk::Button *commonpsd = Gtk::make_managed<Gtk::Button> (
      gettext ("PPm common plots"));
  commonpsd->set_name ("commonpsdButton");
  commonpsd->set_margin (5);
  commonpsd->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  commonpsd->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::plotGlobalCommon));
  grid->attach (*commonpsd, 0, 11);

  Gtk::Button *download = Gtk::make_managed<Gtk::Button> (
      gettext ("Download data"));
  download->set_name ("downloadButton");
  download->set_margin (5);
  download->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  download->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::downloadMenu));
  grid->attach (*download, 0, 12);

  Gtk::Button *aboutpr = Gtk::make_managed<Gtk::Button> (gettext ("About"));
  aboutpr->set_name ("aboutprButton");
  aboutpr->set_margin (5);
  aboutpr->get_style_context ()->add_provider (
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  aboutpr->signal_clicked ().connect (
      sigc::mem_fun (*this, &MainWindow::aboutProg));
  grid->attach (*aboutpr, 0, 13);
  this->signal_close_request ().connect ( [this, &af]
  {
    std::vector<Gtk::Window*> win = this->get_application ()->get_windows ();
    for (size_t i = 0; i < win.size (); i++)
      {
	Gtk::Window *wind = win[i];
	if (wind != nullptr && wind != this)
	  {
	    wind->hide ();
	    delete wind;
	  }
      }
    this->hide ();
    std::string fp;
    fp = std::filesystem::temp_directory_path ().u8string ();
#ifdef __linux
					   fp = fp + "/Money";
#endif
#ifdef _WIN32
                                           fp = fp + "Money";
#endif
					   std::filesystem::path path =
					       std::filesystem::u8path (fp);
					   if (std::filesystem::exists (path))
					     {
					       std::filesystem::remove_all (path);
}
    return true;
  },
  false);
}

void
MainWindow::plotPriceall ()
{
  Gdk::Rectangle req = screenRes ();
  GraphicWidget *gr = new GraphicWidget (this->get_application (),
					 req.get_width (), req.get_height (),
					 this);
  gr->class_finished.connect ( [gr]
  {
    delete gr;
  });
  gr->plot (0, 0);
}

void
MainWindow::plotPSDall ()
{
  Gdk::Rectangle req = screenRes ();
  GraphicWidget *gr = new GraphicWidget (this->get_application (),
					 req.get_width (), req.get_height (),
					 this);
  gr->class_finished.connect ( [gr]
  {
    delete gr;
  });
  gr->plot (0, 1);
}

void
MainWindow::plotVolumeAll ()
{
  Gdk::Rectangle req = screenRes ();
  GraphicWidget *gr = new GraphicWidget (this->get_application (),
					 req.get_width (), req.get_height (),
					 this);
  gr->class_finished.connect ( [gr]
  {
    delete gr;
  });
  gr->plot (0, 2);
}

void
MainWindow::plotPriceDeals ()
{
  Gdk::Rectangle req = screenRes ();
  GraphicWidget *gr = new GraphicWidget (this->get_application (),
					 req.get_width (), req.get_height (),
					 this);
  gr->class_finished.connect ( [gr]
  {
    delete gr;
  });
  gr->plot (1, 3);
}

void
MainWindow::plotPSDDeals ()
{
  Gdk::Rectangle req = screenRes ();
  GraphicWidget *gr = new GraphicWidget (this->get_application (),
					 req.get_width (), req.get_height (),
					 this);
  gr->class_finished.connect ( [gr]
  {
    delete gr;
  });
  gr->plot (1, 4);
}

void
MainWindow::plotVolumeDeals ()
{
  Gdk::Rectangle req = screenRes ();
  GraphicWidget *gr = new GraphicWidget (this->get_application (),
					 req.get_width (), req.get_height (),
					 this);
  gr->class_finished.connect ( [gr]
  {
    delete gr;
  });
  gr->plot (1, 5);
}

void
MainWindow::plotAllCommon ()
{
  Gdk::Rectangle req = screenRes ();
  GraphicWidget *gr = new GraphicWidget (this->get_application (),
					 req.get_width (), req.get_height (),
					 this);
  gr->class_finished.connect ( [gr]
  {
    delete gr;
  });
  gr->plot (0, 6);
}

void
MainWindow::plotDealsCommon ()
{
  Gdk::Rectangle req = screenRes ();
  GraphicWidget *gr = new GraphicWidget (this->get_application (),
					 req.get_width (), req.get_height (),
					 this);
  gr->class_finished.connect ( [gr]
  {
    delete gr;
  });
  gr->plot (1, 7);
}

void
MainWindow::plotPSDGlobal ()
{
  Gdk::Rectangle req = screenRes ();
  GraphicWidget *gr = new GraphicWidget (this->get_application (),
					 req.get_width (), req.get_height (),
					 this);
  gr->class_finished.connect ( [gr]
  {
    delete gr;
  });
  gr->plot (2, 8);
}

void
MainWindow::plotVolumeGlobal ()
{
  Gdk::Rectangle req = screenRes ();
  GraphicWidget *gr = new GraphicWidget (this->get_application (),
					 req.get_width (), req.get_height (),
					 this);
  gr->class_finished.connect ( [gr]
  {
    delete gr;
  });
  gr->plot (2, 9);
}

void
MainWindow::plotMoneyGlobal ()
{
  Gdk::Rectangle req = screenRes ();
  GraphicWidget *gr = new GraphicWidget (this->get_application (),
					 req.get_width (), req.get_height (),
					 this);
  gr->class_finished.connect ( [gr]
  {
    delete gr;
  });
  gr->plot (2, 10);
}

void
MainWindow::plotGlobalCommon ()
{
  Gdk::Rectangle req = screenRes ();
  GraphicWidget *gr = new GraphicWidget (this->get_application (),
					 req.get_width (), req.get_height (),
					 this);
  gr->class_finished.connect ( [gr]
  {
    delete gr;
  });
  gr->plot (2, 11);
}

void
MainWindow::downloadMenu ()
{
  Gtk::MessageDialog *dialog = new Gtk::MessageDialog (
      *this, gettext ("<span foreground='#EFFF00' size='large'>"
		      "Wait, share's list is downloading..."
		      "</span>"),
      true, Gtk::MessageType::INFO, Gtk::ButtonsType::NONE, true);
  dialog->set_decorated (false);
  dialog->set_name ("dialog");
  dialog->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  Gtk::Box *bm = dialog->get_message_area ();
  bm->set_margin (10);
  Gtk::Button *cancel = dialog->add_button (gettext ("Cancel"), 3);
  cancel->set_halign (Gtk::Align::CENTER);
  cancel->set_name ("dialogButton");
  cancel->set_margin (5);
  cancel->get_style_context ()->add_provider (css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  dialog->set_hide_on_close (true);

  DownloadMenu *menu = new DownloadMenu (this->get_application (), this);
  if (dispv != nullptr)
    {
      delete dispv;
    }
  Glib::Dispatcher *dispatcher = new Glib::Dispatcher;
  dispv = dispatcher;
  menu->techdwnld.connect (sigc::mem_fun (*this, &MainWindow::ondwnldmenuCall));
  std::thread *thr = new std::thread ( [menu]
  {
    menu->downloadTechnical ();
  });
  thr->detach ();
  delete thr;

  dialog->signal_response ().connect (
      sigc::bind (sigc::mem_fun (*this, &MainWindow::deleteDialog), dialog,
		  menu, cancel));
  dispatcher->connect (
      sigc::bind (sigc::mem_fun (*this, &MainWindow::deleteDialog), 10, dialog,
		  menu, cancel));
  dialog->set_application (this->get_application ());
  dialog->present ();
}

void
MainWindow::deleteDialog (int id, Gtk::MessageDialog *dl, DownloadMenu *menu,
			  Gtk::Button *button)
{

  if (id == 10)
    {
      id = signid;
    }
  if (id == 0)
    {
      dl->hide ();
      delete menu;
      delete dl;
    }
  if (id == 1)
    {
      dl->hide ();
      delete dl;
      menu->downloadMenu ();
    }
  if (id == 2)
    {
      dl->set_message (gettext ("Network error"));
      button->set_label (gettext ("Close"));
      button->signal_clicked ().connect (
	  sigc::bind (sigc::mem_fun (*dl, &Gtk::MessageDialog::response), 0));
    }
  if (id == 3)
    {
      menu->cancelTechnical ();
    }

}

void
MainWindow::ondwnldmenuCall (int Signid)
{
  signid = Signid;
  dispv->emit ();
}

Gdk::Rectangle
MainWindow::screenRes ()
{
  Glib::RefPtr<Gdk::Surface> surf = this->get_surface ();
  Glib::RefPtr<Gdk::Display> disp = this->get_display ();
  Glib::RefPtr<Gdk::Monitor> mon = disp->get_monitor_at_surface (surf);
  Gdk::Rectangle req;
  mon->get_geometry (req);
  return req;
}

void
MainWindow::aboutProg ()
{
  Gtk::Window *window = new Gtk::Window;
  window->set_application (this->get_application ());
  window->set_title (gettext ("About"));
  Gtk::Notebook *noteb = Gtk::make_managed<Gtk::Notebook> ();
  noteb->set_halign (Gtk::Align::CENTER);
  window->set_child (*noteb);

  std::fstream f;
  std::string filename;
  AuxFunc af;
  std::filesystem::path filepath;
  Glib::RefPtr<Gtk::TextBuffer> txtab = Gtk::TextBuffer::create ();
  Glib::ustring abbuf = gettext (
      "Money is a programm to collect statistics from moex.com\n"
      "Author Yury Bobylev <bobilev_yury@mail.ru>.\n"
      "Program uses next libraries:\n"
      "GTK https://www.gtk.org\n"
      "Libcurl https://curl.se/libcurl/\n"
      "Libzip https://libzip.org\n"
      "GMP https://gmplib.org/"
      "ICU https://icu.unicode.org");
  txtab->set_text (abbuf);
  Gtk::TextView *tvab = Gtk::make_managed<Gtk::TextView> ();
  tvab->set_buffer (txtab);
  tvab->set_editable (false);
  tvab->set_margin (5);
  Gdk::Rectangle scrres = screenRes ();
  Gtk::ScrolledWindow *scrab = Gtk::make_managed<Gtk::ScrolledWindow> ();
  scrab->set_policy (Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  scrab->set_halign (Gtk::Align::CENTER);
  if (scrres.get_width () < 700)
    {
      scrab->set_min_content_width (scrres.get_width () - 10);
    }
  else
    {
      scrab->set_min_content_width (700);
    }
  if (scrres.get_height () < 700)
    {
      scrab->set_min_content_height (scrres.get_height () - 10);
    }
  else
    {
      scrab->set_min_content_height (700);
    }
  scrab->set_child (*tvab);
  noteb->append_page (*scrab, gettext ("About"));

  std::filesystem::path p = std::filesystem::u8path (af.get_selfpath ());
  filename = p.parent_path ().u8string () + "/../share/Money/COPYING";
  filepath = std::filesystem::u8path (filename);
  Glib::RefPtr<Gtk::TextBuffer> txtlc = Gtk::TextBuffer::create ();
  f.open (filepath, std::ios_base::in | std::ios_base::binary);
  if (f.is_open ())
    {
      size_t sz = std::filesystem::file_size (filepath);
      std::vector<char> ab;
      ab.resize (sz);
      f.read (&ab[0], ab.size ());
      f.close ();
      Glib::ustring abbuf (ab.begin (), ab.end ());
      txtlc->set_text (abbuf);
    }
  else
    {
      std::cerr << "Licence file not found" << std::endl;
    }

  Gtk::TextView *tvlc = Gtk::make_managed<Gtk::TextView> ();
  tvlc->set_buffer (txtlc);
  tvlc->set_editable (false);
  tvlc->set_margin (5);

  Gtk::ScrolledWindow *scrlc = Gtk::make_managed<Gtk::ScrolledWindow> ();
  scrlc->set_policy (Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  scrlc->set_halign (Gtk::Align::CENTER);
  if (scrres.get_width () < 700)
    {
      scrlc->set_min_content_width (scrres.get_width () - 10);
    }
  else
    {
      scrlc->set_min_content_width (700);
    }
  if (scrres.get_height () < 700)
    {
      scrlc->set_min_content_height (scrres.get_height () - 10);
    }
  else
    {
      scrlc->set_min_content_height (700);
    }
  scrlc->set_child (*tvlc);
  noteb->append_page (*scrlc, gettext ("Licence"));

  window->signal_close_request ().connect ( [window]
  {
    window->hide ();
    delete window;
    return true;
  },
					   false);
  window->present ();
}
