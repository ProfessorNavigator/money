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

#include "AuxFunc.h"

AuxFunc::AuxFunc()
{
  // TODO Auto-generated constructor stub
}

AuxFunc::~AuxFunc()
{
  // TODO Auto-generated destructor stub
}

double
AuxFunc::grigtojulian(int day, int month, int year, int hour, int minut,
                      double sec)
{
  double a1, m1, c1, a2;
  a1 = std::floor(static_cast<double>(month - 3) / 12.0);
  m1 = static_cast<double>(month) - 3.0 - 12.0 * a1;
  c1 = std::floor(static_cast<double>(year + a1) / 100.0);
  a2 = static_cast<double>(year) + a1 - 100.0 * c1;
  int J = static_cast<int>(std::floor(146097.0 * c1 / 4.0)) + static_cast<int>
          (std::floor(
             36525.0 * a2 / 100.0)) + static_cast<int>(std::floor((
                   153.0 * m1 + 2.0) / 5.0)) + day + 1721119;
  double JD;
  if(J >= 0)
    {

      JD = static_cast<double>(J) + static_cast<double>(hour) / 24.0 +
           static_cast<double>(minut) / 1440.0 + sec / 86400.0;
    }
  else
    {
      double k = static_cast<double>(hour) / 24.0 + static_cast<double>
                 (minut) / 1440.0 + sec / 86400.0;
      k = 1 - k;
      JD = static_cast<double>(J) - k;
    }
  return JD;
}

std::string
AuxFunc::togrigday(double JDN)
{
  int day, month, year, hour, minut;
  double seconds;
  dateJulian(JDN, &day, &month, &year, &hour, &minut, &seconds);

  std::string result;
  if(day < 10)
    {
      result = "0" + std::to_string(day);
    }
  else
    {
      result = std::to_string(day);
    }

  return result;
}

std::string
AuxFunc::togrigmonth(double JDN)
{
  int day, month, year, hour, minut;
  double seconds;
  dateJulian(JDN, &day, &month, &year, &hour, &minut, &seconds);

  std::string result;
  if(month < 10)
    {
      result = "0" + std::to_string(month);
    }
  else
    {
      result = std::to_string(month);
    }

  return result;
}

std::string
AuxFunc::togrigyear(double JDN)
{
  int day, month, year, hour, minut;
  double seconds;
  dateJulian(JDN, &day, &month, &year, &hour, &minut, &seconds);

  std::string result;
  if(year < 10)
    {
      result = "0" + std::to_string(year);
    }
  else
    {
      result = std::to_string(year);
    }

  return result;
}

void
AuxFunc::dateJulian(double JDN, int *day, int *month, int *year, int *hour,
                    int *minut, double *second)
{
  int J = static_cast<int>(JDN);
  int c1 = static_cast<int>(std::floor((4.0 * static_cast<double>
                                        (J) - 6884477.0) / 146097.0));
  int e1 = 4 * J - 6884477 - 146097 * c1;

  int a1 = (100 * static_cast<int>(std::floor(static_cast<double>
                                   (e1) / 4.0)) + 99) / 36525;
  int e2 = 100 * static_cast<int>(std::floor(static_cast<double>
                                  (e1) / 4.0)) + 99 - 36525 * a1;

  int m1 = (5 * static_cast<int>(std::floor(static_cast<double>
                                 (e2) / 100.0)) + 2) / 153;
  int e3 = 5 * static_cast<int>(std::floor(static_cast<double>
                                (e2) / 100.0)) + 2 - 153 * m1;
  int a2 = static_cast<int>(std::floor(static_cast<double>(m1 + 2) / 12.0));
  int m2 = m1 + 2 - 12 * a2;
  *year = 100 * c1 + a1 + a2;
  *month = m2 + 1;
  *day = static_cast<int>(std::floor(static_cast<double>(e3) / 5.0)) + 1;
  double F, p;
  F = std::modf(JDN, &p);
  if(F < 0.0)
    {
      F = 1 + F;
    }
  *hour = static_cast<int>(F * 24.0);
  *minut = static_cast<int>((F * 24.0 - static_cast<double>(*hour)) * 60.0);
  *second = ((F * 24.0 - static_cast<double>(*hour)) * 60.0 - static_cast<double>
             (*minut)) * 60.0;
}

