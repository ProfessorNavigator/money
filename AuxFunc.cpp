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

AuxFunc::AuxFunc ()
{
  // TODO Auto-generated constructor stub
}

AuxFunc::~AuxFunc ()
{
  // TODO Auto-generated destructor stub
}

int
AuxFunc::grigtojulian (int day, int month, int year)
{
  int a, y, m, JDN;
  a = (14 - month) / 12;
  y = year + 4800 - a;
  m = month + 12 * a - 3;
  JDN = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  return JDN;
}

std::string
AuxFunc::togrigday (int JDN)
{
  int a, b, c, d, e, m, day;
  a = (int) (JDN + 0.5) + 32044;
  b = (int) ((4 * a + 3) / 146097);
  c = a - (int) (146097 * b / 4);
  d = (int) ((4 * c + 3) / 1461);
  e = c - (int) (1461 * d / 4);
  m = (int) ((5 * e + 2) / 153);
  day = e - (int) ((153 * m + 2) / 5) + 1;

  std::string result;
  if (day < 10)
    {
      result = "0" + std::to_string (day);
    }
  else
    {
      result = std::to_string (day);
    }

  return result;
}

std::string
AuxFunc::togrigmonth (int JDN)
{
  int a, b, c, d, e, m, month;
  a = static_cast<int> (JDN + 0.5) + 32044;
  b = static_cast<int> ((4 * a + 3) / 146097);
  c = a - static_cast<int> (146097 * b / 4);
  d = static_cast<int> ((4 * c + 3) / 1461);
  e = c - static_cast<int> (1461 * d / 4);
  m = static_cast<int> ((5 * e + 2) / 153);
  month = m + 3 - 12 * static_cast<int> (m / 10);

  std::string result;
  if (month < 10)
    {
      result = "0" + std::to_string (month);
    }
  else
    {
      result = std::to_string (month);
    }

  return result;
}

std::string
AuxFunc::togrigyear (int JDN)
{
  int a, b, c, d, e, m, year;
  a = static_cast<int> (JDN + 0.5) + 32044;
  b = static_cast<int> ((4 * a + 3) / 146097);
  c = a - static_cast<int> (146097 * b / 4);
  d = static_cast<int> ((4 * c + 3) / 1461);
  e = c - static_cast<int> (1461 * d / 4);
  m = static_cast<int> ((5 * e + 2) / 153);
  year = 100 * b + d - 4800 + static_cast<int> (m / 10);

  std::string result;
  if (year < 10)
    {
      result = "0" + std::to_string (year);
    }
  else
    {
      result = std::to_string (year);
    }

  return result;
}

void
AuxFunc::callNetwork (std::string urli, std::string filename, int *check)
{
  CURL *curl;
  FILE *fp;
  CURLcode res;
  curl = curl_easy_init ();
#ifdef _WIN32
  std::filesystem::path certp = std::filesystem::u8path (get_selfpath ());
  certp = certp.parent_path ();
  std::string certstr = certp.u8string ();
  certstr = certstr + "/ca-bundle.trust.crt";
#endif

  if (curl)
    {
      std::filesystem::path p = std::filesystem::u8path (filename);
      fp = fopen (p.string ().c_str (), "wb");
      curl_easy_setopt(curl, CURLOPT_URL, urli.c_str ());
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
#ifdef _WIN32
      curl_easy_setopt(curl, CURLOPT_CAINFO, certstr.c_str ());
#endif
      res = curl_easy_perform (curl);
      if (res != CURLE_OK)
	{
	  *check = 0;
	  std::cerr << "curl_easy_perform() failed: "
	      << curl_easy_strerror (res) << std::endl;
	  fclose (fp);
	}
      else
	{
	  *check = 1;
	  fclose (fp);
	}
      curl_easy_cleanup (curl);

    }
}

void
AuxFunc::cp1251toutf8 (std::string &line)
{
  const std::string::size_type srclen = line.size ();
  std::vector<UChar> target (srclen);
  UErrorCode status = U_ZERO_ERROR;
  UConverter *conv = ucnv_open ("cp1251", &status);
  int32_t len = ucnv_toUChars (conv, target.data (), srclen, line.c_str (),
			       srclen, &status);
  ucnv_close (conv);
  if (!U_SUCCESS (status))
    {
      std::cout << u_errorName (status) << std::endl;
    }
  icu::UnicodeString ustr (target.data (), len);
  line.clear ();
  ustr.toUTF8String (line);
}

