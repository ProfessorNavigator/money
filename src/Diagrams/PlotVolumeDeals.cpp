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

#include "PlotVolumeDeals.h"

PlotVolumeDeals::PlotVolumeDeals(
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

PlotVolumeDeals::~PlotVolumeDeals()
{
  // TODO Auto-generated destructor stub
}

void
PlotVolumeDeals::calcForDraw()
{
  plotdate->clear();
  mpf_set_default_prec(512);
  mpf_class quan(0), mon(0);
  std::vector<mpf_class> Quan;
  std::vector<mpf_class> Mon;
  std::fstream f;
  std::string line, midd, temp;
  int count = 0;
  f.open(filename, std::ios_base::in);
  if(!f.is_open())
    {
      std::cout << "File to plot daily volume not opened" << std::endl;
    }
  else
    {
      std::tuple<int, int, int> temptup;
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
		      std::get<0>(temptup) = countch;
		    }
		  if(temp == "VALUE")
		    {
		      std::get<1>(temptup) = countch;
		    }
		  if(temp == "PRICE")
		    {
		      std::get<2>(temptup) = countch;
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
	  if(count > 2 && !line.empty())
	    {
	      midd = line;
	      temp = line;
	      for(int i = 0; i < std::get<0>(temptup); i++)
		{
		  temp = midd.substr(0, midd.find(";"));
		  midd = midd.erase(0, temp.size() + std::string(";").size());
		}
	      midd = midd.substr(0, midd.find(";"));
	      std::tuple<std::string, double, double, double, double> ttup;
	      std::get<0>(ttup) = midd;

	      midd = line;
	      temp = line;
	      for(int i = 0; i < std::get<1>(temptup); i++)
		{
		  temp = midd.substr(0, midd.find(";"));
		  midd = midd.erase(0, temp.size() + std::string(";").size());
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
	      for(int i = 0; i < std::get<2>(temptup); i++)
		{
		  temp = midd.substr(0, midd.find(";"));
		  midd = midd.erase(0, temp.size() + std::string(";").size());
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
  mpf_class temp2;
  for(size_t i = 0; i < Quan.size(); i++)
    {
      temp2 = Quan[i];
      Volume.push_back(temp2.get_d());
      std::get<1>(plotdate->at(i)) = Volume[i];
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
}

void
PlotVolumeDeals::cleanVectors(int dateb, int datee)
{
  std::locale loc("C");
  plotdate->erase(
      std::remove_if(plotdate->begin(), plotdate->end(), [dateb, loc]
      (auto &el)
	{
	  std::string val = std::get<0>(el);
	  val = val.substr(0, val.find(":"));

	  std::stringstream strm;
	  strm.imbue(loc);
	  strm << val;
	  int day;
	  strm >> day;

	  val = std::get<0>(el);
	  val.erase(0, val.find(":") + std::string(":").size());
	  val = val.substr(0, val.find(":"));
	  strm.clear();
	  strm.str("");
	  strm.imbue(loc);
	  strm << val;
	  int month;
	  strm >> month;

	  val = std::get<0>(el);
	  val.erase(0, val.rfind(":") + std::string(":").size());
	  strm.clear();
	  strm.str("");
	  strm.imbue(loc);
	  strm << val;
	  int year;
	  strm >> year;

	  return day * 3600 + month * 60 + year < dateb;
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
	  val = val.substr(0, val.find(":"));

	  std::stringstream strm;
	  strm.imbue(loc);
	  strm << val;
	  int day;
	  strm >> day;

	  val = std::get<0>(el);
	  val.erase(0, val.find(":") + std::string(":").size());
	  val = val.substr(0, val.find(":"));
	  strm.clear();
	  strm.str("");
	  strm.imbue(loc);
	  strm << val;
	  int month;
	  strm >> month;

	  val = std::get<0>(el);
	  val.erase(0, val.rfind(":") + std::string(":").size());
	  strm.clear();
	  strm.str("");
	  strm.imbue(loc);
	  strm << val;
	  int year;
	  strm >> year;

	  return day * 3600 + month * 60 + year > datee;
	}),
      plotdate->end());

  sz = Volume.size() - plotdate->size();
  if(sz > 0)
    {
      Volume.erase(Volume.end() - sz, Volume.end());
      Volmid.erase(Volmid.end() - sz, Volmid.end());
    }
  if(plotdate->size() > 0)
    {
      datebeg = std::get<0>(plotdate->at(0));
      dateend = std::get<0>(plotdate->at(plotdate->size() - 1));
    }
}

int
PlotVolumeDeals::Draw(mglGraph *gr)
{
  std::vector<int> X;
  for(size_t i = 0; i < Volume.size(); i++)
    {
      X.push_back(i);
    }
  mglData x(X), y(Volume), y2(Volmid);

  int d = 6;
  int number = X.size();
  while(number % d > 0)
    {
      number = number + 1;
    }
  d = number / d;

  AuxFunc af;
  std::string grnm = gettext("Shares turnover");
  grnm = af.utf8to(grnm);

  //Общие настройки графика
  gr->SetObjId(12);
  gr->SetSize(width, height);
  gr->Title(grnm.c_str(), "", 5);
  gr->SetQuality(3);
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
  gr->SetRanges(x.Minimal(), x.Maximal(), miny, maxy);
  gr->SetFontSize(3);
  gr->SetOriginTick(false);

  std::vector<double> ticks;
  double tickstep = (maxy - miny) / 3;
  if(tickstep < 0)
    {
      tickstep = -tickstep;
    }
  double tickval = y.Minimal();
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
  gr->Axis("y", "k");
  gr->Axis("!fx", "k");
  gr->SetTickSkip(true);
  gr->Label('x', af.utf8to(gettext("Transactions")).c_str(), 0);
  gr->Label('y', af.utf8to(gettext("Shares quantity")).c_str(), 0);

  //Отображение графика
  gr->Grid("xy", "{xA0A136}");
  gr->Plot(x, y, "g");
  gr->Plot(x, y2, "r");
  gr->AddLegend(af.utf8to(gettext("Shares turnover")).c_str(), "g");
  gr->AddLegend(af.utf8to(gettext("Average shares turnover")).c_str(), "r");
  gr->SetFontSize(3);
  gr->Legend(1.1, 1.3);

  //Подписи оси х
  mglPoint p1(x.Minimal(), maxy + ((maxy - miny) * 0.02));
  mglPoint p5(x.Maximal(), maxy + ((maxy - miny) * 0.02));
  datebeg = af.utf8to(datebeg);
  dateend = af.utf8to(dateend);
  gr->Puts(p1, datebeg.c_str(), "k", 3);
  gr->Puts(p5, dateend.c_str(), "k", 3);

  return 0;
}