void
AuxFunc::callNetwork(std::string urli, std::string filename, int *check)
{
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  #ifdef _WIN32
  std::filesystem::path certp = std::filesystem::u8path(get_selfpath());
  certp = certp.parent_path();
  std::string certstr = certp.u8string();
  certstr = certstr + "/ca-bundle.trust.crt";
  #endif

  if(curl)
    {
      std::filesystem::path p = std::filesystem::u8path(filename);
      std::fstream f;
      f.open(p, std::ios_base::out | std::ios_base::binary);
      curl_easy_setopt(curl, CURLOPT_URL, urli.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &f);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func);
      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
      #ifdef _WIN32
      certp = std::filesystem::u8path(certstr);
      if(std::filesystem::exists(certp))
        {
          curl_easy_setopt(curl, CURLOPT_CAINFO, certstr.c_str());
        }
      #endif
      res = curl_easy_perform(curl);
      if(res != CURLE_OK)
        {
          *check = 0;
          std::cerr << "curl_easy_perform() failed: "
                    << curl_easy_strerror(res) << std::endl;
        }
      else
        {
          *check = 1;
        }
      curl_easy_cleanup(curl);
      f.close();
    }
}

size_t
AuxFunc::curl_write_func(char *ptr, size_t size, size_t nmemb, std::fstream *f)
{
  f->write(ptr, size * nmemb);
  return size * nmemb;
}

void
AuxFunc::cp1251toutf8(std::string &line)
{
  const std::string::size_type srclen = line.size();
  std::vector<UChar> target(srclen);
  UErrorCode status = U_ZERO_ERROR;
  UConverter *conv = ucnv_open("cp1251", &status);
  int32_t len = ucnv_toUChars(conv, target.data(), srclen, line.c_str(),
                              srclen, &status);
  ucnv_close(conv);
  if(!U_SUCCESS(status))
    {
      std::cout << u_errorName(status) << std::endl;
    }
  icu::UnicodeString ustr(target.data(), len);
  line.clear();
  ustr.toUTF8String(line);
}

void
AuxFunc::stringToLower(std::string &line)
{
  std::string innerline = line;
  icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(innerline.c_str());
  ustr.toLower();
  line.clear();
  ustr.toUTF8String(line);
}

void
AuxFunc::homePath(std::string *filename)
{
  char *fnm = getenv("USERPROFILE");
  if(fnm)
    {
      *filename = std::string(getenv("USERPROFILE"));
    }
  else
    {
      fnm = getenv("HOMEDRIVE");
      if(fnm)
        {
          *filename = std::string(getenv("HOMEDRIVE"));
        }
      else
        {
          fnm = getenv("HOMEPATH");
          if(fnm)
            {
              *filename = std::string(getenv("HOMEPATH"));
            }
          else
            {
              fnm = getenv("HOME");
              if(fnm)
                {
                  *filename = std::string(getenv("HOME"));
                }
              else
                {
                  fnm = getenv("SystemDrive");
                  if(fnm)
                    {
                      *filename = std::string(getenv("SystemDrive"));
                    }
                  else
                    {
                      std::cerr << "Cannot find user home folder" << std::endl;
                      exit(1);
                    }
                }
            }
        }
    }
  toutf8(*filename);
}

int
AuxFunc::addYears(double JulianDay, int numyears)
{
  int day, month, year, hours, minuts;
  double seconds;
  dateJulian(JulianDay, &day, &month, &year, &hours, &minuts, &seconds);
  year = year + numyears;
  double JDN = grigtojulian(day, month, year, hours, minuts, seconds);

  return JDN;
}

