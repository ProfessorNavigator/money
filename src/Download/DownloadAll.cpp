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

#include "DownloadAll.h"

DownloadAll::DownloadAll(
  int *cancel,
  std::vector<std::pair<Glib::ustring, Glib::ustring>> *instruments,
  std::vector<std::pair<Glib::ustring, Glib::ustring>> *instrumentsF,
  std::vector<std::pair<Glib::ustring, Glib::ustring>> *boards, double db)
{
  Cancel = cancel;
  Instruments = *instruments;
  InstrumentsF = *instrumentsF;
  Boards = *boards;
  Db = db;
}

DownloadAll::~DownloadAll()
{
  // TODO Auto-generated destructor stub
}

void
DownloadAll::downloadAll()
{
  AuxFunc af;
  std::string dirstr;
  std::filesystem::path dir;
  dirstr = std::filesystem::temp_directory_path().u8string();
  #ifdef __linux
  dirstr = dirstr + "/Money";
  #endif
  #ifdef _WIN32
  dirstr = dirstr + "Money";
  #endif
  dir = std::filesystem::u8path(dirstr);

  if(!std::filesystem::exists(dir))
    {
      std::filesystem::create_directory(dir);
    }

  int check = 0;
  int errorcount = 0;
  int linecount = 0;
  std::string line, tmp, tmp2, writevalue = "";
  double bd = Db;
  std::fstream f;
  dirstr = dirstr + "/Dates";
  for(;;)
    {
      pulse.emit();
      af.callNetwork(
        "https://iss.moex.com/iss/history/engines/stock/markets/shares/dates.csv",
        dirstr, &check);
      if(check > 0)
        {
          break;
        }
      else
        {
          usleep(100000);
          errorcount++;
        }
      if(errorcount > 100)
        {
          globError.emit();
          return void ();
        }
    }
  check = 0;
  errorcount = 0;
  dir = dirstr;
  f.open(dir, std::ios_base::in);
  double dt = 0.0;
  while(!f.eof())
    {
      getline(f, line);
      if(linecount == 3)
        {
          line = line.erase(0, line.find(";") + std::string(";").size());
          int year, month, day;
          tmp = line;
          tmp = tmp.substr(0, tmp.find("-"));
          std::stringstream strm;
          std::locale loc("C");
          strm.imbue(loc);
          strm << tmp;
          strm >> year;
          tmp = line;
          tmp = tmp.erase(0, tmp.find("-") + std::string("-").size());
          tmp = tmp.substr(0, tmp.find("-"));

          strm.clear();
          strm.str();
          strm.imbue(loc);
          strm << tmp;
          strm >> month;
          tmp = line;
          tmp = tmp.erase(0, tmp.find("-") + std::string("-").size());
          tmp = tmp.erase(0, tmp.find("-") + std::string("-").size());

          strm.clear();
          strm.str();
          strm.imbue(loc);
          strm << tmp;
          strm >> day;
          dt = af.grigtojulian(day, month, year, 0, 0, 0.0);
        }
      linecount++;
    }
  f.close();
  linecount = 0;
  std::vector<std::vector<std::string>> boardsvect;
  for(size_t i = 0; i < Boards.size(); i++)
    {
      std::vector<std::string> vect;
      boardsvect.push_back(vect);
    }
  line = "";
  if(bd < dt)
    {
      bd = bd + 1.0;
      std::string adress, filename;
      int dcount = 0;
      progress.emit(0);
      for(size_t i = 0; i < Instruments.size(); i++)
        {
          std::tuple<int, int, int, int, int> tupforsave;
          pulse.emit();
          if(*Cancel == 1)
            {
              canceled.emit();
              return void ();
            }
          dcount = 0;
          check = 0;
          errorcount = 0;
          dirstr = std::filesystem::temp_directory_path().u8string();
          #ifdef __linux
          dirstr = dirstr + "/Money/LoadAll";
          #endif
          #ifdef _WIN32
          dirstr = dirstr + "Money/LoadAll";
          #endif
          dir = std::filesystem::u8path(dirstr);
          std::filesystem::remove_all(dir);
          std::filesystem::create_directories(dir);
          for(;;)
            {
              pulse.emit();
              if(*Cancel == 1)
                {
                  canceled.emit();
                  return void ();
                }
              adress =
                "https://iss.moex.com/iss/history/engines/stock/markets/shares/securities/"
                + Instruments.at(i).first + ".csv?from="
                + af.togrigyear(bd) + "-" + af.togrigmonth(bd) + "-"
                + af.togrigday(bd) + "&till=" + af.togrigyear(dt) + "-"
                + af.togrigmonth(dt) + "-" + af.togrigday(dt)
                + "&start=" + std::to_string(dcount);
              Glib::ustring sustring(Instruments.at(i).first);
              auto itinf = std::find_if(InstrumentsF.begin(),
                                        InstrumentsF.end(), [&sustring]
                                        (auto & el)
              {
                return std::get<0>(el) == sustring;
              });
              if(itinf != InstrumentsF.end())
                {
                  adress =
                    "https://iss.moex.com/iss/history/engines/stock/markets/foreignshares/securities/"
                    + Instruments.at(i).first + ".csv?from="
                    + af.togrigyear(bd) + "-" + af.togrigmonth(bd) + "-"
                    + af.togrigday(bd) + "&till=" + af.togrigyear(dt)
                    + "-" + af.togrigmonth(dt) + "-" + af.togrigday(dt)
                    + "&start=" + std::to_string(dcount);
                }
              filename = dirstr + "/" + Instruments.at(i).first
                         + std::to_string(dcount);
              for(;;)
                {
                  af.callNetwork(adress, filename, &check);
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
                      netError.emit();
                      return void ();
                    }
                }

              linecount = 0;
              f.open(
                std::filesystem::path(std::filesystem::u8path(filename)),
                std::ios_base::in);
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
                  std::filesystem::path filepath;
                  filepath = filename;
                  std::filesystem::remove(filepath);
                  break;
                }
              dcount = dcount + 100;
            }
          dcount = 0;
          for(;;)
            {
              pulse.emit();
              if(*Cancel == 1)
                {
                  canceled.emit();
                  return void ();
                }
              linecount = 0;
              line = std::filesystem::temp_directory_path().u8string();
              #ifdef __linux
              line = line + "/Money/LoadAll/" + Instruments.at(i).first
                     + std::to_string(dcount);
              #endif
              #ifdef _WIN32
              line = line + "Money/LoadAll/" + Instruments.at(i).first
                     + std::to_string(dcount);
              #endif
              dir = std::filesystem::u8path(line);

              if(std::filesystem::exists(dir))
                {
                  std::tuple<int, int> temptup;
                  f.open(dir, std::ios_base::in);
                  while(!f.eof())
                    {
                      getline(f, line);
                      af.cp1251toutf8(line);
                      std::string temp;
                      if(linecount == 2)
                        {
                          tmp = line;
                          int countch = 0;
                          while(tmp.size() > 0)
                            {
                              temp = tmp.substr(0, tmp.find(";"));
                              std::string::size_type n;
                              if(temp == "BOARDID")
                                {
                                  std::get<0> (temptup) = countch;
                                }
                              if(temp == "VOLUME")
                                {
                                  std::get<1> (temptup) = countch;
                                  std::get<2> (tupforsave) = countch;
                                }
                              if(temp == "TRADEDATE")
                                {
                                  std::get<0> (tupforsave) = countch;
                                }
                              if(temp == "VALUE")
                                {
                                  std::get<1> (tupforsave) = countch;
                                }
                              if(temp == "OPEN")
                                {
                                  std::get<3> (tupforsave) = countch;
                                }
                              if(temp == "CLOSE")
                                {
                                  std::get<4> (tupforsave) = countch;
                                }
                              n = tmp.find(";");
                              if(n != std::string::npos)
                                {
                                  tmp.erase(0, n + std::string(";").size());
                                }
                              else
                                {
                                  break;
                                }
                              countch++;
                            }
                        }

                      if(linecount > 2 && line != "")
                        {
                          tmp = line;
                          for(int i = 0; i < std::get<0> (temptup); i++)
                            {
                              temp = tmp.substr(0, tmp.find(";"));
                              tmp = tmp.erase(
                                      0, temp.size() + std::string(";").size());
                            }
                          tmp = tmp.substr(0, tmp.find(";"));

                          tmp2 = line;
                          for(int i = 0; i < std::get<1> (temptup); i++)
                            {
                              temp = tmp2.substr(0, tmp2.find(";"));
                              tmp2 = tmp2.erase(
                                       0, temp.size() + std::string(";").size());
                            }
                          tmp2 = tmp2.substr(0, tmp2.find(";"));
                          Glib::ustring searchstr(tmp);
                          auto it = std::find_if(
                                      Boards.begin(), Boards.end(), [searchstr]
                                      (auto & element)
                          {
                            return element.first == searchstr;
                          });
                          if(it != Boards.end() && tmp2 != "0")
                            {
                              int index = std::distance(Boards.begin(), it);
                              boardsvect.at(index).push_back(line);
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
              dcount = dcount + 100;
            }
          for(size_t j = 0; j < boardsvect.size(); j++)
            {
              if(boardsvect[j].size() > 0)
                {
                  saveRes(
                    boardsvect[j],
                    Instruments.at(i).second + "-"
                    + Instruments.at(i).first,
                    Boards.at(j).first, tupforsave);
                }
            }
          progress.emit((double) i / (double) Instruments.size());
          for(size_t j = 0; j < boardsvect.size(); j++)
            {
              boardsvect.at(j).clear();
            }
        }
      progress.emit(100);
      saveDate.emit(dt);
      finished.emit();
    }
  else
    {
      saveDate.emit(dt);
      finished.emit();
    }
}

void
DownloadAll::saveRes(std::vector<std::string> &source, Glib::ustring instrname,
                     Glib::ustring boardid,
                     std::tuple<int, int, int, int, int> tupforsave)
{
  std::string dirstr;
  AuxFunc af;
  af.homePath(&dirstr);
  std::filesystem::path dir;
  dirstr = dirstr + "/Money/" + boardid;
  dir = std::filesystem::u8path(dirstr);
  if(!std::filesystem::exists(dir))
    {
      std::filesystem::create_directories(dir);
    }
  mpf_set_default_prec(1024);
  mpf_class Volume(0), Money(0), PSD(0), PSOD(0);
  int linecount = 0;
  double money, volume;
  std::vector<std::string> finalv;
  std::string line, writevalue;
  std::fstream f;
  std::filesystem::path finalpath;
  af.homePath(&line);
  std::string finalpathstr = line + "/Money/" + boardid + "/" + instrname
                             + ".csv";
  finalpath = std::filesystem::u8path(finalpathstr);
  if(!std::filesystem::exists(finalpath))
    {
      finalv.push_back(instrname + "\n");
      finalv.push_back(
        "Дата;Средневзвешенная цена;Первая цена;Последняя цена;Объем, шт.;"
        "Объем оборота денег;ПСОд;ПСд\n");
    }
  else
    {
      std::tuple<int, int> temptup;
      std::string temp;
      f.open(finalpath, std::ios_base::in);
      while(!f.eof())
        {
          getline(f, line);
          if(linecount == 1)
            {
              std::string tmp = line;
              int countch = 0;
              while(tmp.size() > 0)
                {
                  temp = tmp.substr(0, tmp.find(";"));
                  std::string::size_type n;
                  if(temp == "Объем, шт.")
                    {
                      std::get<0> (temptup) = countch;
                    }
                  if(temp == "Объем оборота денег")
                    {
                      std::get<1> (temptup) = countch;
                    }
                  n = tmp.find(";");
                  if(n != std::string::npos)
                    {
                      tmp.erase(0, n + std::string(";").size());
                    }
                  else
                    {
                      break;
                    }
                  countch++;
                }
            }
          if(line != "")
            {
              line = line + "\n";
              finalv.push_back(line);
              if(linecount > 1)
                {
                  std::string tmp;
                  tmp = line;
                  for(int i = 0; i < std::get<0> (temptup); i++)
                    {
                      temp = tmp.substr(0, tmp.find(";"));
                      tmp = tmp.erase(
                              0, temp.size() + std::string(";").size());
                    }
                  tmp = tmp.substr(0, tmp.find(";"));
                  mpf_class tmpvol, tmpmon;
                  std::stringstream strm;
                  std::locale loc("C");
                  strm.imbue(loc);
                  strm << tmp;
                  strm >> tmpvol;

                  tmp = line;
                  for(int i = 0; i < std::get<1> (temptup); i++)
                    {
                      temp = tmp.substr(0, tmp.find(";"));
                      tmp = tmp.erase(
                              0, temp.size() + std::string(";").size());
                    }
                  tmp = tmp.substr(0, tmp.find(";"));
                  strm.str("");
                  strm.clear();
                  strm.imbue(loc);
                  strm << tmp;
                  strm >> tmpmon;
                  Volume = Volume + tmpvol;
                  Money = Money + tmpmon;
                }
            }
          linecount++;
        }
      f.close();
    }
  std::string temp;
  for(size_t i = 0; i < source.size(); i++)
    {
      line = source[i];
      for(int j = 0; j < std::get<2> (tupforsave); j++)
        {
          temp = line.substr(0, line.find(";"));
          line = line.erase(0, temp.size() + std::string(";").size());
        }
      line = line.substr(0, line.find(";"));
      std::stringstream strm;
      std::locale loc("C");
      strm.imbue(loc);
      strm << line;
      int checkl;
      strm >> checkl;

      if(checkl > 0)
        {
          line = source[i];
          for(int j = 0; j < std::get<0> (tupforsave); j++)
            {
              temp = line.substr(0, line.find(";"));
              line = line.erase(0, temp.size() + std::string(";").size());
            }
          line = line.substr(0, line.find(";"));
          std::string year, month, day;
          std::string datestr;
          year = line;
          year = year.substr(0, year.find("-"));
          datestr = year;
          month = line;
          month = month.erase(0, month.find("-") + std::string("-").size());
          month = month.substr(0, month.find("-"));
          datestr = month + "." + datestr;
          if(month.substr(0, 0) == "0")
            {
              month.erase(0);
            }
          day = line;
          day = day.erase(0, day.find("-") + std::string("-").size());
          day = day.erase(0, day.find("-") + std::string("-").size());
          datestr = day + "." + datestr;
          if(day.substr(0, 0) == "0")
            {
              day.erase(0);
            }
          double td = af.grigtojulian(std::stoi(day), std::stoi(month),
                                      std::stoi(year), 0, 0, 0.0);
          writevalue = af.togrigday(td) + "." + af.togrigmonth(td) + "."
                       + af.togrigyear(td) + ";";

          line = source[i];
          for(int j = 0; j < std::get<1> (tupforsave); j++)
            {
              temp = line.substr(0, line.find(";"));
              line = line.erase(0, temp.size() + std::string(";").size());
            }
          line = line.substr(0, line.find(";"));
          strm.str("");
          strm.clear();
          strm.imbue(loc);
          strm << line;
          strm >> money;
          Money = Money + money;

          line = source[i];
          for(int j = 0; j < std::get<2> (tupforsave); j++)
            {
              temp = line.substr(0, line.find(";"));
              line = line.erase(0, temp.size() + std::string(";").size());
            }
          line = line.substr(0, line.find(";"));
          strm.str("");
          strm.clear();
          strm.imbue(loc);
          strm << line;
          strm >> volume;
          Volume = Volume + volume;

          double waprice = money / volume;
          strm.str("");
          strm.clear();
          strm.imbue(loc);
          strm << std::fixed;
          strm << std::setprecision(3);
          strm << waprice;
          strm >> line;
          writevalue = writevalue + line + ";";

          line = source[i];
          for(int j = 0; j < std::get<3> (tupforsave); j++)
            {
              temp = line.substr(0, line.find(";"));
              line = line.erase(0, temp.size() + std::string(";").size());
            }
          line = line.substr(0, line.find(";"));
          writevalue = writevalue + line + ";";

          line = source[i];
          for(int j = 0; j < std::get<4> (tupforsave); j++)
            {
              temp = line.substr(0, line.find(";"));
              line = line.erase(0, temp.size() + std::string(";").size());
            }
          line = line.substr(0, line.find(";"));
          writevalue = writevalue + line + ";";

          strm.str("");
          strm.clear();
          strm.imbue(loc);
          strm << std::fixed;
          strm << std::setprecision(0);
          strm << volume;
          writevalue = writevalue + strm.str() + ";";

          strm.str("");
          strm.clear();
          strm.imbue(loc);
          strm << std::fixed;
          strm << std::setprecision(3);
          strm << money;
          writevalue = writevalue + strm.str() + ";";

          PSOD = mpf_class(volume) / mpf_class(money);
          strm.str("");
          strm.clear();
          strm.imbue(loc);
          strm << std::fixed;
          strm << std::setprecision(22);
          strm << PSOD;
          writevalue = writevalue + strm.str() + ";";

          PSD = Volume / Money;
          strm.str("");
          strm.clear();
          strm.imbue(loc);
          strm << std::fixed;
          strm << std::setprecision(22);
          strm << PSD;
          writevalue = writevalue + strm.str() + "\n";
          auto it = std::find_if(finalv.begin(), finalv.end(), [datestr]
                                 (std::string & element)
          {
            std::string fnd = element;
            fnd = fnd.substr(0, fnd.find(";"));
            return datestr == fnd;
          });
          if(it == finalv.end())
            {
              finalv.push_back(writevalue);
            }
        }
    }
  f.open(finalpath, std::ios_base::out | std::ios_base::binary);
  for(size_t i = 0; i < finalv.size(); i++)
    {
      f.write(finalv[i].c_str(), finalv[i].size());
    }
  f.close();
}
