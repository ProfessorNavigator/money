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

#include <DownloadTechnical.h>

DownloadTechnical::DownloadTechnical(int *cancel)
{
  this->cancel = cancel;
}

DownloadTechnical::~DownloadTechnical()
{
  // TODO Auto-generated destructor stub
}

void
DownloadTechnical::downloadTechnical()
{
  int result = 0;
  if(*cancel == 0)
    {
      result = downloadBoards();
    }
  if(*cancel == 0 && result == 0)
    {
      result = downloadListF();
    }
  if(*cancel == 0 && result == 0)
    {
      result = downloadList();
    }
  if(*cancel == 0 && result == 0)
    {
      if(techdwnld)
	{
	  techdwnld(1);
	}
    }
  else
    {
      if(*cancel != 0 && result == 0)
	{
	  if(techdwnld)
	    {
	      techdwnld(0);
	    }
	}
      else
	{
	  if(techdwnld)
	    {
	      techdwnld(2);
	    }
	}
    }
}

int
DownloadTechnical::availibaleDates()
{
  int check = 0;
  int errorcount = 0;
  AuxFunc af;
  std::string tmp = af.tempPath();
  tmp = tmp + "/Money/Dates";
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
DownloadTechnical::availibaleDatesF()
{
  int check = 0;
  int errorcount = 0;
  AuxFunc af;
  std::string tmp = af.tempPath();
  tmp = tmp + "/Money/Dates";
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

int
DownloadTechnical::downloadBoards()
{
  int result = 0;
  std::string url =
      "https://iss.moex.com/iss/engines/stock/markets/shares/boards.csv";
  std::string filename;
  AuxFunc af;
  filename = af.tempPath();
  filename = filename + "/Money/BoardsListTmp";
  std::filesystem::path boardsp = std::filesystem::u8path(filename);
  if(!std::filesystem::exists(boardsp.parent_path()))
    {
      std::filesystem::create_directories(boardsp.parent_path());
    }
  else
    {
      if(std::filesystem::exists(boardsp))
	{
	  std::filesystem::remove_all(boardsp);
	}
    }
  int check = 0;
  std::string line;
  int count = 0;
  for(;;)
    {
      if(*cancel > 0)
	{
	  std::filesystem::remove_all(boardsp);
	  return result;
	}
      af.callNetwork(url, filename, &check);
      count++;
      if(count == 20)
	{
	  result = -1;
	  std::filesystem::remove_all(boardsp);
	  return result;
	}
      if(check > 0)
	{
	  break;
	}
      sleep(1);
    }

  af.homePath(&filename);
  filename = filename + "/.Money/BoardsList";
  std::filesystem::path boardsout = std::filesystem::u8path(filename);
  if(std::filesystem::exists(boardsout))
    {
      std::filesystem::remove_all(boardsout);
    }
  if(!std::filesystem::exists(boardsout.parent_path()))
    {
      std::filesystem::create_directories(boardsout.parent_path());
    }
  std::filesystem::copy(boardsp, boardsout);
  std::filesystem::remove_all(boardsp.parent_path());
  boardsp = boardsout;

  check = 0;
  url =
      "https://iss.moex.com/iss/engines/stock/markets/foreignshares/boards.csv";
  filename = af.tempPath();
  filename = filename + "/Money/BoardsListF";
  std::filesystem::path boardsfp = std::filesystem::u8path(filename);
  if(!std::filesystem::exists(boardsfp.parent_path()))
    {
      std::filesystem::create_directories(boardsfp.parent_path());
    }
  else
    {
      if(std::filesystem::exists(boardsfp))
	{
	  std::filesystem::remove_all(boardsfp);
	}
    }
  count = 0;
  for(;;)
    {
      if(*cancel > 0)
	{

	  return result;
	}
      af.callNetwork(url, filename, &check);
      count++;
      if(count == 20)
	{
	  result = -1;
	  return result;
	}
      if(check > 0)
	{
	  break;
	}
      sleep(1);
    }
  std::vector<std::string> boardsv;
  std::fstream f;
  f.open(boardsp, std::ios_base::in);
  if(f.is_open())
    {
      count = 0;
      while(!f.eof())
	{
	  std::string line;
	  getline(f, line);
	  if(count > 1 && !line.empty())
	    {
	      af.cp1251toutf8(line);
	      boardsv.push_back(line);
	    }
	  count++;
	}
      f.close();
    }
  std::filesystem::remove_all(boardsp);

  f.open(boardsp, std::ios_base::out | std::ios_base::binary);
  if(f.is_open())
    {
      for(size_t i = 0; i < boardsv.size(); i++)
	{
	  std::string line = boardsv[i] + "\n";
	  f.write(line.c_str(), line.size());
	}
      f.close();
    }

  std::vector<std::string> boardsfv;
  f.open(boardsfp, std::ios_base::in);
  if(f.is_open())
    {
      count = 0;
      while(!f.eof())
	{
	  std::string line;
	  getline(f, line);
	  if(count > 2 && !line.empty())
	    {
	      af.cp1251toutf8(line);
	      boardsfv.push_back(line);
	    }
	  count++;
	}
      f.close();
    }
  std::filesystem::remove_all(boardsfp.parent_path());
  while(boardsfv.size() > 0)
    {
      std::string line = *(boardsfv.end() - 1);
      if(line.empty())
	{
	  boardsfv.pop_back();
	}
      else
	{
	  break;
	}
    }
  size_t cb = 0;
  if(std::filesystem::exists(boardsp) && boardsfv.size() > 3)
    {
      cb = 3;
    }
  f.open(boardsp,
	 std::ios_base::out | std::ios_base::app | std::ios_base::binary);
  if(f.is_open())
    {
      for(size_t i = cb; i < boardsfv.size(); i++)
	{
	  std::string line = boardsfv[i] + "\n";
	  f.write(line.c_str(), line.size());
	}
      f.close();
    }
  return result;
}

int
DownloadTechnical::downloadListF()
{
  int result = 0;
  std::string line, url;
  AuxFunc af;
  std::string filename;
  filename = af.tempPath();
  filename = filename + "/Money/ListFtemp";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  if(std::filesystem::exists(filepath))
    {
      std::filesystem::remove_all(filepath);
    }
  std::filesystem::create_directories(filepath);
  int check = 0;
  int count = 0;
  int linecount = 0;
  int errorcount = 0;
  for(;;)
    {
      if(*cancel > 0)
	{
	  std::filesystem::remove_all(filepath);
	  return result;
	}

      filename = filepath.u8string() + "/List" + std::to_string(count);
      std::filesystem::path p = std::filesystem::u8path(filename);
      url = "https://iss.moex.com/iss/history/engines/"
	  "stock/markets/foreignshares/listing.csv?start="
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
	      result = -1;
	      return result;
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
	      result = -1;
	      return result;
	    }
	}
      count = count + 100;
    }
  std::vector<std::filesystem::path> pv;
  if(std::filesystem::exists(filepath))
    {
      for(auto &dirit : std::filesystem::directory_iterator(filepath))
	{
	  std::filesystem::path p = dirit.path();
	  if(!std::filesystem::is_directory(p))
	    {
	      pv.push_back(p);
	    }
	}
    }
  std::sort(pv.begin(), pv.end(), []
  (auto &el1, auto &el2)
    {
      return el1.stem().u8string() < el2.stem().u8string();
    });
  af.homePath(&filename);
  filename = filename + "/.Money/ListF";
  std::filesystem::path flp = std::filesystem::u8path(filename);
  if(std::filesystem::exists(flp))
    {
      std::filesystem::remove_all(flp);
    }

  for(size_t i = 0; i < pv.size(); i++)
    {
      std::fstream f;
      std::fstream outf;
      f.open(pv[i], std::ios_base::in);
      outf.open(
	  flp, std::ios_base::out | std::ios_base::app | std::ios_base::binary);
      if(f.is_open() && outf.is_open())
	{
	  count = 0;
	  while(!f.eof())
	    {
	      line.clear();
	      getline(f, line);
	      if(i > 0)
		{
		  if(count > 2 && !line.empty())
		    {
		      af.cp1251toutf8(line);
		      line = line + "\n";
		      outf.write(line.c_str(), line.size());
		    }
		}
	      else
		{
		  if(count > 1 && !line.empty())
		    {
		      af.cp1251toutf8(line);
		      line = line + "\n";
		      outf.write(line.c_str(), line.size());
		    }
		}
	      count++;
	    }
	}
      if(f.is_open())
	{
	  f.close();
	}
      if(outf.is_open())
	{
	  outf.close();
	}
    }
  std::filesystem::remove_all(filepath);
  return result;
}

