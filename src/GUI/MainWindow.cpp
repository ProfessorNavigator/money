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

MainWindow::MainWindow()
{
  css_provider = Gtk::CssProvider::create();
  AuxFunc af;
  std::filesystem::path p = std::filesystem::u8path(af.get_selfpath());
  css_provider->load_from_path(
    Glib::ustring(
      std::string(p.parent_path().u8string())
      + "/../share/Money/mainWindow.css"));
  windowFunc();
}

MainWindow::~MainWindow()
{
   delete dispv;
}

void
MainWindow::windowFunc()
{
  std::string pathto;
  AuxFunc af;
  af.homePath(&pathto);
  pathto = pathto + "/.Money";
  std::filesystem::path path = std::filesystem::u8path(pathto);
  if(!std::filesystem::exists(path))
    {
      std::filesystem::create_directories(path);
    }
  pathto = path.u8string() + "/DateBegin";
  path = std::filesystem::u8path(pathto);
  if(!std::filesystem::exists(path))
    {
      std::fstream file;
      file.open(path, std::ios_base::out | std::ios_base::binary);
      std::string date = std::to_string(static_cast<int>(af.grigtojulian(23, 3, 1997,
                                        0, 0, 0.0)));
      file.write(date.c_str(), date.size());
      file.close();
    }
  pathto = std::filesystem::temp_directory_path().u8string();
  #ifdef __linux
  pathto = pathto + "/Money";
  #endif
  #ifdef _WIN32
  pathto = pathto + "Money";
  #endif
  path = std::filesystem::u8path(pathto);
  if(std::filesystem::exists(path))
    {
      std::filesystem::remove_all(path);
    }

  this->set_title("Money");
  this->set_name("mainWindow");
  get_style_context()->add_provider(css_provider,
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);

  auto grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_halign(Gtk::Align::CENTER);
  grid->set_margin(10);
  this->set_child(*grid);

  Gtk::Button *priceall = Gtk::make_managed<Gtk::Button> (gettext("Price"));
  priceall->set_name("allButton");
  priceall->set_margin(5);
  priceall->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  priceall->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotPriceall));
  grid->attach(*priceall, 0, 0);

  Gtk::Button *moneyprice = Gtk::make_managed<Gtk::Button> (
                              gettext("Purchasing power of money"));
  moneyprice->set_name("allButton");
  moneyprice->set_margin(5);
  moneyprice->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  moneyprice->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotPSDall));
  grid->attach(*moneyprice, 0, 1);

  Gtk::Button *moneyvolume = Gtk::make_managed<Gtk::Button> (
                               gettext("Shares turnover"));
  moneyvolume->set_name("allButton");
  moneyvolume->set_margin(5);
  moneyvolume->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  moneyvolume->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotVolumeAll));
  grid->attach(*moneyvolume, 0, 2);

  Gtk::Button *moneyvolumev = Gtk::make_managed<Gtk::Button> (
                                gettext("Money turnover"));
  moneyvolumev->set_name("allButton");
  moneyvolumev->set_margin(5);
  moneyvolumev->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  moneyvolumev->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotMoneyVolumeAll));
  grid->attach(*moneyvolumev, 0, 3);

  Gtk::Button *pricedeals = Gtk::make_managed<Gtk::Button> (
                              gettext("Prices for transactions"));
  pricedeals->set_name("dealsButton");
  pricedeals->set_margin(5);
  pricedeals->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  pricedeals->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotPriceDeals));
  grid->attach(*pricedeals, 0, 4);

  Gtk::Button *dealsmoneypr = Gtk::make_managed<Gtk::Button> (
                                gettext("PPm for transactions"));
  dealsmoneypr->set_name("dealsmoneyprButton");
  dealsmoneypr->set_margin(5);
  dealsmoneypr->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  dealsmoneypr->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotPSDDeals));
  grid->attach(*dealsmoneypr, 0, 5);

  Gtk::Button *dealsvolume = Gtk::make_managed<Gtk::Button> (
                               gettext("Shares turnover for transactions"));
  dealsvolume->set_name("dealsvolumeButton");
  dealsvolume->set_margin(5);
  dealsvolume->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  dealsvolume->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotVolumeDeals));
  grid->attach(*dealsvolume, 0, 6);

  Gtk::Button *dealsmoneyvol = Gtk::make_managed<Gtk::Button> (
                                 gettext("Money turnover for transactions"));
  dealsmoneyvol->set_name("dealsvolumeButton");
  dealsmoneyvol->set_margin(5);
  dealsmoneyvol->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  dealsmoneyvol->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotMoneyVolDeals));
  grid->attach(*dealsmoneyvol, 0, 7);

  Gtk::Button *commongraphicall = Gtk::make_managed<Gtk::Button> (
                                    gettext("Prediction plots (common)"));
  commongraphicall->set_name("commongraphicallButton");
  commongraphicall->set_margin(5);
  commongraphicall->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  commongraphicall->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotAllCommon));
  grid->attach(*commongraphicall, 0, 8);

  Gtk::Button *commongraphicdeals = Gtk::make_managed<Gtk::Button> (
                                      gettext("Prediction plots (daily)"));
  commongraphicdeals->set_name("commongraphicdealsButton");
  commongraphicdeals->set_margin(5);
  commongraphicdeals->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  commongraphicdeals->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotDealsCommon));
  grid->attach(*commongraphicdeals, 0, 9);

  Gtk::Button *psd = Gtk::make_managed<Gtk::Button> (gettext("PPm global"));
  psd->set_name("psdButton");
  psd->set_margin(5);
  psd->get_style_context()->add_provider(css_provider,
                                         GTK_STYLE_PROVIDER_PRIORITY_USER);
  psd->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotPSDGlobal));
  grid->attach(*psd, 0, 10);

  Gtk::Button *volumetotal = Gtk::make_managed<Gtk::Button> (
                               gettext("Shares turnover global"));
  volumetotal->set_name("volumetotalButton");
  volumetotal->set_margin(5);
  volumetotal->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  volumetotal->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotVolumeGlobal));
  grid->attach(*volumetotal, 0, 11);

  Gtk::Button *moneytotal = Gtk::make_managed<Gtk::Button> (
                              gettext("Money turnover global"));
  moneytotal->set_name("moneytotalButton");
  moneytotal->set_margin(5);
  moneytotal->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  moneytotal->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::plotMoneyGlobal));
  grid->attach(*moneytotal, 0, 12);

  Gtk::Button *download = Gtk::make_managed<Gtk::Button> (
                            gettext("Download data"));
  download->set_name("downloadButton");
  download->set_margin(5);
  download->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  download->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::downloadMenu));
  grid->attach(*download, 0, 13);

  Gtk::Button *aboutpr = Gtk::make_managed<Gtk::Button> (gettext("About"));
  aboutpr->set_name("aboutprButton");
  aboutpr->set_margin(5);
  aboutpr->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  aboutpr->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::aboutProg));
  grid->attach(*aboutpr, 0, 14);
  this->signal_close_request().connect([this, &af]
  {
    std::vector<Gtk::Window *> win = this->get_application()->get_windows();
    for(size_t i = 0; i < win.size(); i++)
      {
        Gtk::Window *wind = win[i];
        if(wind != nullptr && wind != this)
          {
            wind->hide();
            delete wind;
          }
      }
    this->hide();
    std::string fp;
    fp = std::filesystem::temp_directory_path().u8string();
    #ifdef __linux
    fp = fp + "/Money";
    #endif
    #ifdef _WIN32
    fp = fp + "Money";
    #endif
    std::filesystem::path path =
      std::filesystem::u8path(fp);
    if(std::filesystem::exists(path))
      {
        std::filesystem::remove_all(path);
      }
    return true;
  },
  false);
}

