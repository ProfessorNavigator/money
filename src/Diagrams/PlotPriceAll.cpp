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

#include "PlotPriceAll.h"

PlotPriceAll::PlotPriceAll(
    std::string file,
    int Height,
    int Width,
    std::vector<std::tuple<std::string, double, double, double, double>> *Plotdate)
{
  filepath = std::filesystem::u8path(file);
  height = Height;
  width = Width;
  plotdate = Plotdate;
  calcForDraw();
}

PlotPriceAll::~PlotPriceAll()
{
  // TODO Auto-generated destructor stub
}

void
PlotPriceAll::calcForDraw()
{
  plotdate->clear();
  mpf_set_default_prec(512);
  std::fstream f;

  std::string line, midd, temp;
  int count = 0;

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

  f.open(filepath, std::ios_base::in);
  if(!f.is_open())
    {
      std::cout << "File for plot price (all) not opened" << std::endl;
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
	      for(int i = 0; i < 1; i++)
		{
		  temp = midd.substr(0, midd.find(";"));
		  midd.erase(0, temp.size() + std::string(";").size());
		}
	      midd = midd.substr(0, midd.find(";"));
	      double tmpdouble;
	      std::locale loc("C");
	      std::stringstream strm;
	      strm.imbue(loc);
	      strm << midd;
	      strm >> tmpdouble;
	      Pricemid.push_back(tmpdouble);
	      std::get<1>(ttup) = tmpdouble;

	      midd = line;
	      temp = line;
	      for(int i = 0; i < 2; i++)
		{
		  temp = midd.substr(0, midd.find(";"));
		  midd.erase(0, temp.size() + std::string(";").size());
		}
	      midd = midd.substr(0, midd.find(";"));
	      strm.str("");
	      strm.clear();
	      strm.imbue(loc);
	      strm << midd;
	      strm >> tmpdouble;
	      Pricebeg.push_back(tmpdouble);
	      std::get<2>(ttup) = tmpdouble;

	      midd = line;
	      temp = line;
	      for(int i = 0; i < 3; i++)
		{
		  temp = midd.substr(0, midd.find(";"));
		  midd.erase(0, temp.size() + std::string(";").size());
		}
	      midd = midd.substr(0, midd.find(";"));
	      strm.str("");
	      strm.clear();
	      strm.imbue(loc);
	      strm << midd;
	      strm >> tmpdouble;
	      Priceend.push_back(tmpdouble);
	      std::get<3>(ttup) = tmpdouble;

	      midd = line;
	      temp = line;
	      for(int i = 0; i < 7; i++)
		{
		  temp = midd.substr(0, midd.find(";"));
		  midd.erase(0, temp.size() + std::string(";").size());
		}
	      midd = midd.substr(0, midd.find(";"));
	      strm.str("");
	      strm.clear();
	      strm.imbue(loc);
	      strm << midd;
	      strm >> tmpdouble;
	      Tc.push_back(1 / tmpdouble);
	      std::get<4>(ttup) = 1 / tmpdouble;
	      plotdate->push_back(ttup);
	    }

	  count = count + 1;
	}
      f.close();
    }
}

void
PlotPriceAll::cleanVectors(double dateb, double datee)
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

  size_t sz = Pricemid.size() - plotdate->size();

  if(sz > 0)
    {
      Pricemid.erase(Pricemid.begin(), Pricemid.begin() + sz);
      Pricebeg.erase(Pricebeg.begin(), Pricebeg.begin() + sz);
      Priceend.erase(Priceend.begin(), Priceend.begin() + sz);
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

  sz = Pricemid.size() - plotdate->size();
  if(sz > 0)
    {
      Pricemid.erase(Pricemid.end() - sz, Pricemid.end());
      Pricebeg.erase(Pricebeg.end() - sz, Pricebeg.end());
      Priceend.erase(Priceend.end() - sz, Priceend.end());
      Tc.erase(Tc.end() - sz, Tc.end());
    }
  if(plotdate->size() > 0)
    {
      datebeg = std::get<0>(plotdate->at(0));
      dateend = std::get<0>(plotdate->at(plotdate->size() - 1));
    }
}

