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

#include "DownloadMenu.h"

DownloadMenu::DownloadMenu(Glib::RefPtr<Gtk::Application> app,
                           Gtk::Window *mwin)
{
  App = app;
  Mwin = mwin;
  css_provider = Gtk::CssProvider::create();
  AuxFunc af;
  std::filesystem::path p = std::filesystem::u8path(af.get_selfpath());
  css_provider->load_from_path(
    Glib::ustring(
      std::string(p.parent_path().u8string())
      + "/../share/Money/downloadMenu.css"));
}

DownloadMenu::~DownloadMenu()
{

}

void
DownloadMenu::downloadTechnical()
{
  std::string url =
    "https://iss.moex.com/iss/engines/stock/markets/shares/boards.csv";
  std::string filename;
  AuxFunc af;
  af.homePath(&filename);
  filename = filename + "/.Money/BoardsList";
  int check = 0;
  int check2 = 0;
  std::string line;
  int count = 0;
  for(;;)
    {
      if(CancelTech > 0)
        {
          techdwnld.emit(0);
          return void ();
        }
      af.callNetwork(url, filename, &check);
      count++;
      if(count == 20)
        {
          techdwnld.emit(2);
          return void ();
        }
      if(check > 0)
        {
          break;
        }
      sleep(1);
    }
  url =
    "https://iss.moex.com/iss/engines/stock/markets/foreignshares/boards.csv";
  af.homePath(&filename);
  filename = filename + "/.Money/BoardsListF";
  count = 0;
  for(;;)
    {
      if(CancelTech > 0)
        {
          techdwnld.emit(0);
          return void ();
        }
      af.callNetwork(url, filename, &check2);
      count++;
      if(count == 20)
        {
          techdwnld.emit(2);
          return void ();
        }
      if(check2 > 0)
        {
          break;
        }
      sleep(1);
    }
  std::filesystem::path p = std::filesystem::u8path(filename);
  std::vector<std::string> tv;
  if(check2 > 0)
    {
      std::fstream f;
      f.open(p, std::ios_base::in);
      count = 0;
      while(!f.eof())
        {
          std::string line;
          getline(f, line);
          if(count > 2 && line != "")
            {
              tv.push_back(line);
              line.erase(0, line.find(";") + 1);
              line.erase(0, line.find(";") + 1);
              line = line.substr(0, line.find(";"));
              af.cp1251toutf8(line);
            }
          count++;
        }
      f.close();
    }
  std::filesystem::remove(p);
  if(check2 > 0)
    {
      af.homePath(&filename);
      filename = filename + "/.Money/BoardsList";
      p = std::filesystem::u8path(filename);
      std::fstream f;
      std::vector<std::string> tv2;
      f.open(p, std::ios_base::in);
      count = 0;
      while(!f.eof())
        {
          std::string line;
          getline(f, line);
          if(count <= 2)
            {
              tv2.push_back(line);
            }
          else
            {
              if(line != "")
                {
                  tv2.push_back(line);
                }
            }
          count++;
        }
      f.close();
      for(size_t i = 0; i < tv.size(); i++)
        {
          tv2.push_back(tv[i]);
        }
      f.open(p, std::ios_base::out | std::ios_base::binary);
      for(size_t i = 0; i < tv2.size(); i++)
        {
          std::string line = tv2[i];
          line = line + "\n";
          f.write(line.c_str(), line.size());
        }
      f.close();
    }
  if(check > 0 || check2 > 0)
    {
      af.homePath(&filename);
      filename = filename + "/.Money/BoardsList";
      p = std::filesystem::u8path(filename);

      std::fstream f;

      f.open(p, std::ios_base::in);

      count = 0;

      if(f.is_open())
        {

          while(!f.eof())
            {
              getline(f, line);
              af.cp1251toutf8(line);
              if(count > 2 && line != "")
                {
                  std::pair<Glib::ustring, Glib::ustring> bp;
                  Glib::ustring tmp;
                  tmp = line;
                  tmp.erase(0, tmp.find(";") + Glib::ustring(";").size());
                  tmp.erase(0, tmp.find(";") + Glib::ustring(";").size());
                  tmp = tmp.substr(0, tmp.find(";"));
                  bp.first = tmp;
                  tmp = line;
                  tmp.erase(0, tmp.find(";") + Glib::ustring(";").size());
                  tmp.erase(0, tmp.find(";") + Glib::ustring(";").size());
                  tmp.erase(0, tmp.find(";") + Glib::ustring(";").size());
                  tmp = tmp.substr(0, tmp.find(";"));
                  bp.second = tmp;
                  boards.push_back(bp);
                }
              count++;
            }
          f.close();
        }
    }

  std::filesystem::path path(std::filesystem::temp_directory_path());
  std::string tmp = path.u8string();
  #ifdef __linux
  tmp = tmp + "/Money/List";
  #endif
  #ifdef _WIN32
  tmp = tmp + "Money/List";
  #endif
  path = std::filesystem::u8path(tmp);
  if(!std::filesystem::exists(path))
    {
      std::filesystem::create_directories(path);
    }
  check = 0;
  count = 0;
  int linecount = 0;
  int errorcount = 0;
  for(;;)
    {
      tmp = std::filesystem::temp_directory_path().u8string();
      #ifdef __linux
      tmp = tmp + "/Money/List";
      #endif
      #ifdef _WIN32
      tmp = tmp + "Money/List";
      #endif
      if(CancelTech > 0)
        {
          path = std::filesystem::u8path(tmp);
          std::filesystem::remove_all(path);
          techdwnld.emit(0);
          return void ();
        }
      tmp = tmp + "/List" + std::to_string(count);
      p = std::filesystem::u8path(tmp);
      url =
        "https://iss.moex.com/iss/history/engines/stock/markets/foreignshares/listing.csv?start="
        + std::to_string(count);
      linecount = 0;
      errorcount = 0;
      check = 0;
      line = "";
      for(;;)
        {
          af.callNetwork(url, p.u8string(), &check);
          if(check > 0)
            {
              break;
            }
          else
            {
              errorcount++;
            }
          if(errorcount > 100)
            {
              techdwnld.emit(2);
              return void ();
            }
        }
      std::fstream f(p, std::ios_base::in);
      while(!f.eof())
        {
          getline(f, line);
          if(linecount == 3)
            {
              break;
            }
          linecount++;
        }
      f.close();
      if(line == "")
        {
          if(availibaleDatesF() == 0)
            {
              std::filesystem::remove(p);
              break;
            }
          else
            {
              techdwnld.emit(2);
              return void ();
            }
        }
      count = count + 100;
    }

  tmp = std::filesystem::temp_directory_path().u8string();
  #ifdef __linux
  tmp = tmp + "/Money/List";
  #endif
  #ifdef _WIN32
  tmp = tmp + "Money/List";
  #endif
  path = std::filesystem::u8path(tmp);
  if(std::filesystem::exists(path))
    {
      std::fstream f;
      for(auto &dirit : std::filesystem::directory_iterator(path))
        {
          p = dirit.path();
          f.open(p, std::ios_base::in);
          std::string tmp1, tmp2, previous;
          linecount = 0;
          while(!f.eof())
            {
              getline(f, line);
              af.cp1251toutf8(line);
              if(linecount > 2)
                {
                  if(line != "")
                    {
                      tmp1 = line;
                      tmp1.erase(0,
                                 tmp1.find(";") + std::string(";").size());
                      tmp1 = tmp1.substr(0, tmp1.find(";"));

                      tmp2 = line;
                      tmp2 = tmp2.substr(0, tmp2.find(";"));
                      if(previous != tmp2)
                        {
                          instrumentsF.push_back(std::make_pair(tmp2, tmp1));
                        }
                      previous = tmp2;
                    }
                }
              linecount++;
            }
          f.close();
        }
    }
  std::filesystem::remove_all(path);
  std::filesystem::create_directories(path);
  check = 0;
  count = 0;
  linecount = 0;
  errorcount = 0;
  for(;;)
    {
      if(CancelTech > 0)
        {
          tmp = std::filesystem::temp_directory_path().u8string();
          #ifdef __linux
          tmp = tmp + "/Money/List";
          #endif
          #ifdef _WIN32
          tmp = tmp + "Money/List";
          #endif
          path = std::filesystem::u8path(tmp);
          std::filesystem::remove_all(path);
          techdwnld.emit(0);
          return void ();
        }
      linecount = 0;
      errorcount = 0;
      check = 0;
      line = "";
      url =
        "https://iss.moex.com/iss/history/engines/stock/markets/shares/listing.csv?start="
        + std::to_string(count);
      filename = tmp + "/List" + std::to_string(count);
      for(;;)
        {
          af.callNetwork(url, filename, &check);
          if(check > 0)
            {
              break;
            }
          else
            {
              errorcount++;
            }
          if(errorcount > 100)
            {
              techdwnld.emit(2);
              return void ();
            }
        }
      path = std::filesystem::u8path(filename);
      std::fstream f(path, std::ios_base::in);
      while(!f.eof())
        {
          getline(f, line);
          if(linecount == 3)
            {
              break;
            }
          linecount++;
        }
      f.close();
      if(line == "")
        {
          if(availibaleDates() == 0)
            {
              techdwnld.emit(1);
            }
          else
            {
              techdwnld.emit(2);
              return void ();
            }
          std::filesystem::remove(path);
          break;
        }
      count = count + 100;
    }
}

