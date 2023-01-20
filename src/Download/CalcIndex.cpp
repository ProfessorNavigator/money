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

#include "CalcIndex.h"

CalcIndex::CalcIndex(int *canceled)
{
  Canceled = canceled;
}

CalcIndex::~CalcIndex()
{
  // TODO Auto-generated destructor stub
}

void
CalcIndex::mainCalc()
{
  mpf_set_default_prec(2048);
  AuxFunc af;
  std::vector<std::filesystem::path> maindir;
  std::vector<std::filesystem::path> filelist;
  mpf_class money(0), volume(0), summon(0), sumvol(0), psod(0), psd(0);
  std::string dirline;
  af.homePath(&dirline);
  dirline = dirline + "/.Money/BoardsList";
  std::filesystem::path dir = std::filesystem::u8path(dirline);
  std::fstream f;
  std::vector<std::string> excludevect;
  std::tuple<int, int, int> temptup;
  std::string temp;
  int linecount = 0;
  f.open(dir, std::ios_base::in);
  while(!f.eof())
    {
      std::string line;
      getline(f, line);
      if(linecount == 0 && !line.empty())
	{
	  std::string tmp = line;
	  int countch = 0;
	  while(tmp.size() > 0)
	    {
	      temp = tmp.substr(0, tmp.find(";"));
	      std::string::size_type n;
	      if(temp == "boardid")
		{
		  std::get<0>(temptup) = countch;
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
      if(!line.empty())
	{
	  std::string::size_type n;
	  n = line.find("USD");
	  if(n != std::string::npos)
	    {
	      std::string tmp = line;
	      for(int i = 0; i < std::get<0>(temptup); i++)
		{
		  temp = tmp.substr(0, tmp.find(";"));
		  tmp = tmp.erase(0, temp.size() + std::string(";").size());
		}
	      tmp = tmp.substr(0, tmp.find(";"));
	      excludevect.push_back(tmp);
	    }
	  n = line.find("EUR");
	  if(n != std::string::npos)
	    {
	      std::string tmp = line;
	      for(int i = 0; i < std::get<0>(temptup); i++)
		{
		  temp = tmp.substr(0, tmp.find(";"));
		  tmp = tmp.erase(0, temp.size() + std::string(";").size());
		}
	      tmp = tmp.substr(0, tmp.find(";"));
	      excludevect.push_back(tmp);
	    }
	  n = line.find("CNY");
	  if(n != std::string::npos)
	    {
	      std::string tmp = line;
	      for(int i = 0; i < std::get<0>(temptup); i++)
		{
		  temp = tmp.substr(0, tmp.find(";"));
		  tmp = tmp.erase(0, temp.size() + std::string(";").size());
		}
	      tmp = tmp.substr(0, tmp.find(";"));
	      excludevect.push_back(tmp);
	    }
	}
      linecount++;
    }
  f.close();
  excludevect.push_back("Deals");
  excludevect.push_back("PPm");
  af.homePath(&dirline);
  dirline = dirline + "/Money";
  dir = std::filesystem::u8path(dirline);
  if(operationName)
    {
      operationName(gettext("File list creation:"));
    }
  for(auto &p : std::filesystem::directory_iterator(dir))
    {
      if(*Canceled == 1)
	{
	  if(canceled)
	    {
	      canceled();
	    }
	  return void();
	}
      std::filesystem::path path = p.path();
      std::string line = path.filename().u8string();
      auto it = std::find(excludevect.begin(), excludevect.end(), line);
      if(it == excludevect.end())
	{
	  maindir.push_back(path);
	}
    }
  for(size_t i = 0; i < maindir.size(); i++)
    {
      if(*Canceled == 1)
	{
	  if(canceled)
	    {
	      canceled();
	    }
	  return void();
	}
      for(auto &p : std::filesystem::directory_iterator(maindir[i]))
	{
	  std::filesystem::path path = p.path();
	  filelist.push_back(path);
	}
      if(progr)
	{
	  progr(
	      static_cast<double>(i + 1) / static_cast<double>(maindir.size()));
	}
    }

  if(progr)
    {
      progr(0);
    }
  if(operationName)
    {
      operationName(gettext("Data collection:"));
    }
  std::vector<std::tuple<int, mpf_class, mpf_class>> data; //date, volume, money
  for(size_t i = 0; i < filelist.size(); i++)
    {
      if(*Canceled == 1)
	{
	  if(canceled)
	    {
	      canceled();
	    }
	  return void();
	}
      std::string line;
      f.open(filelist[i], std::ios_base::in);
      int linecount = 0;
      while(!f.eof())
	{
	  getline(f, line);
	  if(linecount == 1 && !line.empty())
	    {
	      std::string tmp = line;
	      int countch = 0;
	      while(tmp.size() > 0)
		{
		  temp = tmp.substr(0, tmp.find(";"));
		  std::string::size_type n;
		  if(temp == "Дата")
		    {
		      std::get<0>(temptup) = countch;
		    }
		  if(temp == "Объем, шт.")
		    {
		      std::get<1>(temptup) = countch;
		    }
		  if(temp == "Объем оборота денег")
		    {
		      std::get<2>(temptup) = countch;
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
	  if(linecount > 1 && !line.empty())
	    {
	      std::string datestr, volumestr, moneystr;
	      datestr = line;
	      for(int i = 0; i < std::get<0>(temptup); i++)
		{
		  temp = datestr.substr(0, datestr.find(";"));
		  datestr = datestr.erase(
		      0, temp.size() + std::string(";").size());
		}
	      datestr = datestr.substr(0, datestr.find(";"));

	      volumestr = line;
	      for(int i = 0; i < std::get<1>(temptup); i++)
		{
		  temp = volumestr.substr(0, volumestr.find(";"));
		  volumestr = volumestr.erase(
		      0, temp.size() + std::string(";").size());
		}
	      volumestr = volumestr.substr(0, volumestr.find(";"));

	      moneystr = line;
	      for(int i = 0; i < std::get<2>(temptup); i++)
		{
		  temp = moneystr.substr(0, moneystr.find(";"));
		  moneystr = moneystr.erase(
		      0, temp.size() + std::string(";").size());
		}
	      moneystr = moneystr.substr(0, moneystr.find(";"));

	      double date;
	      mpf_class volume, money;
	      int day, month, year;
	      std::string tmp;
	      tmp = datestr;
	      tmp = tmp.substr(0, tmp.find("."));
	      day = std::stoi(tmp);
	      tmp = datestr;
	      tmp.erase(0, tmp.find(".") + std::string(".").size());
	      tmp = tmp.substr(0, tmp.find("."));
	      month = std::stoi(tmp);
	      tmp = datestr;
	      tmp.erase(0, tmp.find(".") + std::string(".").size());
	      tmp.erase(0, tmp.find(".") + std::string(".").size());
	      year = std::stoi(tmp);
	      date = af.grigtojulian(day, month, year, 0, 0, 0.0);
	      std::locale loc("C");
	      std::stringstream strm;
	      strm.imbue(loc);
	      strm << volumestr;
	      strm >> volume;
	      strm.str("");
	      strm.clear();
	      strm.imbue(loc);
	      strm << moneystr;
	      strm >> money;
	      if(date < af.grigtojulian(1, 1, 1998, 0, 0, 0.0))
		{
		  money = money * 0.001;
		}
	      auto it = std::find_if(data.begin(), data.end(), [&date]
	      (std::tuple<int, mpf_class, mpf_class> &elemnt)
		{
		  return date == std::get<0>(elemnt);
		});
	      if(it != data.end())
		{
		  int tmpdate = std::get<0>(*it);
		  mpf_class tmpvolume(std::get<1>(*it));
		  mpf_class tmpmoney(std::get<2>(*it));
		  tmpvolume = tmpvolume + volume;
		  tmpmoney = tmpmoney + money;
		  *it = std::make_tuple(tmpdate, tmpvolume, tmpmoney);
		}
	      else
		{
		  data.push_back(std::make_tuple(date, volume, money));
		}
	    }
	  linecount++;
	}
      f.close();
      if(progr)
	{
	  progr(
	      static_cast<double>(i + 1)
		  / static_cast<double>(filelist.size()));
	}
    }
  maindir.clear();
  filelist.clear();
  std::sort(
      data.begin(),
      data.end(),
      []
      (std::tuple<int, mpf_class, mpf_class> &first,
       std::tuple<int, mpf_class, mpf_class> &second)
	 {
	   return std::get<0>(first) < std::get<0>(second);
	 });
  af.homePath(&dirline);
  dirline = dirline + "/Money/PPm";
  dir = std::filesystem::u8path(dirline);
  if(!std::filesystem::exists(dir))
    {
      std::filesystem::create_directories(dir);
    }

  std::vector<std::pair<int, int>> infdates;

  for(int i = af.addYears(std::get<0>(data[0]), 1);
      i < std::get<0>(data[data.size() - 1]); i = af.addYears(i, 1))
    {
      if(*Canceled == 1)
	{
	  if(canceled)
	    {
	      canceled();
	    }
	  return void();
	}
      int t = i;
      int tt = i;
      auto it = std::find_if(data.begin(), data.end(), [&t]
      (auto &element)
	{
	  return t == std::get<0>(element);
	});
      if(it != data.end())
	{
	  infdates.push_back(std::make_pair(t, t));
	}
      else
	{
	  for(;;)
	    {
	      tt = tt - 1;
	      auto iter = std::find_if(data.begin(), data.end(), [&tt]
	      (auto &element)
		{
		  return tt == std::get<0>(element);
		});
	      if(iter != data.end())
		{
		  break;
		}
	    }
	  infdates.push_back(std::make_pair(t, tt));
	}
    }
  auto it = std::find_if(infdates.begin(), infdates.end(), [&data]
  (auto &element)
    {
      return std::get<0>(data[data.size() - 1]) == element.second;
    });
  if(it == infdates.end())
    {
      infdates.push_back(
	  std::make_pair(std::get<0>(data[data.size() - 1]),
			 std::get<0>(data[data.size() - 1])));
    }
  std::vector<std::string> inflation;
  std::string lineinfl = "Дата;ИПС\n";
  inflation.push_back(lineinfl);
  af.homePath(&dirline);
  dirline = dirline + "/Money/PPm/PPm.csv";
  dir = std::filesystem::u8path(dirline);
  f.open(dir, std::ios_base::out | std::ios_base::binary);
  std::string line = "Дата;Объём акций;Объем денег;ПСОд;ПСд\n";
  f.write(line.c_str(), line.size());
  mpf_class Volume(0), Money(0), previospsd(0);
  if(operationName)
    {
      operationName(gettext("Result saving:"));
    }
  if(progr)
    {
      progr(0);
    }
  for(size_t i = 0; i < data.size(); i++)
    {
      if(*Canceled == 1)
	{
	  if(canceled)
	    {
	      canceled();
	    }
	  return void();
	}
      std::string dateline = af.togrigday(std::get<0>(data[i])) + "."
	  + af.togrigmonth(std::get<0>(data[i])) + "."
	  + af.togrigyear(std::get<0>(data[i]));
      std::string volumestr;
      std::stringstream strm;
      std::locale loc("C");
      strm.imbue(loc);
      strm << std::fixed;
      strm << std::setprecision(0);
      strm << std::get<1>(data[i]);
      strm >> volumestr;

      std::string moneystr;
      strm.str("");
      strm.clear();
      strm.imbue(loc);
      strm << std::fixed;
      strm << std::setprecision(3);
      strm << std::get<2>(data[i]);
      strm >> moneystr;

      Volume = Volume + std::get<1>(data[i]);
      Money = Money + std::get<2>(data[i]);

      psod = std::get<1>(data[i]) / std::get<2>(data[i]);
      psd = Volume / Money;

      std::string psodstr;
      strm.str("");
      strm.clear();
      strm.imbue(loc);
      strm << std::fixed;
      strm << std::setprecision(22);
      strm << psod;
      strm >> psodstr;

      std::string psdstr;
      strm.str("");
      strm.clear();
      strm.imbue(loc);
      strm << std::fixed;
      strm << std::setprecision(22);
      strm << psd;
      strm >> psdstr;
      line = dateline + ";" + volumestr + ";" + moneystr + ";" + psodstr + ";"
	  + psdstr + "\n";
      f.write(line.c_str(), line.size());
      int tmpdate = std::get<0>(data[i]);
      auto iter = std::find_if(infdates.begin(), infdates.end(), [&tmpdate]
      (auto &element)
	{
	  return tmpdate == element.second;
	});
      if(iter != infdates.end())
	{
	  int index = std::distance(infdates.begin(), iter);
	  if(index == 0)
	    {
	      mpf_class ips;
	      ips = (psd - std::get<1>(data[0]) / std::get<2>(data[0])) * 100
		  / (std::get<1>(data[0]) / std::get<2>(data[0]));
	      previospsd = psd;
	      strm.str("");
	      strm.clear();
	      strm.imbue(loc);
	      strm << std::fixed;
	      strm << std::setprecision(2);
	      strm << ips;
	      strm >> lineinfl;
	      lineinfl = af.togrigday(infdates[index].first) + "."
		  + af.togrigmonth(infdates[index].first) + "."
		  + af.togrigyear(infdates[index].first) + ";" + lineinfl
		  + "\n";
	      inflation.push_back(lineinfl);
	    }
	  else
	    {
	      mpf_class ips;
	      ips = (psd - previospsd) * 100 / previospsd;
	      previospsd = psd;
	      strm.str("");
	      strm.clear();
	      strm.imbue(loc);
	      strm << std::fixed;
	      strm << std::setprecision(2);
	      strm << ips;
	      strm >> lineinfl;
	      lineinfl = af.togrigday(infdates[index].first) + "."
		  + af.togrigmonth(infdates[index].first) + "."
		  + af.togrigyear(infdates[index].first) + ";" + lineinfl
		  + "\n";
	      inflation.push_back(lineinfl);
	    }
	}
      if(progr)
	{
	  progr(static_cast<double>(i + 1) / static_cast<double>(data.size()));
	}
    }
  f.close();
  af.homePath(&dirline);
  dirline = dirline + "/Money/PPm/Inflation.csv";
  dir = std::filesystem::u8path(dirline);
  f.open(dir, std::ios_base::out | std::ios_base::binary);
  for(size_t i = 0; i < inflation.size(); i++)
    {
      std::string writeline = inflation[i];
      f.write(writeline.c_str(), writeline.size());
    }
  f.close();
  if(allComplet)
    {
      allComplet();
    }
}
