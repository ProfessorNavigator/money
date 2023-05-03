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

#include "MainWindow.h"

MainWindow::MainWindow()
{
  Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
  AuxFunc af;
  std::filesystem::path p = std::filesystem::u8path(af.get_selfpath());

  std::string css_styles;

  std::string filename = p.parent_path().u8string()
      + "/../share/Money/stylesMoney.css";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  std::fstream f;
  f.open(filepath, std::ios_base::in | std::ios_base::binary);
  if(f.is_open())
    {
      css_styles.resize(std::filesystem::file_size(filepath));
      f.read(&css_styles[0], css_styles.size());
      f.close();
    }
  css_provider->load_from_data(css_styles);
  Glib::RefPtr<Gdk::Display> disp = this->get_display();
  Gtk::StyleContext::add_provider_for_display(disp, css_provider,
  GTK_STYLE_PROVIDER_PRIORITY_USER);
  windowFunc();
}

MainWindow::~MainWindow()
{
  // TODO Auto-generated destructor stub
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
      std::string date = std::to_string(
	  static_cast<int>(af.grigtojulian(23, 3, 1997, 0, 0, 0.0)));
      file.write(date.c_str(), date.size());
      file.close();
    }
  pathto = af.tempPath();
  pathto = pathto + "/Money";

  path = std::filesystem::u8path(pathto);
  if(std::filesystem::exists(path))
    {
      std::filesystem::remove_all(path);
    }

  this->set_title("Money");
  this->set_name("mainWindow");
  this->set_default_size(1, 1);

  auto grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::CENTER);
  grid->set_valign(Gtk::Align::CENTER);
  this->set_child(*grid);

  Gtk::Button *priceall = Gtk::make_managed<Gtk::Button>(gettext("Price"));
  priceall->set_name("allButton");
  priceall->set_margin(5);
  priceall->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotPriceall));
  grid->attach(*priceall, 0, 0);

  Gtk::Button *moneyprice = Gtk::make_managed<Gtk::Button>(
      gettext("Purchasing power of money"));
  moneyprice->set_name("allButton");
  moneyprice->set_margin(5);
  moneyprice->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotPSDall));
  grid->attach(*moneyprice, 0, 1);

  Gtk::Button *moneyvolume = Gtk::make_managed<Gtk::Button>(
      gettext("Shares turnover"));
  moneyvolume->set_name("allButton");
  moneyvolume->set_margin(5);
  moneyvolume->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotVolumeAll));
  grid->attach(*moneyvolume, 0, 2);

  Gtk::Button *moneyvolumev = Gtk::make_managed<Gtk::Button>(
      gettext("Money turnover"));
  moneyvolumev->set_name("allButton");
  moneyvolumev->set_margin(5);
  moneyvolumev->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotMoneyVolumeAll));
  grid->attach(*moneyvolumev, 0, 3);

  Gtk::Button *pricedeals = Gtk::make_managed<Gtk::Button>(
      gettext("Prices for transactions"));
  pricedeals->set_name("dealsButton");
  pricedeals->set_margin(5);
  pricedeals->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotPriceDeals));
  grid->attach(*pricedeals, 0, 4);

  Gtk::Button *dealsmoneypr = Gtk::make_managed<Gtk::Button>(
      gettext("PPm for transactions"));
  dealsmoneypr->set_name("dealsButton");
  dealsmoneypr->set_margin(5);
  dealsmoneypr->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotPSDDeals));
  grid->attach(*dealsmoneypr, 0, 5);

  Gtk::Button *dealsvolume = Gtk::make_managed<Gtk::Button>(
      gettext("Shares turnover for transactions"));
  dealsvolume->set_name("dealsButton");
  dealsvolume->set_margin(5);
  dealsvolume->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotVolumeDeals));
  grid->attach(*dealsvolume, 0, 6);

  Gtk::Button *dealsmoneyvol = Gtk::make_managed<Gtk::Button>(
      gettext("Money turnover for transactions"));
  dealsmoneyvol->set_name("dealsButton");
  dealsmoneyvol->set_margin(5);
  dealsmoneyvol->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotMoneyVolDeals));
  grid->attach(*dealsmoneyvol, 0, 7);

  Gtk::Button *commongraphicall = Gtk::make_managed<Gtk::Button>(
      gettext("Prediction plots (common)"));
  commongraphicall->set_name("commongraphicallButton");
  commongraphicall->set_margin(5);
  commongraphicall->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotAllCommon));
  grid->attach(*commongraphicall, 0, 8);

  Gtk::Button *commongraphicdeals = Gtk::make_managed<Gtk::Button>(
      gettext("Prediction plots (daily)"));
  commongraphicdeals->set_name("commongraphicdealsButton");
  commongraphicdeals->set_margin(5);
  commongraphicdeals->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotDealsCommon));
  grid->attach(*commongraphicdeals, 0, 9);

  Gtk::Button *psd = Gtk::make_managed<Gtk::Button>(gettext("PPm global"));
  psd->set_name("allButton");
  psd->set_margin(5);
  psd->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotPSDGlobal));
  grid->attach(*psd, 0, 10);

  Gtk::Button *volumetotal = Gtk::make_managed<Gtk::Button>(
      gettext("Shares turnover global"));
  volumetotal->set_name("allButton");
  volumetotal->set_margin(5);
  volumetotal->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotVolumeGlobal));
  grid->attach(*volumetotal, 0, 11);

  Gtk::Button *moneytotal = Gtk::make_managed<Gtk::Button>(
      gettext("Money turnover global"));
  moneytotal->set_name("allButton");
  moneytotal->set_margin(5);
  moneytotal->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::plotMoneyGlobal));
  grid->attach(*moneytotal, 0, 12);

  Gtk::Button *anomalysearch = Gtk::make_managed<Gtk::Button>(
      gettext("Search anomaly"));
  anomalysearch->set_name("anomalyButton");
  anomalysearch->set_margin(5);
  anomalysearch->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::anomalySearch));
  grid->attach(*anomalysearch, 0, 13);

  Gtk::Button *download = Gtk::make_managed<Gtk::Button>(
      gettext("Download data"));
  download->set_name("downloadButton");
  download->set_margin(5);
  download->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::downloadMenu));
  grid->attach(*download, 0, 14);

  Gtk::Button *aboutpr = Gtk::make_managed<Gtk::Button>(gettext("About"));
  aboutpr->set_name("aboutprButton");
  aboutpr->set_margin(5);
  aboutpr->signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::aboutProg));
  grid->attach(*aboutpr, 0, 15);
  this->signal_close_request().connect([this, &af]
  {
    std::vector<Gtk::Window*> win = this->get_application()->get_windows();
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
    std::string fp = af.tempPath();
    fp = fp + "/Money";

    std::filesystem::path path = std::filesystem::u8path(fp);
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
  OpenDialog op(this);
  op.creatDialogAll(0, req);
}

void
MainWindow::plotPSDall()
{
  Gdk::Rectangle req = screenRes();
  OpenDialog op(this);
  op.creatDialogAll(1, req);
}

void
MainWindow::plotVolumeAll()
{
  Gdk::Rectangle req = screenRes();
  OpenDialog op(this);
  op.creatDialogAll(2, req);
}

void
MainWindow::plotMoneyVolumeAll()
{
  Gdk::Rectangle req = screenRes();
  OpenDialog op(this);
  op.creatDialogAll(3, req);
}

void
MainWindow::plotPriceDeals()
{
  Gdk::Rectangle req = screenRes();
  OpenDialogDeals opd(this);
  opd.createDialog(4, req);
}

void
MainWindow::plotPSDDeals()
{
  Gdk::Rectangle req = screenRes();
  OpenDialogDeals opd(this);
  opd.createDialog(5, req);
}

void
MainWindow::plotVolumeDeals()
{
  Gdk::Rectangle req = screenRes();
  OpenDialogDeals opd(this);
  opd.createDialog(6, req);
}

void
MainWindow::plotMoneyVolDeals()
{
  Gdk::Rectangle req = screenRes();
  OpenDialogDeals opd(this);
  opd.createDialog(7, req);
}

void
MainWindow::plotAllCommon()
{
  Gdk::Rectangle req = screenRes();
  OpenDialog op(this);
  op.creatDialogAll(8, req);
}

void
MainWindow::plotDealsCommon()
{
  Gdk::Rectangle req = screenRes();
  OpenDialogDeals opd(this);
  opd.createDialog(9, req);
}

void
MainWindow::plotPSDGlobal()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(req.get_width(), req.get_height(), this,
					"");
  gr->plot(2, 10, "");
}

