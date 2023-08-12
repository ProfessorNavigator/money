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

#include "PSD.h"

PSD::PSD(
    std::string file,
    int Height,
    int Width,
    std::vector<std::tuple<std::string, double, double, double, double>> *Plotdate)
{
  filename = std::filesystem::u8path(file);
  height = Height;
  width = Width;
  plotdate = Plotdate;
  calcForDraw();
}

PSD::~PSD()
{
  // TODO Auto-generated destructor stub
}

void
PSD::calcForDraw()
{
  plotdate->clear();
  mpf_set_default_prec(512);
  std::fstream f;
  int count = 0;
  std::string line, midd, temp;

  std::filesystem::path p;
  AuxFunc af;
  af.homePath(&line);
  line = line + "/.Money/BoardsList";
  p = af.utf8to(line);
  f.open(p, std::ios_base::in);
  if(f.is_open())
    {
      while(!f.eof())
	{
	  getline(f, line);
	  if(count > 0 && !line.empty())
	    {
	      std::string::size_type nusd, neur, ncny;
	      neur = line.find("EUR");
	      nusd = line.find("USD");
	      ncny = line.find("CNY");
	      if(neur != std::string::npos || nusd != std::string::npos
		  || ncny != std::string::npos)
		{
		  temp = line;
		  temp.erase(0, temp.find(";") + std::string(";").size());
		  temp.erase(0, temp.find(";") + std::string(";").size());
		  temp = temp.substr(0, temp.find(";"));
		  if(neur != std::string::npos)
		    {
		      eurv.push_back(temp);
		    }
		  else if(nusd != std::string::npos)
		    {
		      usdv.push_back(temp);
		    }
		  else if(ncny != std::string::npos)
		    {
		      cnyv.push_back(temp);
		    }
		}
	    }
	  count++;
	}
      f.close();
    }
  line.clear();
  temp.clear();
  count = 0;

  f.open(filename, std::ios_base::in);
  if(!f.is_open())
    {
      std::cout << "File to plot price(All) not opened" << std::endl;
    }
  else
    {
      while(!f.eof())
	{
	  getline(f, line);

	  if(count > 1 && !line.empty())
	    {
	      midd = line;
	      midd = midd.substr(0, midd.find(";"));
	      std::tuple<std::string, double, double, double, double> ttup;
	      std::get<0>(ttup) = midd;

	      midd = line;
	      temp = line;
	      for(int i = 0; i < 7; i++)
		{

		  temp = midd.substr(0, midd.find(";"));
		  midd = midd.erase(0, temp.size() + std::string(";").size());
		}
	      midd = midd.substr(0, midd.find(";"));
	      std::stringstream strm;
	      std::locale loc("C");
	      strm.imbue(loc);
	      double tmpdouble;
	      strm << midd;
	      strm >> tmpdouble;
	      Tc.push_back(tmpdouble);
	      std::get<1>(ttup) = tmpdouble;

	      midd = line;
	      temp = line;
	      for(int i = 0; i < 6; i++)
		{
		  temp = midd.substr(0, midd.find(";"));
		  midd = midd.erase(0, temp.size() + std::string(";").size());
		}
	      midd = midd.substr(0, midd.find(";"));
	      strm.str("");
	      strm.clear();
	      strm.imbue(loc);
	      strm << midd;
	      strm >> tmpdouble;
	      Dc.push_back(tmpdouble);
	      std::get<2>(ttup) = tmpdouble;
	      plotdate->push_back(ttup);
	    }

	  count = count + 1;
	}
      f.close();
    }
}

void
PSD::cleanVectors(double dateb, double datee)
{
  std::locale loc("C");
  plotdate->erase(
      std::remove_if(plotdate->begin(), plotdate->end(), [dateb, loc]
      (auto &el)
	{
	  std::string val = std::get<0>(el);
	  val = val.substr(0, val.find("."));

	  std::stringstream strm;
	  strm.imbue(loc);
	  strm << val;
	  int day;
	  strm >> day;

	  val = std::get<0>(el);
	  val.erase(0, val.find(".") + std::string(".").size());
	  val = val.substr(0, val.find("."));
	  strm.clear();
	  strm.str("");
	  strm.imbue(loc);
	  strm << val;
	  int month;
	  strm >> month;

	  val = std::get<0>(el);
	  val.erase(0, val.rfind(".") + std::string(".").size());
	  strm.clear();
	  strm.str("");
	  strm.imbue(loc);
	  strm << val;
	  int year;
	  strm >> year;

	  AuxFunc af;
	  double JD = af.grigtojulian(day, month, year, 0, 0, 0.0);

	  return JD < dateb;
	}),
      plotdate->end());

  size_t sz = Dc.size() - plotdate->size();

  if(sz > 0)
    {
      Dc.erase(Dc.begin(), Dc.begin() + sz);
      Tc.erase(Tc.begin(), Tc.begin() + sz);
    }

  plotdate->erase(
      std::remove_if(plotdate->begin(), plotdate->end(), [datee, loc]
      (auto &el)
	{
	  std::string val = std::get<0>(el);
	  val = val.substr(0, val.find("."));

	  std::stringstream strm;
	  strm.imbue(loc);
	  strm << val;
	  int day;
	  strm >> day;

	  val = std::get<0>(el);
	  val.erase(0, val.find(".") + std::string(".").size());
	  val = val.substr(0, val.find("."));
	  strm.clear();
	  strm.str("");
	  strm.imbue(loc);
	  strm << val;
	  int month;
	  strm >> month;

	  val = std::get<0>(el);
	  val.erase(0, val.rfind(".") + std::string(".").size());
	  strm.clear();
	  strm.str("");
	  strm.imbue(loc);
	  strm << val;
	  int year;
	  strm >> year;

	  AuxFunc af;
	  double JD = af.grigtojulian(day, month, year, 0, 0, 0.0);

	  return JD > datee;
	}),
      plotdate->end());

  sz = Dc.size() - plotdate->size();
  if(sz > 0)
    {
      Dc.erase(Dc.end() - sz, Dc.end());
      Tc.erase(Tc.end() - sz, Tc.end());
    }
  if(plotdate->size() > 0)
    {
      datebeg = std::get<0>(plotdate->at(0));
      dateend = std::get<0>(plotdate->at(plotdate->size() - 1));
    }
}