int
DownloadTechnical::downloadList()
{
  int result = 0;
  std::string line, url;
  AuxFunc af;
  std::string filename;
  filename = af.tempPath();
  filename = filename + "/Money/Listtemp";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  if(std::filesystem::exists(filepath))
    {
      std::filesystem::remove_all(filepath);
    }
  std::filesystem::create_directories(filepath);
  int check = 0;
  int count = 0;
  int linecount = 0;
  int errorcount = 0;
  for(;;)
    {
      if(*cancel > 0)
	{
	  std::filesystem::remove_all(filepath);
	  return result;
	}

      filename = filepath.u8string() + "/List" + std::to_string(count);
      std::filesystem::path p = std::filesystem::u8path(filename);
      url = "https://iss.moex.com/iss/history/engines/"
	  "stock/markets/shares/listing.csv?start=" + std::to_string(count);
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
	      result = -1;
	      return result;
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
	  if(availibaleDates() == 0)
	    {
	      std::filesystem::remove(p);
	      break;
	    }
	  else
	    {
	      result = -1;
	      return result;
	    }
	}
      count = count + 100;
    }
  std::vector<std::filesystem::path> pv;
  if(std::filesystem::exists(filepath))
    {
      for(auto &dirit : std::filesystem::directory_iterator(filepath))
	{
	  std::filesystem::path p = dirit.path();
	  if(!std::filesystem::is_directory(p))
	    {
	      pv.push_back(p);
	    }
	}
    }
  std::sort(pv.begin(), pv.end(), []
  (auto &el1, auto &el2)
    {
      return el1.stem().u8string() < el2.stem().u8string();
    });
  af.homePath(&filename);
  filename = filename + "/.Money/List";
  std::filesystem::path flp = std::filesystem::u8path(filename);
  if(std::filesystem::exists(flp))
    {
      std::filesystem::remove_all(flp);
    }

  for(size_t i = 0; i < pv.size(); i++)
    {
      std::fstream f;
      std::fstream outf;
      f.open(pv[i], std::ios_base::in);
      outf.open(
	  flp, std::ios_base::out | std::ios_base::app | std::ios_base::binary);
      if(f.is_open() && outf.is_open())
	{
	  count = 0;
	  while(!f.eof())
	    {
	      line.clear();
	      getline(f, line);
	      if(i > 0)
		{
		  if(count > 2 && !line.empty())
		    {
		      af.cp1251toutf8(line);
		      line = line + "\n";
		      outf.write(line.c_str(), line.size());
		    }
		}
	      else
		{
		  if(count > 1 && !line.empty())
		    {
		      af.cp1251toutf8(line);
		      line = line + "\n";
		      outf.write(line.c_str(), line.size());
		    }
		}
	      count++;
	    }
	}
      if(f.is_open())
	{
	  f.close();
	}
      if(outf.is_open())
	{
	  outf.close();
	}
    }
  std::filesystem::remove_all(filepath);
  return result;
}