void
AuxFunc::stringToLower (std::string &line)
{
  std::string innerline = line;
  icu::UnicodeString ustr = icu::UnicodeString::fromUTF8 (innerline.c_str ());
  ustr.toLower ();
  line.clear ();
  ustr.toUTF8String (line);
}

void
AuxFunc::homePath (std::string *filename)
{
  try
    {
      *filename = std::string (getenv ("USERPROFILE"));
    }
  catch (std::exception &e)
    {
      try
	{
	  *filename = std::string (getenv ("HOMEDRIVE"));
	}
      catch (std::exception &e)
	{
	  try
	    {
	      *filename = std::string (getenv ("HOMEPATH"));
	    }
	  catch (std::exception &e)
	    {
	      try
		{
		  *filename = std::string (getenv ("HOME"));
		}
	      catch (std::exception &e)
		{
		  try
		    {
		      *filename = std::string (getenv ("SystemDrive"));
		    }
		  catch (std::exception &e)
		    {
		      exit (1);
		    }
		}
	    }
	}
    }

  toutf8 (*filename);
}

int
AuxFunc::addYears (int JulianDay, int numyears)
{
  int day, month, year;
  std::string daystr, monthstr, yearstr;
  daystr = togrigday (JulianDay);

  std::stringstream strm;
  strm << daystr;
  strm >> day;

  monthstr = togrigmonth (JulianDay);
  strm.str ("");
  strm.clear ();
  strm << monthstr;
  strm >> month;

  yearstr = togrigyear (JulianDay);
  strm.str ("");
  strm.clear ();
  strm << yearstr;
  strm >> year;
  year = year + numyears;
  int a, y, m, JDN;
  a = (14 - month) / 12;
  y = year + 4800 - a;
  m = month + 12 * a - 3;
  JDN = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  return JDN;
}

void
AuxFunc::packing (std::string source, std::string out)
{
  int er = 0;
  std::filesystem::path dir;
  dir = std::filesystem::u8path (source);
  source = dir.generic_u8string ();
  if (std::filesystem::exists (dir))
    {
      if (std::filesystem::is_directory (dir))
	{
	  zip_t *z;
	  zip_error_t err;
	  z = zip_open (out.c_str (), ZIP_TRUNCATE | ZIP_CREATE, &er);
	  if (er < 1)
	    {
	      std::vector<std::filesystem::path> listf;
	      std::vector<std::filesystem::path> listd;
	      std::string line;
	      if (!std::filesystem::is_empty (dir))
		{
		  for (auto &iter : std::filesystem::recursive_directory_iterator (
		      dir))
		    {
		      std::filesystem::path path = iter.path ();
		      path = std::filesystem::u8path (path.generic_u8string ());
		      if (std::filesystem::is_directory (path))
			{
			  listd.push_back (path);
			}
		      else
			{
			  listf.push_back (path);
			}
		    }
		  std::sort (listd.begin (), listd.end (), []
		  (auto &el1, auto &el2)
		    {
		      return el1.string ().size () < el2.string ().size ();
		    });

		  std::string pardir = dir.filename ().u8string ();
		  zip_dir_add (z, pardir.c_str (), ZIP_FL_ENC_UTF_8);

		  for (size_t i = 0; i < listd.size (); i++)
		    {
		      line = listd[i].u8string ();
		      std::string::size_type n;
		      n = line.find (source, 0);
		      line.erase (n, source.size ());
		      line = pardir + line;
		      if (!std::filesystem::is_empty (listd[i]))
			{
			  zip_dir_add (z, line.c_str (), ZIP_FL_ENC_UTF_8);
			}
		    }
		  for (size_t i = 0; i < listf.size (); i++)
		    {
		      line = listf[i].u8string ();
		      std::string::size_type n;
		      n = line.find (source, 0);
		      line.erase (n, source.size ());
		      zip_source_t *zsource;
		      zsource = zip_source_file_create (
			  listf[i].u8string ().c_str (), 0, 0, &err);
		      line = pardir + line;
		      zip_file_add (z, line.c_str (), zsource,
		      ZIP_FL_ENC_UTF_8);
		    }
		}

	      zip_close (z);
	    }
	  else
	    {
	      std::cerr << "Error on packaing: " << strerror (er) << std::endl;
	    }
	}
      else
	{
	  zip_t *z;
	  zip_error_t err;
	  std::string line = dir.filename ().u8string ();
	  z = zip_open (out.c_str (), ZIP_TRUNCATE | ZIP_CREATE, &er);
	  if (er >= 1)
	    {
	      std::cerr << "Packing (file) error: " << strerror (er)
		  << std::endl;
	    }
	  else
	    {
	      zip_source_t *zsource;
	      zsource = zip_source_file_create (source.c_str (), 0, 0, &err);
	      if (zsource == nullptr)
		{
		  std::cerr << "Error on open file while packing" << std::endl;
		}
	      else
		{
		  zip_file_add (z, line.c_str (), zsource, ZIP_FL_ENC_UTF_8);
		}
	      zip_close (z);
	    }
	}
    }
}