int
PSD::Draw(mglGraph *gr)
{
  std::vector<int> X;
  for(size_t i = 0; i < Tc.size(); i++)
    {
      X.push_back(i);
    }
  mglData x(X), y(Tc), y1(Dc);
  double miny, maxy;
  if(y.Minimal() <= y1.Minimal())
    {
      miny = y.Minimal();
    }
  else
    {
      miny = y1.Minimal();
    }

  if(y.Maximal() >= y1.Maximal())
    {
      maxy = y.Maximal();
    }
  else
    {
      maxy = y1.Maximal();
    }
  //Координаты подписей оси х
  mglPoint p1(x.Minimal(), maxy + ((maxy - miny) * 0.02));
  mglPoint p5(x.Maximal(), maxy + ((maxy - miny) * 0.02));

  AuxFunc af;
  std::string grnm = gettext("Purchasing power of money");
  grnm = af.utf8to(grnm);

  int d = 6;
  int number = X.size();
  while(number % d > 0)
    {
      number = number + 1;
    }
  d = number / d;

  std::vector<mglPoint> Coord;
  std::vector<std::string> dates;
  for(size_t i = 0; i < X.size(); i = i + d)
    {
      if(i > 0)
	{
	  mglPoint p(i, maxy + ((maxy - miny) * 0.02));
	  Coord.push_back(p);
	  dates.push_back(std::get<0>(plotdate->at(i)));
	}
    }
  //Общие настройки графика
  gr->SetObjId(31);
  gr->SetSize(width, height);
  gr->Title(grnm.c_str(), "", 5);
  gr->SetQuality(3);
  gr->SetRanges(x.Minimal(), x.Maximal(), miny, maxy);
  gr->SetFontSize(3);
  gr->SetOriginTick(false);

  std::vector<double> ticks;
  double tickstep = (maxy - miny) / 3;
  if(tickstep < 0)
    {
      tickstep = -tickstep;
    }
  double tickval = miny;
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
      if(!tickstr.empty())
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
  gr->Grid("xy", "{xA0A136}");
  gr->Axis("y", "k");
  gr->Axis("x!f0", "k");
  std::string yname;
  yname = filename.parent_path().filename().u8string();
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
  auto itcny = std::find(cnyv.begin(), cnyv.end(), yname);
  if(itcny != cnyv.end())
    {
      yname = gettext("Shares/CNY");
    }
  if(itusd == usdv.end() && iteur == eurv.end() && itcny == cnyv.end())
    {
      yname = gettext("Shares/Rouble");
    }
  gr->Label('x', af.utf8to(gettext("Days")).c_str(), 0);
  gr->Label('y', af.utf8to(yname).c_str(), 0);

  //Подписи оси х
  datebeg = af.utf8to(datebeg);
  dateend = af.utf8to(dateend);
  gr->Puts(p1, datebeg.c_str(), "k", 3);
  gr->Puts(p5, dateend.c_str(), "k", 3);
  for(size_t i = 0; i < dates.size(); i++)
    {
      std::string tmstr = dates[i];
      tmstr = af.utf8to(tmstr);
      gr->Puts(Coord[i], tmstr.c_str(), "k", 3);
    }

  //Отображение графика
  gr->Plot(x, y, "r");
  gr->Plot(x, y1, "g");
  gr->AddLegend(af.utf8to(gettext("PPm")).c_str(), "r");
  gr->AddLegend(af.utf8to(gettext("PPTm")).c_str(), "g");
  gr->SetFontSize(3);
  gr->Legend(1.1, 1.3);
  return 0;
}
