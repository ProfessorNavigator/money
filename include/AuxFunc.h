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

#ifndef INCLUDE_AUXFUNC_H_
#define INCLUDE_AUXFUNC_H_
#include <string>
#include <sstream>
#include <curl/curl.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <algorithm>
#include <cctype>
#include <zip.h>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <tuple>
#include <unistd.h>
#include <cstring>
#include <cmath>

#ifdef __WIN32
  #include <Windows.h>
#endif

extern std::string MONEY_CA_CERT;

class AuxFunc
{
public:
  AuxFunc();
  virtual
  ~AuxFunc();
  double
  grigtojulian(int day, int month, int year, int hour, int minut, double sec);
  void
  dateJulian(double JDN, int *day, int *month, int *year, int *hour, int *minut,
	     double *second);
  std::string
  togrigday(double JDN);
  std::string
  togrigmonth(double JDN);
  std::string
  togrigyear(double JDN);
  void
  callNetwork(std::string urli, std::string filename, int *check);
  void
  cp1251toutf8(std::string &line);
  void
  stringToLower(std::string &line);
  void
  homePath(std::string *filename);
  std::string
  tempPath();
  int
  addYears(double JulianDay, int numyears);
  void
  packing(std::string source, std::string out);
  void
  unpacking(std::string archadress, std::string outfolder);
  int
  fileNames(std::string adress,
	    std::vector<std::tuple<int, int, std::string>> &filenames);
  void
  unpackByIndex(std::string archadress, std::string outfolder, int index);
  std::string
  get_selfpath();
  std::string
  utf8to(std::string line);
  void
  toutf8(std::string &line);
private:
  int
  fileNames(std::string adress, std::vector<std::string> &filenames);
  static size_t
  curl_write_func(char *ptr, size_t size, size_t nmemb, std::fstream *f);
};

#endif /* INCLUDE_AUXFUNC_H_ */
