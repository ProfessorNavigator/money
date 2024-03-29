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

#include "PlotAllCommon.h"

PlotAllCommon::PlotAllCommon(
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

PlotAllCommon::~PlotAllCommon()
{
  // TODO Auto-generated destructor stub
}

void
PlotAllCommon::calcForDraw()
{
  plotdate->clear();
  mpf_set_default_prec(512);
  std::fstream f;
  int count = 0;
  std::string line, midd, temp;
  std::vector<double> volume;
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
      std::cout << "File to plot common graphics (All) not opened" << std::endl;
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
	      double tmpd;
	      strm << midd;
	      strm >> tmpd;
	      Tc.push_back(tmpd);
	      std::get<1>(ttup) = tmpd;

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
	      strm >> tmpd;
	      Dc.push_back(tmpd);
	      std::get<2>(ttup) = tmpd;

	      midd = line;
	      temp = line;
	      for(int i = 0; i < 4; i++)
		{
		  temp = midd.substr(0, midd.find(";"));
		  midd = midd.erase(0, temp.size() + std::string(";").size());
		}
	      midd = midd.substr(0, midd.find(";"));
	      strm.str("");
	      strm.clear();
	      strm.imbue(loc);
	      strm << midd;
	      strm >> tmpd;
	      volume.push_back(tmpd);
	      plotdate->push_back(ttup);
	    }

	  count = count + 1;
	}
      f.close();
    }

  for(size_t i = 0; i < Tc.size(); i++)
    {
      Ot.push_back((Dc[i] / Tc[i] - 1) * 100);
      std::get<3>(plotdate->at(i)) = Ot[i];
    }

  mpf_class summ(0);
  for(size_t i = 0; i < volume.size(); i++)
    {
      summ = summ + volume[i];
      mpf_class dn = static_cast<double>(i + 1);
      mpf_class res = summ / dn;
      sharesvol.push_back(res.get_d());
      std::get<4>(plotdate->at(i)) = res.get_d();
    }
  midd = filename.filename().u8string();
  std::string::size_type n;
  n = midd.find(".csv", 0);
  if(n != std::string::npos)
    {
      midd.erase(n, std::string(".csv").size());
    }
}

void
PlotAllCommon::cleanVectors(double dateb, double datee)
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

  size_t sz = Tc.size() - plotdate->size();

  if(sz > 0)
    {
      Tc.erase(Tc.begin(), Tc.begin() + sz);
      Dc.erase(Dc.begin(), Dc.begin() + sz);
      Ot.erase(Ot.begin(), Ot.begin() + sz);
      sharesvol.erase(sharesvol.begin(), sharesvol.begin() + sz);
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

  sz = Tc.size() - plotdate->size();
  if(sz > 0)
    {
      Tc.erase(Tc.end() - sz, Tc.end());
      Dc.erase(Dc.end() - sz, Dc.end());
      Ot.erase(Ot.end() - sz, Ot.end());
      sharesvol.erase(sharesvol.end() - sz, sharesvol.end());
    }
  if(plotdate->size() > 0)
    {
      datebeg = std::get<0>(plotdate->at(0));
      dateend = std::get<0>(plotdate->at(plotdate->size() - 1));
    }
}