int
PlotPriceAll::Draw(mglGraph *gr)
{
  std::vector<int> X;
  std::vector<double> comp;
  for(size_t i = 0; i < Pricemid.size(); i++)
    {
      X.push_back(i);
    }
  mglData x(X), y(Pricemid), y1(Pricebeg), y2(Priceend), y3(Tc);

  comp.push_back(y.Maximal());
  comp.push_back(y1.Maximal());
  comp.push_back(y2.Maximal());
  comp.push_back(y.Minimal());
  comp.push_back(y1.Minimal());
  comp.push_back(y2.Minimal());

  double maxval = *std::max_element(comp.begin(), comp.end());
  double minval = *std::min_element(comp.begin(), comp.end());

  mglPoint p1(x.Minimal(), maxval + (maxval - minval) * 0.02);
  mglPoint p5(x.Maximal(), maxval + (maxval - minval) * 0.02);

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
	  mglPoint p(i, maxval + (maxval - minval) * 0.02);
	  Coord.push_back(p);
	  dates.push_back(std::get<0>(plotdate->at(i)));
	}
    }

  AuxFunc af;
  std::string grnm = gettext("Price");
  grnm = af.utf8to(grnm);

  gr->SetObjId(11);
  gr->SetSize(width, height);
  gr->Title(grnm.c_str(), "", 5);
  gr->SetQuality(3);
  gr->SetRanges(x.Minimal(), x.Maximal(), minval, maxval);
  gr->SetFontSize(3);
  gr->SetOriginTick(false);

  std::vector<double> ticks;
  double tickstep = (maxval - minval) / 3;
  if(tickstep < 0)
    {
      tickstep = -tickstep;
    }
  double tickval = minval;
  std::stringstream strm;
  std::locale loc("C");
  std::string tickstr, tick;
  for(int i = 0; i < 2; i++)
    {

      strm.str("");
      strm.clear();
      strm.imbue(loc);
      strm << std::fixed;
      strm << std::setprecision(3);
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
  gr->Grid("xy", "{xA0A136}");
  gr->SetTickSkip(true);
  gr->Axis("y", "k");
  gr->Axis("!f0x", "k");
  std::string yname;
  yname = filepath.parent_path().filename().u8string();
  auto itusd = std::find(usdv.begin(), usdv.end(), yname);
  if(itusd != usdv.end())
    {
      yname = "USD";
    }
  auto iteur = std::find(eurv.begin(), eurv.end(), yname);
  if(iteur != eurv.end())
    {
      yname = "EUR";
    }
  auto itcny = std::find(cnyv.begin(), cnyv.end(), yname);
  if(itcny != cnyv.end())
    {
      yname = "CNY";
    }
  if(itusd == usdv.end() && iteur == eurv.end() && itcny == cnyv.end())
    {
      yname = gettext("Roubles");
    }
  gr->Label('x', af.utf8to(gettext("Days")).c_str(), 0);
  gr->Label('y', af.utf8to(yname).c_str(), 0);
  datebeg = af.utf8to(datebeg);
  dateend = af.utf8to(dateend);

  gr->Puts(p1, datebeg.c_str(), "k", 3);
  gr->Puts(p5, dateend.c_str(), "k", 3);

  for(size_t i = 0; i < dates.size(); i++)
    {
      std::string tmpl = dates[i];
      tmpl = af.utf8to(tmpl);
      gr->Puts(Coord[i], tmpl.c_str(), "k", 3);
    }

  gr->Plot(x, y, "r");
  gr->Plot(x, y1, "b");
  gr->Plot(x, y2, "g");
  gr->Plot(x, y3, "k");
  gr->AddLegend(af.utf8to(gettext("Weighted average price")).c_str(), "r");
  gr->AddLegend(af.utf8to(gettext("First")).c_str(), "b");
  gr->AddLegend(af.utf8to(gettext("Last")).c_str(), "g");
  gr->AddLegend(af.utf8to(gettext("ECg")).c_str(), "k");
  gr->SetFontSize(3);
  gr->Legend(1.3, 1.5);

  return 0;
}