int
AuxFunc::fileNames (std::string adress, std::vector<std::string> &filenames)
{
  zip_t *z;
  std::string flname;
  int er = 0;
  int num;
  z = zip_open (adress.c_str (), ZIP_RDONLY, &er);
  if (er < 1)
    {
      num = zip_get_num_files (z);

      for (int i = 0; i < num; i++)
	{
	  flname = zip_get_name (z, i, ZIP_FL_ENC_UTF_8);
	  filenames.push_back (flname);
	}
      zip_close (z);
    }
  else
    {
      std::cerr << "Error on getting file names from archive: " << strerror (er)
	  << std::endl;
    }

  return er;
}

int
AuxFunc::fileNames (std::string adress,
		    std::vector<std::tuple<int, int, std::string>> &filenames)
{
  zip_t *z;

  std::string flname;
  int er = 0;
  int num;
  z = zip_open (adress.c_str (), ZIP_RDONLY, &er);
  if (er < 1)
    {
      num = zip_get_num_files (z);

      for (int i = 0; i < num; i++)
	{
	  flname = zip_get_name (z, i, ZIP_FL_ENC_UTF_8);
	  struct zip_stat st;
	  zip_stat_index (z, i, ZIP_FL_ENC_GUESS, &st);
	  int sz = st.size;
	  std::tuple<int, int, std::string> tuple;
	  tuple = std::make_tuple (i, sz, flname);
	  filenames.push_back (tuple);
	}
      zip_close (z);
    }
  else
    {
      std::cerr << "Error on getting file names from archive: " << strerror (er)
	  << std::endl;
    }

  return er;
}

void
AuxFunc::unpacking (std::string archadress, std::string outfolder)
{
  std::vector<std::string> filenames;
  std::string line, archnm;
  fileNames (archadress, filenames);
  for (size_t i = 0; i < filenames.size (); i++)
    {
      line = filenames[i];
      std::filesystem::path path;
      if (line.substr (line.size () - 1, line.size () - 1) == "/")
	{
	  line = outfolder + "/" + line;
	  path = std::filesystem::u8path (line);
	  std::filesystem::create_directories (path);
	}
    }
  zip_t *z;
  zip_file_t *file;
  zip_stat_t st;
  int er = 0;
  z = zip_open (archadress.c_str (), ZIP_RDONLY, &er);
  if (er < 1)
    {
      for (size_t i = 0; i < filenames.size (); i++)
	{
	  line = filenames[i];
	  if (line.substr (line.size () - 1, line.size () - 1) != "/")
	    {
	      file = zip_fopen (z, line.c_str (), ZIP_FL_ENC_UTF_8);
	      zip_stat (z, line.c_str (), ZIP_STAT_NAME | ZIP_FL_ENC_UTF_8,
			&st);
	      std::vector<char> content;
	      std::filesystem::path path;
	      line = outfolder + "/" + line;
	      path = std::filesystem::u8path (line);
	      std::fstream f;
	      f.open (path, std::ios_base::out);
	      for (zip_uint64_t i = 0; i < st.size; i++)
		{
		  content.clear ();
		  content.resize (1);
		  zip_fread (file, content.data (), 1);
		  f.write (content.data (), content.size ());
		}
	      f.close ();
	      zip_fclose (file);
	    }
	}

      zip_close (z);
    }
  else
    {
      std::cout << strerror (er) << std::endl;
    }
}