void
MainWindow::plotPriceall()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(0, 0);
}

void
MainWindow::plotPSDall()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(0, 1);
}

void
MainWindow::plotVolumeAll()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(0, 2);
}

void
MainWindow::plotMoneyVolumeAll()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(0, 3);
}

void
MainWindow::plotPriceDeals()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(1, 4);
}

void
MainWindow::plotPSDDeals()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(1, 5);
}

void
MainWindow::plotVolumeDeals()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(1, 6);
}

void
MainWindow::plotMoneyVolDeals()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(1, 7);
}

void
MainWindow::plotAllCommon()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(0, 8);
}

void
MainWindow::plotDealsCommon()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(1, 9);
}

void
MainWindow::plotPSDGlobal()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(2, 10);
}

void
MainWindow::plotVolumeGlobal()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(2, 11);
}

void
MainWindow::plotMoneyGlobal()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(this->get_application(),
                                        req.get_width(), req.get_height(),
                                        this);
  gr->class_finished.connect([gr]
  {
    delete gr;
  });
  gr->plot(2, 12);
}

void
MainWindow::downloadMenu()
{
  Gtk::MessageDialog *dialog = new Gtk::MessageDialog(
    *this, gettext("<span foreground='#EFFF00' size='large'>"
                   "Wait, shares list is downloading..."
                   "</span>"),
    true, Gtk::MessageType::INFO, Gtk::ButtonsType::NONE, true);
  dialog->set_decorated(false);
  dialog->set_name("dialog");
  dialog->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  Gtk::Box *bm = dialog->get_message_area();
  bm->set_margin(10);
  Gtk::Button *cancel = dialog->add_button(gettext("Cancel"), 3);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_name("dialogButton");
  cancel->set_margin(5);
  cancel->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  dialog->set_hide_on_close(true);

  DownloadMenu *menu = new DownloadMenu(this->get_application(), this);
  delete dispv;
  dispv = nullptr;
  Glib::Dispatcher *dispatcher = new Glib::Dispatcher;
  dispv = dispatcher;
  menu->techdwnld.connect(sigc::mem_fun(*this, &MainWindow::ondwnldmenuCall));
  std::thread *thr = new std::thread([menu]
  {
    menu->downloadTechnical();
  });
  thr->detach();
  delete thr;

  dialog->signal_response().connect(
    sigc::bind(sigc::mem_fun(*this, &MainWindow::deleteDialog), dialog,
               menu, cancel));
  dispatcher->connect(
    sigc::bind(sigc::mem_fun(*this, &MainWindow::deleteDialog), 10, dialog,
               menu, cancel));
  dialog->set_application(this->get_application());
  dialog->present();
}

