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

#include "PSDDeals.h"

PSDDeals::PSDDeals(
  std::string file,
  int Height,
  int Width,
  std::vector<std::tuple<std::string, double, double, double, double>> *Plotdate)
{
  filename = std::filesystem::u8path(file);
  height = Height;
  width = Width;
  plotdate = Plotdate;
}

PSDDeals::~PSDDeals()
{
  // TODO Auto-generated destructor stub
}

void
PSDDeals::calcForDraw()
{
  plotdate->clear();
  mpf_set_default_prec(512);
  mpf_class quan, mon, summa, summm;
  std::vector<mpf_class> Quan;
  std::vector<mpf_class> Mon;
  std::fstream f;
  std::string line, midd, temp;
  int count = 0;

  std::filesystem::path p;
  AuxFunc af;
  af.homePath(&line);
  line = line + "/.Money/BoardsList";
  p = af.utf8to(line);
  if(std::filesystem::exists(p))
    {
      f.open(p, std::ios_base::in);
      while(!f.eof())
        {
          getline(f, line);
          if(count > 2 && line != "")
            {
              af.cp1251toutf8(line);
              std::string::size_type nusd, neur;
              neur = line.find("EUR");
              nusd = line.find("USD");
              if(neur != std::string::npos || nusd != std::string::npos)
                {
                  temp = line;
                  temp.erase(0, temp.find(";") + std::string(";").size());
                  temp.erase(0, temp.find(";") + std::string(";").size());
                  temp = temp.substr(0, temp.find(";"));
                  if(neur != std::string::npos)
                    {
                      eurv.push_back(temp);
                    }
                  if(nusd != std::string::npos)
                    {
                      usdv.push_back(temp);
                    }
                }
            }
          count++;
        }
      f.close();
    }
  line = "";
  temp = "";
  count = 0;

  if(!std::filesystem::exists(filename))
    {
      std::cout << "File to plot daily PSD not opened" << std::endl;
    }
  else
    {
      std::tuple<int, int, int> temptup;
      f.open(filename, std::ios_base::in);
      while(!f.eof())
        {
          getline(f, line);
          if(count == 2)
            {
              midd = line;
              int countch = 0;
              while(midd.size() > 0)
                {
                  temp = midd.substr(0, midd.find(";"));
                  std::string::size_type n;
                  if(temp == "TRADETIME")
                    {
                      std::get<0> (temptup) = countch;
                    }
                  if(temp == "VALUE")
                    {
                      std::get<1> (temptup) = countch;
                    }
                  if(temp == "PRICE")
                    {
                      std::get<2> (temptup) = countch;
                    }
                  n = midd.find(";");
                  if(n != std::string::npos)
                    {
                      midd.erase(0, n + std::string(";").size());
                    }
                  else
                    {
                      break;
                    }
                  countch++;
                }
            }
          if(count > 2 && line != "")
            {
              if(count == 3)
                {
                  yname = line;
                  yname.erase(0, yname.find(";") + std::string(";").size());
                  yname.erase(0, yname.find(";") + std::string(";").size());
                  yname = yname.substr(0, yname.find(";"));
                }

              midd = line;
              temp = line;
              for(int i = 0; i < std::get<0> (temptup); i++)
                {
                  temp = midd.substr(0, midd.find(";"));
                  midd = midd.erase(0,
                                    temp.size() + std::string(";").size());
                }
              midd = midd.substr(0, midd.find(";"));
              std::tuple<std::string, double, double, double, double> ttup;
              std::get<0> (ttup) = midd;

              midd = line;
              temp = line;
              for(int i = 0; i < std::get<1> (temptup); i++)
                {
                  temp = midd.substr(0, midd.find(";"));
                  midd = midd.erase(0,
                                    temp.size() + std::string(";").size());
                }
              midd = midd.substr(0, midd.find(";"));
              std::stringstream strm;
              std::locale loc("C");
              strm.imbue(loc);
              strm << midd;
              strm >> mon;
              Mon.push_back(mon);

              midd = line;
              temp = line;
              for(int i = 0; i < std::get<2> (temptup); i++)
                {
                  temp = midd.substr(0, midd.find(";"));
                  midd = midd.erase(0,
                                    temp.size() + std::string(";").size());
                }
              midd = midd.substr(0, midd.find(";"));
              strm.str("");
              strm.clear();
              strm.imbue(loc);
              strm << midd;
              strm >> quan;
              Quan.push_back(mon / quan);
              plotdate->push_back(ttup);
            }
          count = count + 1;
        }
      f.close();
    }
  for(size_t i = 0; i < Quan.size(); i++)
    {
      DC.push_back(Quan[i] / Mon[i]);
      summa = summa + Quan[i];
      summm = summm + Mon[i];
      TC.push_back(summa / summm);
    }

  for(size_t i = 0; i < TC.size(); i++)
    {
      Tc.push_back(TC[i].get_d());
      Dc.push_back(DC[i].get_d());
      std::get<2> (plotdate->at(i)) = Dc[i];
      std::get<1> (plotdate->at(i)) = Tc[i];
    }

  if(plotdate->size() > 0)
    {
      datebeg = std::get<0> (plotdate->at(0));
      dateend = std::get<0> (plotdate->at(plotdate->size() - 1));
    }
}