void
AuxFunc::packing(std::string source, std::string out)
{
  int er = 0;
  std::filesystem::path dir;
  dir = std::filesystem::u8path(source);
  source = dir.generic_u8string();
  if(std::filesystem::exists(dir))
    {
      if(std::filesystem::is_directory(dir))
        {
          zip_t *z;
          zip_error_t err;
          z = zip_open(out.c_str(), ZIP_TRUNCATE | ZIP_CREATE, &er);
          if(er < 1)
            {
              std::vector<std::filesystem::path> listf;
              std::vector<std::filesystem::path> listd;
              std::string line;
              if(!std::filesystem::is_empty(dir))
                {
                  for(auto &iter : std::filesystem::recursive_directory_iterator(
                        dir))
                    {
                      std::filesystem::path path = iter.path();
                      path = std::filesystem::u8path(path.generic_u8string());
                      if(std::filesystem::is_directory(path))
                        {
                          listd.push_back(path);
                        }
                      else
                        {
                          listf.push_back(path);
                        }
                    }
                  std::sort(listd.begin(), listd.end(), []
                            (auto & el1, auto & el2)
                  {
                    return el1.string().size() < el2.string().size();
                  });

                  std::string pardir = dir.filename().u8string();
                  zip_dir_add(z, pardir.c_str(), ZIP_FL_ENC_UTF_8);

                  for(size_t i = 0; i < listd.size(); i++)
                    {
                      line = listd[i].u8string();
                      std::string::size_type n;
                      n = line.find(source, 0);
                      line.erase(n, source.size());
                      line = pardir + line;
                      if(!std::filesystem::is_empty(listd[i]))
                        {
                          zip_dir_add(z, line.c_str(), ZIP_FL_ENC_UTF_8);
                        }
                    }
                  for(size_t i = 0; i < listf.size(); i++)
                    {
                      line = listf[i].u8string();
                      std::string::size_type n;
                      n = line.find(source, 0);
                      line.erase(n, source.size());
                      zip_source_t *zsource;
                      zsource = zip_source_file_create(
                                  listf[i].u8string().c_str(), 0, 0, &err);
                      line = pardir + line;
                      zip_file_add(z, line.c_str(), zsource,
                                   ZIP_FL_ENC_UTF_8);
                    }
                }

              zip_close(z);
            }
          else
            {
              std::cerr << "Error on packaing: " << strerror(er) << std::endl;
            }
        }
      else
        {
          zip_t *z;
          zip_error_t err;
          std::string line = dir.filename().u8string();
          z = zip_open(out.c_str(), ZIP_TRUNCATE | ZIP_CREATE, &er);
          if(er >= 1)
            {
              std::cerr << "Packing (file) error: " << strerror(er)
                        << std::endl;
            }
          else
            {
              zip_source_t *zsource;
              zsource = zip_source_file_create(source.c_str(), 0, 0, &err);
              if(zsource == nullptr)
                {
                  std::cerr << "Error on open file while packing" << std::endl;
                }
              else
                {
                  zip_file_add(z, line.c_str(), zsource, ZIP_FL_ENC_UTF_8);
                }
              zip_close(z);
            }
        }
    }
}

int
AuxFunc::fileNames(std::string adress, std::vector<std::string> &filenames)
{
  zip_t *z;
  std::string flname;
  int er = 0;
  int num;
  z = zip_open(adress.c_str(), ZIP_RDONLY, &er);
  if(er < 1)
    {
      num = zip_get_num_files(z);

      for(int i = 0; i < num; i++)
        {
          flname = zip_get_name(z, i, ZIP_FL_ENC_UTF_8);
          filenames.push_back(flname);
        }
      zip_close(z);
    }
  else
    {
      std::cerr << "Error on getting file names from archive: " << strerror(er)
                << std::endl;
    }

  return er;
}

int
AuxFunc::fileNames(std::string adress,
                   std::vector<std::tuple<int, int, std::string>> &filenames)
{
  zip_t *z;

  std::string flname;
  int er = 0;
  int num;
  z = zip_open(adress.c_str(), ZIP_RDONLY, &er);
  if(er < 1)
    {
      num = zip_get_num_files(z);

      for(int i = 0; i < num; i++)
        {
          flname = zip_get_name(z, i, ZIP_FL_ENC_UTF_8);
          struct zip_stat st;
          zip_stat_index(z, i, ZIP_FL_ENC_GUESS, &st);
          int sz = st.size;
          std::tuple<int, int, std::string> tuple;
          tuple = std::make_tuple(i, sz, flname);
          filenames.push_back(tuple);
        }
      zip_close(z);
    }
  else
    {
      std::cerr << "Error on getting file names from archive: " << strerror(er)
                << std::endl;
    }

  return er;
}

void
AuxFunc::unpacking(std::string archadress, std::string outfolder)
{
  std::vector<std::string> filenames;
  std::string line, archnm;
  fileNames(archadress, filenames);
  for(size_t i = 0; i < filenames.size(); i++)
    {
      line = filenames[i];
      std::filesystem::path path;
      if(line.substr(line.size() - 1, line.size() - 1) == "/")
        {
          line = outfolder + "/" + line;
          path = std::filesystem::u8path(line);
          std::filesystem::create_directories(path);
        }
    }
  zip_t *z;
  zip_file_t *file;
  zip_stat_t st;
  int er = 0;
  z = zip_open(archadress.c_str(), ZIP_RDONLY, &er);
  if(er < 1)
    {
      for(size_t i = 0; i < filenames.size(); i++)
        {
          line = filenames[i];
          if(line.substr(line.size() - 1, line.size() - 1) != "/")
            {
              file = zip_fopen(z, line.c_str(), ZIP_FL_ENC_UTF_8);
              zip_stat(z, line.c_str(), ZIP_STAT_NAME | ZIP_FL_ENC_UTF_8,
                       &st);
              char content;
              std::filesystem::path path;
              line = outfolder + "/" + line;
              path = std::filesystem::u8path(line);
              std::fstream f;
              f.open(path, std::ios_base::out | std::ios_base::binary);
              for(;;)
                {
                  zip_uint64_t ch = zip_fread(file, &content, sizeof(content));
                  if(ch <= 0)
                    {
                      if(ch < 0)
                        {
                          std::cerr << "unpacking file reading error"
                                    << std::endl;
                        }
                      break;
                    }
                  f.write(&content, sizeof(content));
                }
              f.close();
              zip_fclose(file);
            }
        }

      zip_close(z);
    }
  else
    {
      std::cout << strerror(er) << std::endl;
    }
}

