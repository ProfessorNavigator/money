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

#include "PlotVolumeGlobal.h"

PlotVolumeGlobal::PlotVolumeGlobal(
    std::string file,
    int Height,
    int Width,
    std::vector<std::tuple<std::string, double, double, double, double>> *Plotdate,
    std::string Variant)
{
  filename = std::filesystem::u8path(file);
  height = Height;
  width = Width;
  plotdate = Plotdate;
  this->Variant = Variant;
}

PlotVolumeGlobal::~PlotVolumeGlobal()
{
  // TODO Auto-generated destructor stub
}

void
PlotVolumeGlobal::calcForDraw()
{
  plotdate->clear();
  std::string line, temp;
  int count = 0;
  std::fstream f;
  f.open(filename, std::ios_base::in);
  if(!f.is_open())
    {
      std::cout << "Global PSD file to plot volume not opened" << std::endl;
    }
  else
    {
      while(!f.eof())
	{
	  getline(f, line);
	  if(count > 0 && !line.empty())
	    {
	      temp = line;
	      temp.erase(0, temp.find(";") + std::string(";").size());
	      temp = temp.substr(0, temp.find(";"));
	      std::stringstream strm;
	      std::locale loc("C");
	      double tmpd;
	      strm.imbue(loc);
	      strm << temp;
	      strm >> tmpd;
	      std::tuple<std::string, double, double, double, double> ttup;
	      Volume.push_back(tmpd);
	      std::get<1>(ttup) = tmpd;

	      temp = line;
	      temp = temp.substr(0, temp.find(";"));
	      std::get<0>(ttup) = temp;
	      plotdate->push_back(ttup);
	    }
	  count = count + 1;
	}
      f.close();
    }
  mpf_class summ(0);
  mpf_class vols(0);
  mpf_class res(0);
  for(size_t i = 0; i < Volume.size(); i++)
    {
      summ = summ + 1;
      vols = vols + Volume[i];
      res = vols / summ;
      Volmid.push_back(res.get_d());
      std::get<2>(plotdate->at(i)) = Volmid[i];
    }
  if(Variant.empty())
    {
      Variant = "all";
    }
  if(Variant != "all")
    {
      cleanVectors();
    }
  if(plotdate->size() > 0)
    {
      datebeg = std::get<0>(plotdate->at(0));
      dateend = std::get<0>(plotdate->at(plotdate->size() - 1));
    }
}

void
PlotVolumeGlobal::cleanVectors()
{
  std::string bstr = Variant.substr(0, Variant.find(" "));
  std::string endstr = Variant;
  endstr.erase(0, endstr.find(" ") + std::string(" ").size());
  std::stringstream strm;
  std::locale loc("C");
  strm.imbue(loc);
  double dateb;
  strm << bstr;
  strm >> dateb;

  strm.clear();
  strm.str("");
  strm.imbue(loc);
  double datee;
  strm << endstr;
  strm >> datee;
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

  size_t sz = Volume.size() - plotdate->size();

  if(sz > 0)
    {
      Volume.erase(Volume.begin(), Volume.begin() + sz);
      Volmid.erase(Volmid.begin(), Volmid.begin() + sz);
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

  sz = Volume.size() - plotdate->size();
  if(sz > 0)
    {
      Volume.erase(Volume.end() - sz, Volume.end());
      Volmid.erase(Volmid.end() - sz, Volmid.end());
    }
}

int
PlotVolumeGlobal::Draw(mglGraph *gr)
{
  calcForDraw();
  std::vector<int> X;
  for(size_t i = 0; i < Volume.size(); i++)
    {
      X.push_back(i);
    }
  mglData x(X), y(Volume), y2(Volmid);
  double miny, maxy;
  if(y.Minimal() <= y2.Minimal())
    {
      miny = y.Minimal();
    }
  else
    {
      miny = y2.Minimal();
    }
  if(y.Maximal() >= y2.Maximal())
    {
      maxy = y.Maximal();
    }
  else
    {
      maxy = y2.Maximal();
    }
  mglPoint p1(x.Minimal(), maxy + ((maxy - miny) * 0.02));
  mglPoint p5(x.Maximal(), maxy + ((maxy - miny) * 0.02));

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
  gr->SetObjId(12);
  gr->SetSize(width, height);
  gr->SetPlotFactor(0.1);
  AuxFunc af;
  gr->Title(af.utf8to(gettext("Shares turnover")).c_str(), "", 5);
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
      strm << std::setprecision(0);
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
  gr->Label('x', af.utf8to(gettext("Days")).c_str(), 0);
  gr->Label('y', af.utf8to(gettext("Shares quantity")).c_str(), 0);

  gr->Plot(x, y, "g");
  gr->Plot(x, y2, "r");
  gr->AddLegend(af.utf8to(gettext("Turnover")).c_str(), "g");
  gr->AddLegend(af.utf8to(gettext("Average shares turnover")).c_str(), "r");
  gr->Legend(1.0, 1.3);
  datebeg = af.utf8to(datebeg);
  dateend = af.utf8to(dateend);
  gr->Puts(p1, datebeg.c_str(), "k", 3);
  gr->Puts(p5, dateend.c_str(), "k", 3);
  for(size_t i = 0; i < dates.size(); i++)
    {
      std::string tmp = dates[i];
      tmp = af.utf8to(tmp);
      gr->Puts(Coord[i], tmp.c_str(), "k", 3);
    }

  return 0;
}