void
AuxFunc::unpackByIndex (std::string archadress, std::string outfolder,
			int index)
{
  zip_t *z;
  zip_file_t *file;
  zip_stat_t st;
  int er = 0;
  z = zip_open (archadress.c_str (), ZIP_RDONLY, &er);
  if (er < 1)
    {

      std::filesystem::path path = std::filesystem::u8path (outfolder);
      if (!std::filesystem::exists (path))
	{
	  std::filesystem::create_directories (path);
	}
      file = zip_fopen_index (z, index, ZIP_FL_ENC_UTF_8);
      zip_stat_index (z, index, ZIP_STAT_NAME, &st);
      std::vector<char> content;
      std::fstream f;
      std::string fname = path.u8string ();
      fname = fname + "/";
      std::string auxstr = std::string (st.name);
      path = std::filesystem::u8path (auxstr);
      auxstr = path.filename ().u8string ();
      fname = fname + auxstr;
      path = std::filesystem::u8path (fname);
      f.open (path, std::ios_base::out);
      for (zip_uint64_t i = 0; i < st.size; i++)
	{
	  content.clear ();
	  content.resize (1);
	  zip_fread (file, content.data (), 1);
	  f.write (content.data (), content.size ());
	}
      f.close ();
      zip_fclose (file);
      zip_close (z);
    }
  else
    {
      std::cout << "Unpack by index error: " << strerror (er) << std::endl;
    }
}

std::string
AuxFunc::get_selfpath ()
{
  std::filesystem::path p;
#ifdef __linux
  p = std::filesystem::u8path ("/proc/self/exe");
  return std::filesystem::read_symlink (p).u8string ();
#endif
#ifdef __WIN32
  char pth [MAX_PATH];
  GetModuleFileNameA (NULL, pth, MAX_PATH);
  p = std::filesystem::path (pth);
  return p.u8string ();
#endif
}

void
AuxFunc::toutf8 (std::string &line)
{
  const std::string::size_type srclen = line.size ();
  std::vector<UChar> target (srclen);
  UErrorCode status = U_ZERO_ERROR;
  UConverter *conv = ucnv_open (NULL, &status);
  int32_t len = ucnv_toUChars (conv, target.data (), srclen, line.c_str (),
			       srclen, &status);
  ucnv_close (conv);
  if (!U_SUCCESS (status))
    {
      std::cout << u_errorName (status) << std::endl;
    }
  icu::UnicodeString ustr (target.data (), len);
  line.clear ();
  ustr.toUTF8String (line);
}

std::string
AuxFunc::utf8to (std::string line)
{
  UErrorCode status = U_ZERO_ERROR;
  icu::UnicodeString ustr;
  UConverter *c = ucnv_open (NULL, &status);
  if (!U_SUCCESS (status))
    {
      std::cerr << u_errorName (status) << std::endl;
    }
  status = U_ZERO_ERROR;
  std::vector<char> target2;
  ustr.remove ();
  ustr = icu::UnicodeString::fromUTF8 (line.c_str ());
  target2.resize (ustr.length ());
  char16_t data[ustr.length ()];
  for (int i = 0; i < ustr.length (); i++)
    {
      data[i] = ustr.charAt (i);
    }
  size_t cb = ucnv_fromUChars (c, target2.data (), ustr.length (), data,
			       ustr.length (), &status);
  if (!U_SUCCESS (status))
    {
      std::cerr << u_errorName (status) << std::endl;
    }
  if (status == U_BUFFER_OVERFLOW_ERROR)
    {
      status = U_ZERO_ERROR;
      target2.clear ();
      target2.resize (cb);
      ucnv_fromUChars (c, target2.data (), cb, data, ustr.length (), &status);
      if (!U_SUCCESS (status))
	{
	  std::cerr << u_errorName (status) << std::endl;
	}
    }
  line.clear ();
  line = std::string (target2.begin (), target2.end ());

  return line;
}
