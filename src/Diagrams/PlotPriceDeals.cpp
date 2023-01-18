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

#include "PlotPriceDeals.h"

PlotPriceDeals::PlotPriceDeals(
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

PlotPriceDeals::~PlotPriceDeals()
{
  // TODO Auto-generated destructor stub
}

void
PlotPriceDeals::calcForDraw()
{
  plotdate->clear();
  mpf_set_default_prec(512);
  mpf_class quan(0), mon(0), summa(0), summm(0);
  double pr;
  std::vector<mpf_class> Quan;
  std::vector<mpf_class> Mon;
  std::vector<mpf_class> Pricemid;
  std::fstream f;
  int count = 0;
  std::string line, midd, temp;

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

  if(!std::filesystem::exists(filename))
    {
      std::cout << "File to plot daily price not opened" << std::endl;
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
	      if(count == 3)
		{
		  yname = line;
		  yname.erase(0, yname.find(";") + std::string(";").size());
		  yname.erase(0, yname.find(";") + std::string(";").size());
		  yname = yname.substr(0, yname.find(";"));
		}

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
	      strm >> pr;
	      Price.push_back(pr);
	      std::get<1>(ttup) = pr;

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
  for(size_t i = 0; i < Quan.size(); i++)
    {
      summa = summa + Quan[i];
      summm = summm + Mon[i];
      Pricemid.push_back(summm / summa);
    }

  for(size_t i = 0; i < Pricemid.size(); i++)
    {
      Tc.push_back(Pricemid[i].get_d());
      std::get<2>(plotdate->at(i)) = Tc[i];
    }

  if(plotdate->size() > 0)
    {
      datebeg = std::get<0>(plotdate->at(0));
      dateend = std::get<0>(plotdate->at(plotdate->size() - 1));
    }
}

int
PlotPriceDeals::Draw(mglGraph *gr)
{
  std::vector<int> X;
  calcForDraw();
  for(size_t i = 0; i < Tc.size(); i++)
    {
      X.push_back(i);
    }
  mglData x(X), y(Tc), y1(Price);
  int d = 6;
  int number = X.size();
  while(number % d > 0)
    {
      number = number + 1;
    }
  d = number / d;

  //Координаты подписей оси х
  mglPoint p1(x.Minimal(),
	      y1.Maximal() + ((y1.Maximal() - y1.Minimal()) * 0.02));
  mglPoint p5(x.Maximal(),
	      y1.Maximal() + ((y1.Maximal() - y1.Minimal()) * 0.02));

  AuxFunc af;
  std::string grnm = gettext("Price");
  grnm = af.utf8to(grnm);

  //Общие настройки графика
  gr->SetObjId(14);
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
      strm << std::setprecision(4);
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
  gr->Axis("y", "k");
  gr->Axis("!f0x", "k");
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
  gr->Label('x', af.utf8to(gettext("Transactions")).c_str(), 0);
  gr->Label('y', af.utf8to(yname).c_str(), 0);

  //Отображение графика
  gr->Grid("xy", "{xA0A136}");
  gr->Plot(x, y, "r");
  gr->Plot(x, y1, "b");
  gr->AddLegend(af.utf8to(gettext("Price")).c_str(), "b");
  gr->AddLegend(af.utf8to(gettext("ECg")).c_str(), "r");
  gr->SetFontSize(3);
  gr->Legend(1.1, 1.3);

  //Подписи оси х
  datebeg = af.utf8to(datebeg);
  dateend = af.utf8to(dateend);
  gr->Puts(p1, datebeg.c_str(), "k", 3);
  gr->Puts(p5, dateend.c_str(), "k", 3);

  return 0;
}