int
PlotAllCommon::Draw(mglGraph *gr)
{
  std::vector<int> X;
  for(size_t i = 0; i < Tc.size(); i++)
    {
      X.push_back(i);
    }
  mglData x(X), y(Tc), y1(Dc);

  double miny1, maxy1;
  if(y.Minimal() <= y1.Minimal())
    {
      miny1 = y.Minimal();
    }
  else
    {
      miny1 = y1.Minimal();
    }
  if(y.Maximal() >= y1.Maximal())
    {
      maxy1 = y.Maximal();
    }
  else
    {
      maxy1 = y1.Maximal();
    }

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
	  mglPoint p(i, maxy1 + ((maxy1 - miny1) * 0.02));
	  Coord.push_back(p);
	  dates.push_back(std::get<0>(plotdate->at(i)));
	}
    }
  std::string grnm = gettext("Purchasing power of money");
  AuxFunc af;
  grnm = af.utf8to(grnm);

  //Общие настройки графика
  gr->SetSize(width, height);
  gr->SetObjId(21);
  gr->SubPlot(1, 3, 0, ">^_");
  gr->Title(grnm.c_str(), "", 5);
  gr->SetQuality(3);
  gr->SetRanges(x.Minimal(), x.Maximal(), miny1, maxy1);
  gr->SetFontSize(3);
  gr->SetOriginTick(false);

  std::vector<double> ticks;
  double tickstep = (maxy1 - miny1) / 3;
  if(tickstep < 0)
    {
      tickstep = -tickstep;
    }
  double tickval = miny1;
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
  gr->Axis("!f0x", "k");
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
  mglPoint p1(x.Minimal(), maxy1 + ((maxy1 - miny1) * 0.02));
  mglPoint p5(x.Maximal(), maxy1 + ((maxy1 - miny1) * 0.02));
  gr->Puts(p1, datebeg.c_str(), "k", 3);
  gr->Puts(p5, dateend.c_str(), "k", 3);

  for(size_t i = 0; i < dates.size(); i++)
    {
      std::string tl = dates[i];
      gr->Puts(Coord[i], tl.c_str(), "k", 3);
    }

  //Отображение графика
  gr->Plot(x, y, "r");
  gr->Plot(x, y1, "g");
  gr->AddLegend(af.utf8to(gettext("PPm")).c_str(), "r");
  gr->AddLegend(af.utf8to(gettext("PPTm")).c_str(), "g");
  gr->SetFontSize(3);
  gr->Legend(1.1, 1.3);
  gr->ClearLegend();

  std::vector<int> X1;
  std::vector<int> Zer;
  for(size_t i = 0; i < Ot.size(); i++)
    {
      X1.push_back(i);
      Zer.push_back(0);
    }
  mglData x1(X1), y11(Ot), y12(Zer);
  double miny2, maxy2;
  if(y11.Minimal() <= y12.Minimal())
    {
      miny2 = y11.Minimal();
    }
  else
    {
      miny2 = y12.Minimal();
    }
  if(y11.Maximal() >= y12.Maximal())
    {
      maxy2 = y11.Maximal();
    }
  else
    {
      maxy2 = y12.Maximal();
    }

  //Координаты подписей оси х
  mglPoint p11(x1.Minimal(), maxy2 + ((maxy2 - miny2) * 0.02));
  mglPoint p51(x.Maximal(), maxy2 + ((maxy2 - miny2) * 0.02));
  Coord.clear();
  for(size_t i = 0; i < X1.size(); i = i + d)
    {
      if(i > 0)
	{
	  mglPoint p(i, maxy2 + ((maxy2 - miny2) * 0.02));
	  Coord.push_back(p);
	}
    }

  grnm = gettext("PPTm/PPm in %");
  grnm = af.utf8to(grnm);

  //Общие настройки графика
  gr->SetObjId(21);
  gr->SubPlot(1, 3, 1, ">^_");
  gr->Title(grnm.c_str(), "", 5);
  gr->SetQuality(3);
  gr->SetRanges(x1.Minimal(), x1.Maximal(), miny2, maxy2);
  gr->SetFontSize(3);
  gr->SetOriginTick(false);

  ticks.clear();
  tickstep = (maxy2 - miny2) / 3;
  if(tickstep < 0)
    {
      tickstep = -tickstep;
    }
  tickval = miny2;
  tickstr.clear();
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
  mglData fortick2(ticks);
  tickstr = af.utf8to(tickstr);
  gr->SetTicksVal('y', fortick2, tickstr.c_str());
  gr->SetTicks('x', d);
  gr->SetTickSkip(true);
  gr->Grid("xy", "{xA0A136}");
  gr->Axis("y", "k");
  gr->Axis("!f0x", "k");
  gr->Label('x', af.utf8to(gettext("Days")).c_str(), 0);
  gr->Label('y', af.utf8to(gettext("PPTm/PPm (%)")).c_str(), 0);

  //Подписи оси х
  gr->Puts(p11, datebeg.c_str(), "k", 3);
  gr->Puts(p51, dateend.c_str(), "k", 3);

  for(size_t i = 0; i < dates.size(); i++)
    {
      std::string tl = dates[i];
      gr->Puts(Coord[i], tl.c_str(), "k", 3);
    }

  //Отображение графика
  gr->Plot(x1, y12, "b");
  gr->Plot(x1, y11, "r");
  gr->SetFontSize(3);
  gr->AddLegend(af.utf8to(gettext("PPTm/PPm (%)")).c_str(), "r");
  gr->Legend(1.1, 1.3);
  gr->ClearLegend();

  mglData x3(X), y3(sharesvol);
  Coord.clear();
  for(size_t i = 0; i < X1.size(); i = i + d)
    {
      if(i > 0)
	{
	  mglPoint p(i, y3.Maximal() + ((y3.Maximal() - y3.Minimal()) * 0.02));
	  Coord.push_back(p);
	}
    }
  mglPoint p31(x3.Minimal(),
	       y3.Maximal() + ((y3.Maximal() - y3.Minimal()) * 0.02));
  mglPoint p32(x3.Maximal(),
	       y3.Maximal() + ((y3.Maximal() - y3.Minimal()) * 0.02));

  grnm = gettext("Average shares volume per day");
  grnm = af.utf8to(grnm);

  //Общие настройки графика
  gr->SetObjId(21);
  gr->SubPlot(1, 3, 2, ">^_");
  gr->Title(grnm.c_str(), "", 5);
  gr->SetQuality(3);
  gr->SetRanges(x3, y3);
  gr->SetFontSize(3);
  gr->SetOriginTick(false);

  ticks.clear();
  tickstep = (y3.Maximal() - y3.Minimal()) / 3;
  if(tickstep < 0)
    {
      tickstep = -tickstep;
    }
  tickval = y3.Minimal();
  tickstr.clear();
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
  mglData fortick3(ticks);
  tickstr = af.utf8to(tickstr);
  gr->SetTicksVal('y', fortick3, tickstr.c_str());
  gr->SetTicks('x', d);
  gr->SetTickSkip(true);
  gr->Grid("xy", "{xA0A136}");
  gr->Axis("y", "k");
  gr->Axis("!f0x", "k");
  gr->Label('x', af.utf8to(gettext("Days")).c_str(), 0);
  gr->Label('y', af.utf8to(gettext("Shares quantity")).c_str(), 0);

  //Подписи оси х
  gr->Puts(p31, datebeg.c_str(), "k", 3);
  gr->Puts(p32, dateend.c_str(), "k", 3);

  for(size_t i = 0; i < dates.size(); i++)
    {
      std::string tl = dates[i];
      gr->Puts(Coord[i], tl.c_str(), "k", 3);
    }

  //Отображение графика
  gr->Plot(x3, y3, "r");
  gr->SetFontSize(3);
  gr->AddLegend(af.utf8to(gettext("Shares quantity")).c_str(), "r");
  gr->Legend(1.1, 1.3);
  gr->ClearLegend();

  return 0;
}