void
MainWindow::deleteDialog(int id, Gtk::MessageDialog *dl, DownloadMenu *menu,
                         Gtk::Button *button)
{

  if(id == 10)
    {
      id = signid;
    }
  if(id == 0)
    {
      dl->hide();
      delete menu;
      delete dl;
    }
  if(id == 1)
    {
      dl->hide();
      delete dl;
      menu->downloadMenu();
    }
  if(id == 2)
    {
      dl->set_message(gettext("Network error"));
      button->set_label(gettext("Close"));
      button->signal_clicked().connect(
        sigc::bind(sigc::mem_fun(*dl, &Gtk::MessageDialog::response), 0));
    }
  if(id == 3)
    {
      menu->cancelTechnical();
    }

}

void
MainWindow::ondwnldmenuCall(int Signid)
{
  signid = Signid;
  dispv->emit();
}

Gdk::Rectangle
MainWindow::screenRes()
{
  Glib::RefPtr<Gdk::Surface> surf = this->get_surface();
  Glib::RefPtr<Gdk::Display> disp = this->get_display();
  Glib::RefPtr<Gdk::Monitor> mon = disp->get_monitor_at_surface(surf);
  Gdk::Rectangle req;
  mon->get_geometry(req);
  return req;
}

void
MainWindow::aboutProg()
{
  Gtk::AboutDialog *aboutd = new Gtk::AboutDialog;
  aboutd->set_transient_for(*this);
  aboutd->set_application(this->get_application());

  aboutd->set_program_name("Money");
  aboutd->set_version("2.3.2");
  aboutd->set_copyright(
    "Copyright 2022 Yury Bobylev <bobilev_yury@mail.ru>");
  AuxFunc af;
  std::filesystem::path p = std::filesystem::u8path(af.get_selfpath());
  std::string filename = p.parent_path().u8string()
                         + "/../share/Money/COPYING";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  std::fstream f;
  Glib::ustring abbuf;
  f.open(filepath, std::ios_base::in | std::ios_base::binary);
  if(f.is_open())
    {
      size_t sz = std::filesystem::file_size(filepath);
      std::vector<char> ab;
      ab.resize(sz);
      f.read(&ab[0], ab.size());
      f.close();
      abbuf = Glib::ustring(ab.begin(), ab.end());
    }
  else
    {
      std::cerr << "Licence file not found" << std::endl;
    }

  if(abbuf.size() == 0)
    {
      aboutd->set_license_type(Gtk::License::GPL_3_0_ONLY);
    }
  else
    {
      aboutd->set_license(abbuf);
    }

  filename = p.parent_path().u8string() + "/../share/Money/money.png";
  Glib::RefPtr<Gio::File> logofile = Gio::File::create_for_path(filename);
  aboutd->set_logo(Gdk::Texture::create_from_file(logofile));
  abbuf = gettext("Money is a programm to collect statistics from moex.com\n"
                  "Author Yury Bobylev <bobilev_yury@mail.ru>.\n"
                  "Program uses next libraries:\n"
                  "GTK https://www.gtk.org\n"
                  "Libcurl https://curl.se/libcurl/\n"
                  "Libzip https://libzip.org\n"
                  "GMP https://gmplib.org/\n"
                  "ICU https://icu.unicode.org");
  aboutd->set_comments(abbuf);

  aboutd->signal_close_request().connect([aboutd]
  {
    aboutd->hide();
    delete aboutd;
    return true;
  },
  false);
  aboutd->show();
}