int
PSDDeals::Draw(mglGraph *gr)
{
  calcForDraw();
  std::vector<int> X;
  for(size_t i = 0; i < Tc.size(); i++)
    {
      X.push_back(i);
    }
  mglData x(X), y(Tc), y1(Dc);

  int d = 6;
  int number = X.size();
  while(number % d > 0)
    {
      number = number + 1;
    }
  d = number / d;

  //Координаты подписей оси х
  AuxFunc af;
  std::string grnm = gettext("Purchasing power of money");
  grnm = af.utf8to(grnm);

  //Общие настройки графика
  gr->SetObjId(31);
  gr->SetSize(width, height);
  gr->Title(grnm.c_str(), "", 5);
  gr->SetQuality(3);
  gr->SetRanges(x, y1);
  gr->SetFontSize(3);
  gr->SetOriginTick(false);

  std::vector<double> ticks;
  double tickstep = (y1.Maximal() - y1.Minimal()) / 3;
  if(tickstep < 0)
    {
      tickstep = -tickstep;
    }
  double tickval = y1.Minimal();
  std::stringstream strm;
  std::locale loc("C");
  std::string tickstr, tick;
  for(int i = 0; i < 2; i++)
    {
      strm.str("");
      strm.clear();
      strm.imbue(loc);
      strm << std::fixed;
      strm << std::setprecision(5);
      tickval = tickval + tickstep;
      strm << tickval;
      tick = strm.str();
      ticks.push_back(tickval);
      if(tickstr != "")
        {
          tickstr = tickstr + "\n" + tick;
        }
      else
        {
          tickstr = tick;
        }
    }
  mglData fortick(ticks);
  tickstr = af.utf8to(tickstr);

  gr->SetTicksVal('y', fortick, tickstr.c_str());
  gr->SetTicks('x', d);
  gr->SetTickSkip(true);
  gr->Axis("y", "k");
  gr->Axis("!f0x", "k");
  auto itusd = std::find(usdv.begin(), usdv.end(), yname);
  if(itusd != usdv.end())
    {
      yname = gettext("Shares/USD");
    }
  auto iteur = std::find(eurv.begin(), eurv.end(), yname);
  if(iteur != eurv.end())
    {
      yname = gettext("Shares/EUR");
    }
  if(itusd == usdv.end() && iteur == eurv.end())
    {
      yname = gettext("Shares/Rouble");
    }
  gr->Label('x', af.utf8to(gettext("Transactions")).c_str(), 0);
  gr->Label('y', af.utf8to(yname).c_str(), 0);

  //Отображение графика
  gr->Grid("xy", "{xA0A136}");
  gr->Plot(x, y, "r");
  gr->Plot(x, y1, "g");
  gr->AddLegend(af.utf8to(gettext("PPm")).c_str(), "r");
  gr->AddLegend(af.utf8to(gettext("PPTm")).c_str(), "g");
  gr->SetFontSize(3);
  gr->Legend(1.3, 1.3);

  //Подписи оси х
  mglPoint p1(x.Minimal(),
              y1.Maximal() + ((y1.Maximal() - y1.Minimal()) * 0.02));
  mglPoint p5(x.Maximal(),
              y1.Maximal() + ((y1.Maximal() - y1.Minimal()) * 0.02));
  datebeg = af.utf8to(datebeg);
  dateend = af.utf8to(dateend);
  gr->Puts(p1, datebeg.c_str(), "k", 3);
  gr->Puts(p5, dateend.c_str(), "k", 3);

  return 0;
}