void
DownloadMenu::cancelTechnical()
{
  CancelTech = 1;
}

void
DownloadMenu::downloadMenu()
{
  formVector();
  if(instruments.size() > 0)
    {
      AuxFunc af;
      Gtk::Window *window = new Gtk::Window;
      MainMenu = window;
      window->set_name("downloadWindow");
      window->get_style_context()->add_provider(
        css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
      window->set_title(gettext("Download menu"));
      window->set_transient_for(*Mwin);
      window->set_modal(true);

      auto grid = Gtk::make_managed<Gtk::Grid> ();
      grid->set_halign(Gtk::Align::CENTER);

      Gtk::TreeModel::ColumnRecord record;
      Gtk::TreeModelColumn<unsigned int> id;
      Gtk::TreeModelColumn<Glib::ustring> instrument;
      record.add(id);
      record.add(instrument);

      Glib::RefPtr<Gtk::ListStore> model = Gtk::ListStore::create(record);
      Gtk::TreeView *box = Gtk::make_managed<Gtk::TreeView> ();
      box->set_name("treeView");
      box->get_style_context()->add_provider(
        css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
      auto scroll = Gtk::make_managed<Gtk::ScrolledWindow> ();

      window->set_child(*grid);

      Gtk::Expander *exp = Gtk::make_managed<Gtk::Expander> ();
      exp->set_name("expander");
      exp->get_style_context()->add_provider(
        css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
      exp->set_child(*scroll);
      exp->set_label(instruments[0].second + "-" + instruments[0].first);
      Gtk::Widget *widg = exp->get_label_widget();
      widg->set_margin(5);
      exp->set_margin(5);
      exp->set_valign(Gtk::Align::CENTER);
      grid->attach(*exp, 0, 0, 1, 1);
      scroll->set_expand();
      scroll->set_policy(Gtk::PolicyType::AUTOMATIC,
                         Gtk::PolicyType::AUTOMATIC);
      scroll->set_child(*box);

      for(size_t i = 0; i < instruments.size(); i++)
        {
          auto row = *(model->append());
          row[id] = i;
          row[instrument] = instruments[i].second + "-" + instruments[i].first;
        }
      Gtk::TreeViewColumn *column = Gtk::make_managed<Gtk::TreeViewColumn> ();

      column->pack_start(instrument);
      box->append_column(*column);
      box->set_headers_visible(false);
      box->set_activate_on_single_click(true);
      box->set_size_request(-1, -1);
      box->signal_row_activated().connect(
        sigc::bind(sigc::mem_fun(*this, &DownloadMenu::instrSelection),
                   model, id, exp));
      exp->property_expanded().signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &DownloadMenu::expandScroll), exp,
                   window, grid));

      Gtk::Button *instrdeals = Gtk::make_managed<Gtk::Button> ();
      instrdeals->set_name("instrDeals");
      instrdeals->get_style_context()->add_provider(
        css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
      instrdeals->set_label(gettext("Transactions for instrument"));
      instrdeals->set_margin(5);
      instrdeals->signal_clicked().connect(
        sigc::mem_fun(*this, &DownloadMenu::downlodSinglInstrDeals));
      grid->attach(*instrdeals, 0, 1, 1, 1);

      Gtk::Button *instrall = Gtk::make_managed<Gtk::Button> ();
      instrall->set_name("instrAll");
      instrall->get_style_context()->add_provider(
        css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
      instrall->set_label(gettext("Daily results for instrument"));
      instrall->set_margin(5);
      instrall->signal_clicked().connect(
        sigc::mem_fun(*this, &DownloadMenu::downlodSinglInstrAll));
      grid->attach(*instrall, 0, 2, 1, 1);

      Gtk::Label *Dateserver = Gtk::make_managed<Gtk::Label> ();
      Dateserver->set_name("dateServer");
      Dateserver->get_style_context()->add_provider(
        css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
      Glib::ustring dateservstr = gettext("Last available date on server: ")
                                  + af.togrigday(dateserver) + "." + af.togrigmonth(dateserver) + "."
                                  + af.togrigyear(dateserver);
      Dateserver->set_text(dateservstr);
      Dateserver->set_margin(5);
      grid->attach(*Dateserver, 0, 3, 1, 1);

      Gtk::Label *Datehome = Gtk::make_managed<Gtk::Label> ();
      Datehome->set_name("dateHome");
      Datehome->get_style_context()->add_provider(
        css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
      Glib::ustring datehomestr = gettext("Last downloaded data, date: ")
                                  + af.togrigday(datehome) + "." + af.togrigmonth(datehome) + "."
                                  + af.togrigyear(datehome);
      Datehome->set_text(datehomestr);
      Datehome->set_margin(5);
      grid->attach(*Datehome, 0, 4, 1, 1);

      Gtk::Button *dealsdaily = Gtk::make_managed<Gtk::Button> ();
      dealsdaily->set_name("dealsDaily");
      dealsdaily->get_style_context()->add_provider(
        css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
      dealsdaily->set_label(gettext("All transactions for current date"));
      dealsdaily->set_margin(5);
      dealsdaily->signal_clicked().connect(
        sigc::mem_fun(*this, &DownloadMenu::downloadDeals));
      grid->attach(*dealsdaily, 0, 5, 1, 1);

      Gtk::Button *alld = Gtk::make_managed<Gtk::Button> ();
      alld->set_name("alld");
      alld->get_style_context()->add_provider(
        css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
      alld->set_label(gettext("All results"));
      alld->set_margin(5);
      alld->signal_clicked().connect(
        sigc::mem_fun(*this, &DownloadMenu::downloadAll));
      grid->attach(*alld, 0, 6, 1, 1);
      box->set_model(model);

      int x, y, h, w;
      column->cell_get_size(x, y, w, h);

      if(instruments.size() > 20)
        {
          scroll->set_min_content_height(20 * h);
        }
      else
        {
          scroll->set_min_content_height(instruments.size() * h);
        }

      Gtk::Requisition rq1, rq2;
      exp->get_preferred_size(rq1, rq2);
      window->set_default_size(rq2.get_width() + 10, -1);
      scroll->set_max_content_width(rq1.get_width());
      window->signal_close_request().connect([window]
      {
        window->hide();
        delete window;
        return true;
      },
      false);
      window->set_application(App);
      window->present();
    }

}

int
DownloadMenu::availibaleDates()
{
  int check = 0;
  int errorcount = 0;
  AuxFunc af;
  std::string tmp;
  tmp = std::filesystem::temp_directory_path().u8string();
  #ifdef __linux
  tmp = tmp + "/Money/Dates";
  #endif
  #ifdef _WIN32
  tmp = tmp + "Money/Dates";
  #endif
  for(;;)
    {
      af.callNetwork(
        "https://iss.moex.com/iss/history/engines/stock/markets/shares/dates.csv",
        tmp, &check);
      if(check > 0)
        {
          break;
        }
      else
        {
          errorcount++;
        }
      if(errorcount > 100)
        {
          return 1;
        }
    }
  return 0;
}

int
DownloadMenu::availibaleDatesF()
{
  int check = 0;
  int errorcount = 0;
  AuxFunc af;
  std::string tmp;
  tmp = std::filesystem::temp_directory_path().u8string();
  #ifdef __linux
  tmp = tmp + "/Money/Dates";
  #endif
  #ifdef _WIN32
  tmp = tmp + "Money/Dates";
  #endif
  for(;;)
    {
      af.callNetwork(
        "https://iss.moex.com/iss/history/engines/stock/markets/foreignshares/dates.csv",
        tmp, &check);
      if(check > 0)
        {
          break;
        }
      else
        {
          errorcount++;
        }
      if(errorcount > 100)
        {
          return 1;
        }
    }
  return 0;
}

void
DownloadMenu::formVector()
{
  AuxFunc af;
  std::fstream f;
  int count = 0;
  int linecount = 0;
  std::string line, tmp1, tmp2;
  std::string previous = "";
  for(;;)
    {
      linecount = 0;
      std::filesystem::path path;
      line = std::filesystem::temp_directory_path().u8string();
      #ifdef __linux
      line = line + "/Money/List/List" + std::to_string(count);
      #endif
      #ifdef _WIN32
      line = line + "Money/List/List" + std::to_string(count);
      #endif
      path = std::filesystem::u8path(line);
      if(std::filesystem::exists(path))
        {
          f.open(path, std::ios_base::in);
          while(!f.eof())
            {
              getline(f, line);
              af.cp1251toutf8(line);
              if(linecount > 2)
                {
                  if(line != "")
                    {
                      tmp1 = line;
                      tmp1.erase(0,
                                 tmp1.find(";") + std::string(";").size());
                      tmp1 = tmp1.substr(0, tmp1.find(";"));

                      tmp2 = line;
                      tmp2 = tmp2.substr(0, tmp2.find(";"));
                      if(previous != tmp2)
                        {
                          instruments.push_back(
                            std::make_pair(Glib::ustring(tmp2),
                                           Glib::ustring(tmp1)));
                        }
                      previous = tmp2;
                    }
                }
              linecount++;
            }
          f.close();
        }
      else
        {
          break;
        }
      count = count + 100;
    }
  std::sort(instruments.begin(), instruments.end(), []
            (auto & el1, auto & el2)
  {
    std::string line1(std::get<1>(el1));
    std::string line2(std::get<1>(el2));
    AuxFunc af;
    af.stringToLower(line1);
    af.stringToLower(line2);
    return line1 < line2;
  });
  std::sort(instrumentsF.begin(), instrumentsF.end(), []
            (auto & el1, auto & el2)
  {
    std::string line1(std::get<1>(el1));
    std::string line2(std::get<1>(el2));
    AuxFunc af;
    af.stringToLower(line1);
    af.stringToLower(line2);
    return line1 < line2;
  });
  for(size_t i = 0; i < instrumentsF.size(); i++)
    {
      instruments.push_back(instrumentsF[i]);
    }

  line = std::filesystem::temp_directory_path().u8string();
  #ifdef __linux
  line = line + "/Money/Dates";
  #endif

  #ifdef _WIN32
  for(size_t i = 0; i < instruments.size(); i++)
    {
      std::string kaw(instruments[i].second);
      for(;;)
        {
          std::string::size_type n;
          n = kaw.find("\\", 0);
          if(n == std::string::npos)
            {
              break;
            }
          else
            {
              kaw.erase(n, 1);
            }
        }
      for(;;)
        {
          std::string::size_type n;
          n = kaw.find("/", 0);
          if(n == std::string::npos)
            {
              break;
            }
          else
            {
              kaw.erase(n, 1);
            }
        }
      for(;;)
        {
          std::string::size_type n;
          n = kaw.find(":", 0);
          if(n == std::string::npos)
            {
              break;
            }
          else
            {
              kaw.erase(n, 1);
            }
        }
      for(;;)
        {
          std::string::size_type n;
          n = kaw.find("*", 0);
          if(n == std::string::npos)
            {
              break;
            }
          else
            {
              kaw.erase(n, 1);
            }
        }
      for(;;)
        {
          std::string::size_type n;
          n = kaw.find("?", 0);
          if(n == std::string::npos)
            {
              break;
            }
          else
            {
              kaw.erase(n, 1);
            }
        }
      for(;;)
        {
          std::string::size_type n;
          n = kaw.find("\"", 0);
          if(n == std::string::npos)
            {
              break;
            }
          else
            {
              kaw.erase(n, 1);
            }
        }
      for(;;)
        {
          std::string::size_type n;
          n = kaw.find("<", 0);
          if(n == std::string::npos)
            {
              break;
            }
          else
            {
              kaw.erase(n, 1);
            }
        }
      for(;;)
        {
          std::string::size_type n;
          n = kaw.find(">", 0);
          if(n == std::string::npos)
            {
              break;
            }
          else
            {
              kaw.erase(n, 1);
            }
        }
      for(;;)
        {
          std::string::size_type n;
          n = kaw.find("|", 0);
          if(n == std::string::npos)
            {
              break;
            }
          else
            {
              kaw.erase(n, 1);
            }
        }
      for(;;)
        {
          std::string::size_type n;
          n = kaw.find("+", 0);
          if(n == std::string::npos)
            {
              break;
            }
          else
            {
              kaw.erase(n, 1);
            }
        }
      instruments[i].second = Glib::ustring(kaw);
    }
  line = line + "Money/Dates";
  #endif
  std::filesystem::path path = std::filesystem::u8path(line);
  if(std::filesystem::exists(path))
    {
      f.open(path, std::ios_base::in);
      linecount = 0;
      while(!f.eof())
        {
          getline(f, line);
          if(linecount == 3)
            {
              line.erase(0, line.find(";") + std::string(";").size());
              int day, month, year;
              tmp1 = line;
              tmp1 = tmp1.substr(0, tmp1.find("-"));
              year = std::stoi(tmp1.c_str());
              tmp1 = line;
              tmp1.erase(0, tmp1.find("-") + std::string("-").size());
              tmp1 = tmp1.substr(0, tmp1.find("-"));
              month = std::stoi(tmp1.c_str());
              tmp1 = line;
              tmp1.erase(0, tmp1.find("-") + std::string("-").size());
              tmp1.erase(0, tmp1.find("-") + std::string("-").size());
              day = std::stoi(tmp1.c_str());
              dateserver = af.grigtojulian(day, month, year, 0, 0, 0.0);
            }
          linecount++;
        }
      f.close();
    }
  line = std::filesystem::temp_directory_path().u8string();
  #ifdef __linux
  line = line + "/Money/Dates";
  #endif
  #ifdef _WIN32
  line = line + "Money/Dates";
  #endif
  path = std::filesystem::u8path(line);
  std::filesystem::remove(path);
  af.homePath(&line);
  line = line + "/.Money/DateBegin";
  path = std::filesystem::u8path(line);
  if(std::filesystem::exists(path))
    {
      f.open(path, std::ios_base::in);
      getline(f, line);
      f.close();
      datehome = std::stoi(line.c_str());
    }
  line = std::filesystem::temp_directory_path().u8string();
  #ifdef __linux
  line = line + "/Money/List";
  #endif
  #ifdef _WIN32
  line = line + "Money/List";
  #endif
  path = std::filesystem::u8path(line);
  std::filesystem::remove_all(path);
}

void
DownloadMenu::expandScroll(Gtk::Expander *exp, Gtk::Window *window,
                           Gtk::Grid *grid)
{
  if(!exp->property_expanded().get_value())
    {
      Gtk::Requisition rq1, rq2;
      exp->get_preferred_size(rq1, rq2);
      window->set_default_size(rq2.get_width(), -1);
    }
}

void
DownloadMenu::instrSelection(const Gtk::TreeModel::Path &path,
                             Gtk::TreeViewColumn *column,
                             Glib::RefPtr<Gtk::ListStore> model,
                             Gtk::TreeModelColumn<unsigned int> id,
                             Gtk::Expander *exp)
{
  const auto iter = model->get_iter(path);
  if(iter)
    {
      auto row = *iter;
      selectedinstr = row[id];
      exp->set_label(
        instruments[row[id]].second + "-" + instruments[row[id]].first);
      Gtk::Widget *widg = exp->get_label_widget();
      widg->set_margin(5);
      exp->set_expanded(false);
    }
}

void
DownloadMenu::downloadAll()
{
  downloadcancel = 0;
  Gtk::Window *window = new Gtk::Window;
  window->set_title(gettext("Downloading..."));
  window->set_deletable(false);
  window->set_name("downloadWindowd");
  window->set_transient_for(*Mwin);
  window->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  auto grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_halign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label> ();
  label->set_text(gettext("Downloading in progress..."));
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::ProgressBar *progbar = Gtk::make_managed<Gtk::ProgressBar> ();
  progbar->set_show_text(true);
  progbar->set_name("progBar");
  progbar->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  progbar->set_margin(5);
  grid->attach(*progbar, 0, 1, 1, 1);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button> ();
  cancel->set_label(gettext("Cancel"));
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_name("cancelButton");
  cancel->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  grid->attach(*cancel, 0, 2, 1, 1);

  DownloadAll *dall = new DownloadAll(&downloadcancel, &instruments,
                                      &instrumentsF, &boards, datehome);
  Glib::Dispatcher *dwnld_1 = new Glib::Dispatcher;
  Glib::Dispatcher *dwnld_2 = new Glib::Dispatcher;
  Glib::Dispatcher *dwnld_3 = new Glib::Dispatcher;
  Glib::Dispatcher *dwnld_5 = new Glib::Dispatcher;
  Glib::Dispatcher *dwnld_7 = new Glib::Dispatcher;
  double *progbr = new double;
  int *svdt = new int;
  int *cncl = &downloadcancel;
  cancel->signal_clicked().connect([cncl]
  {
    *cncl = 1;
  });
  dwnld_7->connect([progbr, progbar]
  {
    double pr = *progbr;
    progbar->set_fraction(pr);
  });
  dall->progress.connect([progbr, dwnld_7]
                         (double pr)
  {
    *progbr = pr;
    dwnld_7->emit();
  });
  dwnld_1->connect(
    [this, window, dwnld_1, dwnld_2, dwnld_3, dwnld_5, dwnld_7, progbr, svdt,
           dall]
  {
    window->hide();
    delete window;
    delete window;
    delete dwnld_1;
    delete dwnld_2;
    delete dwnld_3;
    delete dwnld_5;
    delete dwnld_7;
    delete progbr;
    delete svdt;
    delete dall;
    this->errorMessage();
  });
  dall->globError.connect([dwnld_1]
  {
    dwnld_1->emit();
  });

  dwnld_2->connect(
    [this, window, dwnld_1, dwnld_2, dwnld_3, dwnld_5, dwnld_7, progbr, svdt,
           dall]
  {
    window->hide();
    delete window;
    delete dwnld_1;
    delete dwnld_2;
    delete dwnld_3;
    delete dwnld_5;
    delete dwnld_7;
    delete progbr;
    delete svdt;
    delete dall;
    this->class_finished.emit();
  });
  dall->canceled.connect([dwnld_2]
  {
    dwnld_2->emit();
  });

  dwnld_3->connect(
    [window, dwnld_1, dwnld_2, dwnld_3, dwnld_5, dwnld_7, progbr, svdt, dall,
             this]
  {
    window->hide();
    delete window;
    delete dwnld_1;
    delete dwnld_2;
    delete dwnld_3;
    delete dwnld_5;
    delete dwnld_7;
    delete progbr;
    delete svdt;
    delete dall;
    this->errorMessage();
  });
  dall->netError.connect([dwnld_3]
  {
    dwnld_3->emit();
  });

  dwnld_5->connect(
    [window, dwnld_1, dwnld_2, dwnld_3, dwnld_5, dwnld_7, progbr, svdt, dall,
             this]
  {
    int dt = *svdt;
    this->saveDate(dt);
    this->calcIndex();
    window->hide();
    delete window;
    delete dwnld_1;
    delete dwnld_2;
    delete dwnld_3;
    delete dwnld_5;
    delete dwnld_7;
    delete progbr;
    delete dall;
    delete svdt;
  });

  dall->saveDate.connect([dwnld_5, svdt]
                         (int date)
  {
    *svdt = date;
    dwnld_5->emit();
  });

  Gtk::Requisition rq1, rq2;
  grid->get_preferred_size(rq1, rq2);
  window->set_default_size(rq2.get_width(), -1);
  window->set_application(App);
  window->present();
  MainMenu->close();
  std::thread *thr = new std::thread([dall]
  {
    dall->downloadAll();
  });
  thr->detach();
  delete thr;
}

void
DownloadMenu::errorMessage()
{
  Gtk::Window *window = new Gtk::Window;
  window->set_title(gettext("Network error"));
  window->set_name("errorMessage");
  window->set_transient_for(*Mwin);
  window->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  auto grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_halign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label> ();
  label->set_text(gettext("Network error!"));
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);
  label->set_size_request(-1, -1);

  Gtk::Button *close = Gtk::make_managed<Gtk::Button> ();
  close->set_label(gettext("Close"));
  close->set_margin(5);
  close->set_halign(Gtk::Align::CENTER);
  grid->attach(*close, 0, 1, 1, 1);

  close->signal_clicked().connect(
    sigc::mem_fun(*window, &Gtk::Window::close));
  Gtk::Requisition rq1, rq2;
  grid->get_preferred_size(rq1, rq2);
  window->set_default_size(rq2.get_width(), -1);
  window->signal_close_request().connect([window, this]
  {
    window->hide();
    delete window;
    this->class_finished.emit();
    return true;
  },
  false);
  window->set_application(App);
  window->present();
}

void
DownloadMenu::saveDate(int dt)
{
  std::fstream f;
  std::string line;
  AuxFunc af;
  af.homePath(&line);
  line = line + "/.Money/DateBegin";
  std::filesystem::path pathdb = std::filesystem::u8path(line);
  std::stringstream strm;
  std::locale loc("C");
  strm.str("");
  strm.clear();
  strm.imbue(loc);
  strm << std::fixed;
  strm << std::setprecision(0);
  strm << dt;
  line = strm.str();
  f.open(pathdb, std::ios_base::out | std::ios_base::binary);
  f.write(line.c_str(), line.size());
  f.close();
}

void
DownloadMenu::calcIndex()
{
  downloadcancel = 0;
  Gtk::Window *window = new Gtk::Window;
  window->set_title(gettext("PPm calculation"));
  window->set_name("downloadWindowd");
  window->set_transient_for(*Mwin);
  window->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  auto grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_halign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label> ();
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::ProgressBar *progbar = Gtk::make_managed<Gtk::ProgressBar> ();
  progbar->set_show_text(true);
  progbar->set_name("progBar");
  progbar->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  progbar->set_margin(5);
  grid->attach(*progbar, 0, 1, 1, 1);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button> ();
  cancel->set_label(gettext("Cancel"));
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_name("cancelButton");
  cancel->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  grid->attach(*cancel, 0, 2, 1, 1);

  CalcIndex *calc = new CalcIndex(&downloadcancel);
  Glib::Dispatcher *calci_1 = new Glib::Dispatcher;
  Glib::Dispatcher *calci_2 = new Glib::Dispatcher;
  Glib::Dispatcher *calci_3 = new Glib::Dispatcher;
  Glib::Dispatcher *calci_4 = new Glib::Dispatcher;
  double *calci_progr = new double;
  Glib::ustring *lname = new Glib::ustring;
  int *cncl = &downloadcancel;

  cancel->signal_clicked().connect([cncl]
  {
    *cncl = 1;
  });

  calc->progr.connect([calci_progr, calci_1]
                      (double element)
  {
    *calci_progr = element;
    calci_1->emit();
  });

  calci_1->connect([progbar, calci_progr]
  {
    double fr = *calci_progr;
    progbar->set_fraction(fr);
  });

  calci_2->connect([label, lname]
  {
    Glib::ustring nm = *lname;
    label->set_text(nm);
  });
  calc->operationName.connect([lname, calci_2]
                              (Glib::ustring e)
  {
    *lname = e;
    calci_2->emit();
  });
  calc->canceled.connect([calci_4]
  {
    calci_4->emit();
  });

  calci_4->connect(sigc::mem_fun(*window, &Gtk::Window::close));

  std::thread *thr = new std::thread([calc]
  {
    calc->mainCalc();
  });
  thr->detach();
  delete thr;

  window->signal_close_request().connect(
    [window, calc, calci_1, calci_2, calci_3, calci_4, calci_progr, lname]
  {
    window->hide();
    delete window;
    delete calci_1;
    delete calci_2;
    delete calci_3;
    delete calci_4;
    delete calci_progr;
    delete lname;
    delete calc;
    return true;
  },
  false);
  calci_3->connect(sigc::mem_fun(*window, &Gtk::Window::close));
  calci_3->connect(sigc::mem_fun(*this, &DownloadMenu::finishMessage));
  calc->allComplet.connect([calci_3]
  {
    calci_3->emit();
  });
  Gtk::Requisition rq1, rq2;
  grid->get_preferred_size(rq1, rq2);
  window->set_default_size(rq2.get_width() + 10, -1);
  window->set_application(App);
  window->present();
}

void
DownloadMenu::downloadDeals()
{
  downloadcancel = 0;
  Gtk::Window *window = new Gtk::Window;
  window->set_title(gettext("Transactions downloading"));
  window->set_deletable(false);
  window->set_name("downloadWindowd");
  window->set_transient_for(*Mwin);
  window->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  auto grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_halign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label> ();
  label->set_text(gettext("Transaction downloading progress:"));
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::ProgressBar *progbar = Gtk::make_managed<Gtk::ProgressBar> ();
  progbar->set_show_text(true);
  progbar->set_margin(5);
  progbar->set_name("progBar");
  progbar->get_style_context()->add_provider(
    css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  grid->attach(*progbar, 0, 1, 1, 1);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button> ();
  cancel->set_label(gettext("Cancel"));
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_name("cancelButton");
  cancel->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  grid->attach(*cancel, 0, 2, 1, 1);

  Glib::Dispatcher *deals_1 = new Glib::Dispatcher;
  Glib::Dispatcher *deals_2 = new Glib::Dispatcher;
  Glib::Dispatcher *deals_3 = new Glib::Dispatcher;
  Glib::Dispatcher *deals_4 = new Glib::Dispatcher;
  Glib::Dispatcher *deals_5 = new Glib::Dispatcher;
  double *frc = new double;
  int *cncl = &downloadcancel;

  DownloadDeals *ddls = new DownloadDeals(&downloadcancel, &instruments,
                                          &instrumentsF, &boards, 0);
  cancel->signal_clicked().connect([cncl]
  {
    *cncl = 1;
  });

  ddls->noDeals.connect([deals_1]
  {
    deals_1->emit();
  });

  deals_1->connect(
    [deals_1, deals_2, deals_3, deals_4, deals_5, frc, window, ddls, this]
  {
    window->hide();
    delete window;
    delete deals_1;
    delete deals_2;
    delete deals_3;
    delete deals_4;
    delete deals_5;
    delete frc;
    delete ddls;
    this->noDeals(1);
  });

  ddls->netError.connect([deals_2]
  {
    deals_2->emit();
  });

  deals_2->connect(
    [deals_1, deals_2, deals_3, deals_4, deals_5, frc, window, ddls, this]
  {
    window->hide();
    delete window;
    delete deals_1;
    delete deals_2;
    delete deals_3;
    delete deals_4;
    delete deals_5;
    delete frc;
    delete ddls;
    this->noDeals(2);
  });

  ddls->canceled.connect([deals_3]
  {
    deals_3->emit();
  });

  deals_3->connect(
    [deals_1, deals_2, deals_3, deals_4, deals_5, frc, window, ddls, this]
  {
    window->hide();
    delete window;
    delete deals_1;
    delete deals_2;
    delete deals_3;
    delete deals_4;
    delete deals_5;
    delete frc;
    delete ddls;
    this->class_finished.emit();
  });

  ddls->progr.connect([frc, deals_4]
                      (double el)
  {
    *frc = el;
    deals_4->emit();
  });

  deals_4->connect([frc, progbar]
  {
    double frac = *frc;
    progbar->set_fraction(frac);
  });

  ddls->finished.connect([deals_5]
  {
    deals_5->emit();
  });

  deals_5->connect(
    [deals_1, deals_2, deals_3, deals_4, deals_5, frc, window, ddls, this]
  {
    window->hide();
    delete window;
    delete deals_1;
    delete deals_2;
    delete deals_3;
    delete deals_4;
    delete deals_5;
    delete frc;
    delete ddls;
    this->finishMessage();
  });

  std::thread *thr = new std::thread([ddls]
  {
    ddls->downloadAll();
  });
  thr->detach();
  delete thr;

  Gtk::Requisition rq1, rq2;
  grid->get_preferred_size(rq1, rq2);
  window->set_default_size(rq2.get_width() + 10, -1);
  window->set_application(App);
  window->present();
  MainMenu->close();
}

void
DownloadMenu::noDeals(int sel)
{
  Gtk::Window *window = new Gtk::Window;
  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_halign(Gtk::Align::CENTER);
  window->set_child(*grid);
  window->set_transient_for(*Mwin);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label> ();
  if(sel == 1)
    {
      label->set_text(gettext("No transactions today"));
      window->set_name("downloadWindowd");
      window->set_title(gettext("Message"));
    }
  else
    {
      label->set_text(gettext("Network error"));
      window->set_title(gettext("Error!"));
      window->set_name("errorMessage");
      window->get_style_context()->add_provider(css_provider,
          GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::Button *button = Gtk::make_managed<Gtk::Button> ();
  button->set_label(gettext("Close"));
  button->set_margin(5);
  button->set_halign(Gtk::Align::CENTER);
  button->set_name("cancelButton");
  button->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);

  grid->attach(*button, 0, 1, 1, 1);

  button->signal_clicked().connect(
    sigc::mem_fun(*window, &Gtk::Window::close));
  window->signal_close_request().connect([window, this]
  {
    window->hide();
    delete window;
    this->class_finished.emit();
    return true;
  },
  false);
  Gtk::Requisition rq1, rq2;
  grid->get_preferred_size(rq1, rq2);
  window->set_default_size(rq2.get_width() + 10, -1);
  window->set_application(App);
  window->present();
}

void
DownloadMenu::downlodSinglInstrAll()
{
  downloadcancel = 0;
  Gtk::Window *window = new Gtk::Window;
  window->set_title(gettext("Downloading..."));
  window->set_name("downloadWindowd");
  window->set_transient_for(*Mwin);
  window->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_halign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label> ();
  label->set_text(gettext("Downloading in progress..."));
  label->set_halign(Gtk::Align::CENTER);
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::ProgressBar *prgb = Gtk::make_managed<Gtk::ProgressBar> ();
  prgb->set_margin(5);
  prgb->set_show_text(false);
  prgb->set_name("progBar");
  prgb->get_style_context()->add_provider(css_provider,
                                          GTK_STYLE_PROVIDER_PRIORITY_USER);
  grid->attach(*prgb, 0, 1, 1, 1);

  Gtk::Button *stop = Gtk::make_managed<Gtk::Button> ();
  stop->set_margin(5);
  stop->set_label(gettext("Cancel"));
  stop->set_halign(Gtk::Align::CENTER);
  stop->set_name("cancelButton");
  stop->get_style_context()->add_provider(css_provider,
                                          GTK_STYLE_PROVIDER_PRIORITY_USER);
  grid->attach(*stop, 0, 2, 1, 1);

  stop->signal_clicked().connect([this]
  {
    this->downloadcancel = 1;
  });

  std::vector<std::pair<Glib::ustring, Glib::ustring>> *vect = new std::vector <
  std::pair<Glib::ustring, Glib::ustring >>;
  vect->push_back(instruments[selectedinstr]);
  AuxFunc af;
  double bd = af.grigtojulian(23, 3, 1997, 0, 0, 0.0);
  DownloadAll *all = new DownloadAll(&downloadcancel, vect, &instrumentsF,
                                     &boards, bd);
  Glib::Dispatcher *sing_d1 = new Glib::Dispatcher;
  Glib::Dispatcher *sing_d2 = new Glib::Dispatcher;
  Glib::Dispatcher *sing_d3 = new Glib::Dispatcher;
  Glib::Dispatcher *sing_d4 = new Glib::Dispatcher;
  all->pulse.connect([sing_d1]
  {
    sing_d1->emit();
  });

  sing_d1->connect([prgb]
  {
    prgb->pulse();
  });

  all->globError.connect([sing_d2]
  {
    sing_d2->emit();
  });

  sing_d2->connect(
    [this, window, all, sing_d1, sing_d2, sing_d3, sing_d4, vect]
  {
    window->hide();
    delete window;
    delete all;
    delete sing_d1;
    delete sing_d2;
    delete sing_d3;
    delete sing_d4;
    delete vect;
    this->errorMessage();
  });

  all->canceled.connect([sing_d3]
  {
    sing_d3->emit();
  });

  sing_d3->connect(
    [this, window, all, sing_d1, sing_d2, sing_d3, sing_d4, vect]
  {
    window->hide();
    delete window;
    delete all;
    delete sing_d1;
    delete sing_d2;
    delete sing_d3;
    delete sing_d4;
    delete vect;
    this->class_finished.emit();
  });

  all->netError.connect([sing_d2]
  {
    sing_d2->emit();
  });

  all->finished.connect([sing_d4]
  {
    sing_d4->emit();
  });

  sing_d4->connect(
    [this, window, all, sing_d1, sing_d2, sing_d3, sing_d4, vect]
  {
    window->hide();
    delete window;
    delete all;
    delete sing_d1;
    delete sing_d2;
    delete sing_d3;
    delete sing_d4;
    delete vect;
    this->finishMessage();
  });

  std::thread *thr = new std::thread([all]
  {
    all->downloadAll();
  });
  thr->detach();
  delete thr;

  Gtk::Requisition rq1, rq2;
  grid->get_preferred_size(rq1, rq2);
  window->set_default_size(rq2.get_width() + 10, -1);
  window->set_application(App);
  window->present();
  MainMenu->close();
}

void
DownloadMenu::finishMessage()
{
  Gtk::Window *window = new Gtk::Window;
  window->set_title(gettext("Downloading completed"));
  window->set_name("finishMessage");
  window->set_transient_for(*Mwin);
  window->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  auto grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_halign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label> ();
  label->set_text(gettext("Downloading completed!"));
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);
  label->set_size_request(-1, -1);

  Gtk::Button *close = Gtk::make_managed<Gtk::Button> ();
  close->set_label("Закрыть");
  close->set_margin(5);
  close->set_halign(Gtk::Align::CENTER);
  close->set_name("cancelButton");
  close->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  grid->attach(*close, 0, 1, 1, 1);

  close->signal_clicked().connect(
    sigc::mem_fun(*window, &Gtk::Window::close));
  Gtk::Requisition rq1, rq2;
  grid->get_preferred_size(rq1, rq2);
  window->set_default_size(rq2.get_width() + 10, -1);
  window->signal_close_request().connect([window, this]
  {
    window->hide();
    delete window;
    this->class_finished.emit();
    return true;
  },
  false);
  window->set_application(App);
  window->present();
}

void
DownloadMenu::downlodSinglInstrDeals()
{
  downloadcancel = 0;
  Gtk::Window *window = new Gtk::Window;
  window->set_title(gettext("Downloading..."));
  window->set_name("downloadWindowd");
  window->set_transient_for(*Mwin);
  window->get_style_context()->add_provider(css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid> ();
  grid->set_halign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label> ();
  label->set_text(gettext("Downloading in progress..."));
  label->set_halign(Gtk::Align::CENTER);
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::ProgressBar *prgb = Gtk::make_managed<Gtk::ProgressBar> ();
  prgb->set_margin(5);
  prgb->set_show_text(false);
  grid->attach(*prgb, 0, 1, 1, 1);

  Gtk::Button *stop = Gtk::make_managed<Gtk::Button> ();
  stop->set_margin(5);
  stop->set_label(gettext("Cancel"));
  stop->set_halign(Gtk::Align::CENTER);
  stop->set_name("cancelButton");
  stop->get_style_context()->add_provider(css_provider,
                                          GTK_STYLE_PROVIDER_PRIORITY_USER);
  grid->attach(*stop, 0, 2, 1, 1);

  stop->signal_clicked().connect([this]
  {
    this->downloadcancel = 1;
  });

  std::vector<std::pair<Glib::ustring, Glib::ustring>> *vect = new std::vector <
  std::pair<Glib::ustring, Glib::ustring >>;
  vect->push_back(instruments[selectedinstr]);
  DownloadDeals *all = new DownloadDeals(&downloadcancel, vect, &instrumentsF,
                                         &boards, 1);
  Glib::Dispatcher *sing_d1 = new Glib::Dispatcher;
  Glib::Dispatcher *sing_d2 = new Glib::Dispatcher;
  Glib::Dispatcher *sing_d3 = new Glib::Dispatcher;
  Glib::Dispatcher *sing_d4 = new Glib::Dispatcher;
  Glib::Dispatcher *sing_d5 = new Glib::Dispatcher;
  all->pulse.connect([sing_d1]
  {
    sing_d1->emit();
  });

  sing_d1->connect([prgb]
  {
    prgb->pulse();
  });

  all->netError.connect([sing_d2]
  {
    sing_d2->emit();
  });

  sing_d2->connect(
    [window, all, sing_d1, sing_d2, sing_d3, sing_d4, sing_d5, vect, this]
  {
    window->hide();
    delete window;
    delete all;
    delete sing_d1;
    delete sing_d2;
    delete sing_d3;
    delete sing_d4;
    delete sing_d5;
    delete vect;
    this->errorMessage();
  });

  all->canceled.connect([sing_d3]
  {
    sing_d3->emit();
  });

  sing_d3->connect(
    [window, all, sing_d1, sing_d2, sing_d3, sing_d4, sing_d5, vect, this]
  {
    window->hide();
    delete window;
    delete all;
    delete sing_d1;
    delete sing_d2;
    delete sing_d3;
    delete sing_d4;
    delete sing_d5;
    delete vect;
    this->class_finished.emit();
  });

  all->finished.connect([sing_d4]
  {
    sing_d4->emit();
  });

  sing_d4->connect(
    [window, all, sing_d1, sing_d2, sing_d3, sing_d4, sing_d5, vect, this]
  {
    window->hide();
    delete window;
    delete all;
    delete sing_d1;
    delete sing_d2;
    delete sing_d3;
    delete sing_d4;
    delete sing_d5;
    delete vect;
    this->finishMessage();
  });

  all->noDeals.connect([sing_d5]
  {
    sing_d5->emit();
  });

  sing_d5->connect(
    [window, all, sing_d1, sing_d2, sing_d3, sing_d4, sing_d5, vect, this]
  {
    window->hide();
    delete window;
    delete all;
    delete sing_d1;
    delete sing_d2;
    delete sing_d3;
    delete sing_d4;
    delete sing_d5;
    delete vect;
    this->noDeals(1);
  });

  std::thread *thr = new std::thread([all]
  {
    all->downloadAll();
  });
  thr->detach();
  delete thr;

  Gtk::Requisition rq1, rq2;
  grid->get_preferred_size(rq1, rq2);
  window->set_default_size(rq2.get_width() + 10, -1);
  window->set_application(App);
  window->present();
  MainMenu->close();
}