void
AuxFunc::unpackByIndex(std::string archadress, std::string outfolder,
                       int index)
{
  zip_t *z;
  zip_file_t *file;
  zip_stat_t st;
  int er = 0;
  z = zip_open(archadress.c_str(), ZIP_RDONLY, &er);
  if(er < 1)
    {

      std::filesystem::path path = std::filesystem::u8path(outfolder);
      if(!std::filesystem::exists(path))
        {
          std::filesystem::create_directories(path);
        }
      file = zip_fopen_index(z, index, ZIP_FL_ENC_UTF_8);
      zip_stat_index(z, index, ZIP_STAT_NAME, &st);
      char content;
      std::fstream f;
      std::string fname = path.u8string();
      fname = fname + "/";
      std::string auxstr = std::string(st.name);
      path = std::filesystem::u8path(auxstr);
      auxstr = path.filename().u8string();
      fname = fname + auxstr;
      path = std::filesystem::u8path(fname);
      f.open(path, std::ios_base::out | std::ios_base::binary);
      for(;;)
        {
          zip_uint64_t ch = zip_fread(file, &content, sizeof(content));
          if(ch <= 0)
            {
              if(ch < 0)
                {
                  std::cerr << "unpackByIndex file  reading error" << std::endl;
                }
              break;
            }
          f.write(&content, sizeof(content));
        }
      f.close();
      zip_fclose(file);
      zip_close(z);
    }
  else
    {
      std::cout << "Unpack by index error: " << strerror(er) << std::endl;
    }
}

std::string
AuxFunc::get_selfpath()
{
  std::filesystem::path p;
  #ifdef __linux
  p = std::filesystem::u8path("/proc/self/exe");
  return std::filesystem::read_symlink(p).u8string();
  #endif
  #ifdef __WIN32
  char pth [MAX_PATH];
  GetModuleFileNameA(NULL, pth, MAX_PATH);
  p = std::filesystem::path(pth);
  return p.u8string();
  #endif
}

void
AuxFunc::toutf8(std::string &line)
{
  const std::string::size_type srclen = line.size();
  std::vector<UChar> target(srclen);
  UErrorCode status = U_ZERO_ERROR;
  UConverter *conv = ucnv_open(NULL, &status);
  int32_t len = ucnv_toUChars(conv, target.data(), srclen, line.c_str(),
                              srclen, &status);
  ucnv_close(conv);
  if(!U_SUCCESS(status))
    {
      std::cout << u_errorName(status) << std::endl;
    }
  icu::UnicodeString ustr(target.data(), len);
  line.clear();
  ustr.toUTF8String(line);
}

std::string
AuxFunc::utf8to(std::string line)
{
  UErrorCode status = U_ZERO_ERROR;
  icu::UnicodeString ustr;
  UConverter *c = ucnv_open(NULL, &status);
  if(!U_SUCCESS(status))
    {
      std::cerr << u_errorName(status) << std::endl;
    }
  status = U_ZERO_ERROR;
  std::vector<char> target2;
  ustr.remove();
  ustr = icu::UnicodeString::fromUTF8(line.c_str());
  target2.resize(ustr.length());
  char16_t data[ustr.length()];
  for(int i = 0; i < ustr.length(); i++)
    {
      data[i] = ustr.charAt(i);
    }
  size_t cb = ucnv_fromUChars(c, target2.data(), ustr.length(), data,
                              ustr.length(), &status);
  if(!U_SUCCESS(status))
    {
      if(status == U_BUFFER_OVERFLOW_ERROR)
        {
          status = U_ZERO_ERROR;
          target2.clear();
          target2.resize(cb);
          ucnv_fromUChars(c, target2.data(), cb, data, ustr.length(), &status);
          if(!U_SUCCESS(status))
            {
              std::cerr << u_errorName(status) << std::endl;
            }
        }
      else
        {
          std::cerr << u_errorName(status) << std::endl;
        }
    }
  line.clear();
  line = std::string(target2.begin(), target2.end());
  ucnv_close(c);

  return line;
}