void
MainWindow::plotVolumeGlobal()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(req.get_width(), req.get_height(), this,
					"");
  gr->plot(2, 11, "");
}

void
MainWindow::plotMoneyGlobal()
{
  Gdk::Rectangle req = screenRes();
  GraphicWidget *gr = new GraphicWidget(req.get_width(), req.get_height(), this,
					"");
  gr->plot(2, 12, "");
}

void
MainWindow::anomalySearch()
{
  AnomalySearch as(this);
  as.windowFunc();
}

void
MainWindow::downloadMenu()
{
  Gtk::Window *window = new Gtk::Window;
  window->set_application(this->get_application());
  window->set_decorated(false);
  window->set_transient_for(*this);
  window->set_modal(true);
  window->set_default_size(1, 1);
  window->set_name("dialog");

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::CENTER);
  grid->set_valign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *msg = Gtk::make_managed<Gtk::Label>();
  msg->set_margin(5);
  msg->set_halign(Gtk::Align::CENTER);
  msg->set_width_chars(30);
  msg->set_wrap(true);
  msg->set_wrap_mode(Pango::WrapMode::WORD);
  msg->set_justify(Gtk::Justification::CENTER);
  msg->set_use_markup(true);
  msg->set_markup(
      Glib::ustring("<span foreground='#EFFF00' size='large'>")
	  + Glib::ustring(gettext("Wait, shares list is downloading..."))
	  + Glib::ustring("</span>"));
  grid->attach(*msg, 0, 0, 1, 1);

  int *cncl = new int(0);
  DownloadTechnical *dt = new DownloadTechnical(cncl);
  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button>();
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_label(gettext("Cancel"));
  cancel->set_name("closeButton");
  cancel->signal_clicked().connect(
      [window, cancel, grid, msg, cncl]
      {
	msg->set_markup(
	    Glib::ustring("<span foreground='#EFFF00' size='large'>")
		+ Glib::ustring(gettext("Cancelling operation..."))
		+ Glib::ustring("</span>"));
	window->set_default_size(1, 1);
	grid->remove(*cancel);

	*cncl = 1;
      });
  grid->attach(*cancel, 0, 1, 1, 1);

  int *dt_resp = new int(0);
  Glib::Dispatcher *dt_resp_disp = new Glib::Dispatcher;
  dt_resp_disp->connect([dt_resp, dt_resp_disp, window, msg, grid, cancel, this]
  {
    this->dtRespFunc(dt_resp, window, grid, cancel, msg);
    delete dt_resp;
    delete dt_resp_disp;
  });

  dt->techdwnld = [dt_resp, dt_resp_disp, dt]
  (int resp_id)
    {
      *dt_resp = resp_id;
      dt_resp_disp->emit();
      delete dt;
    };

  window->signal_close_request().connect([window]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();

  std::thread *thr = new std::thread([dt, cncl]
  {
    dt->downloadTechnical();
    delete cncl;
  });
  thr->detach();
  delete thr;
}

