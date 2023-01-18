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

#include <gtkmm.h>
#include <libintl.h>
#include "MoneyApplication.h"

std::string MONEY_CA_CERT;

int
main(int argc, char *argv[])
{
  std::vector<int> numb;
  for(int i = 0; i < argc; i++)
    {
      if(argv[i])
	{
	  std::string cmdl(argv[i]);
	  std::string::size_type n;
	  n = cmdl.find("--ca-cert=");
	  if(n != std::string::npos)
	    {
	      cmdl.erase(0, cmdl.find("=") + std::string("=").size());
	      if(!cmdl.empty())
		{
		  MONEY_CA_CERT = cmdl;
		}
	    }
	  else
	    {
	      numb.push_back(i);
	    }
	}
    }
  argc = static_cast<int>(numb.size());
  char *argm[numb.size()];
  for(size_t i = 0; i < numb.size(); i++)
    {
      argm[i] = argv[numb[i]];
    }

  char *fnm = getenv("MONEY_CA_CERT");
  if(fnm)
    {
      MONEY_CA_CERT = std::string(fnm);
    }
  AuxFunc af;
  af.toutf8(MONEY_CA_CERT);
  MONEY_CA_CERT.erase(
      std::remove_if(MONEY_CA_CERT.begin(), MONEY_CA_CERT.end(), []
      (auto &el)
	{
	  if(el == '\'' || el == '\"')
	    {
	      return true;
	    }
	  else
	    {
	      return false;
	    }
	}),
      MONEY_CA_CERT.end());
  std::string Sharepath;
  std::filesystem::path p(std::filesystem::u8path(af.get_selfpath()));
  Sharepath = p.parent_path().u8string() + "/../share/locale";
  bindtextdomain("Money", Sharepath.c_str());
  bind_textdomain_codeset("Money", "UTF-8");
  textdomain("Money");
  auto app = MoneyApplication::create();
  return app->run(argc, argm);
}