void
MainWindow::dtRespFunc(int *resp, Gtk::Window *window, Gtk::Grid *grid,
		       Gtk::Button *cancel, Gtk::Label *msg)
{
  if(*resp == 0)
    {
      window->close();
    }
  else if(*resp == 1)
    {
      DownloadMenu menu(this);
      menu.downloadMenu();
      window->close();
    }
  else if(*resp == 2)
    {
      grid->remove(*cancel);

      window->set_default_size(1, 1);
      msg->set_markup(
	  Glib::ustring("<span foreground='#EFFF00' size='large'>")
	      + Glib::ustring(gettext("Network error"))
	      + Glib::ustring("</span>"));

      Gtk::Button *close = Gtk::make_managed<Gtk::Button>();
      close->set_margin(5);
      close->set_halign(Gtk::Align::CENTER);
      close->set_label(gettext("Close"));
      close->set_name("closeButton");
      close->signal_clicked().connect([window]
      {
	window->close();
      });
      grid->attach(*close, 0, 1, 1, 1);

      Glib::RefPtr<Glib::MainContext> mc = Glib::MainContext::get_default();
      while(mc->pending())
	{
	  mc->iteration(true);
	}
    }
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
  aboutd->set_name("dialog");

  aboutd->set_program_name("Money");
  aboutd->set_version("3.1.5");
  aboutd->set_copyright(
      "Copyright 2021-2023 Yury Bobylev <bobilev_yury@mail.ru>");
  AuxFunc af;
  std::filesystem::path p = std::filesystem::u8path(af.get_selfpath());
  std::string filename = p.parent_path().u8string() + "/../share/Money/COPYING";
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

  if(abbuf.empty())
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
