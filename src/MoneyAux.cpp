/*
 * Copyright (C) 2026 Yury Bobylev <bobilev_yury@mail.ru>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <MoneyAux.h>
#include <functional>
#include <iostream>
#include <math.h>
#include <memory>
#include <random>
#include <syncstream>
#include <unicode/ucnv.h>
#include <unicode/ucsdet.h>

MoneyAux::MoneyAux()
{
}

std::vector<std::string>
MoneyAux::detectEncoding(const std::string &src)
{
  std::vector<std::string> result;

  UErrorCode ec = U_ZERO_ERROR;

  std::unique_ptr<UCharsetDetector, std::function<void(UCharsetDetector *)>>
      det(ucsdet_open(&ec),
          [](UCharsetDetector *det)
            {
              if(det != nullptr)
                {
                  ucsdet_close(det);
                }
            });
  if(U_FAILURE(ec))
    {
      std::osyncstream(std::cout)
          << "MoneyAux::detectEncoding(1): " << u_errorName(ec) << std::endl;
      return result;
    }

  ec = U_ZERO_ERROR;
  ucsdet_setText(det.get(), src.c_str(), static_cast<int32_t>(src.size()),
                 &ec);
  if(U_FAILURE(ec))
    {
      std::osyncstream(std::cout)
          << "MoneyAux::detectEncoding(2): " << u_errorName(ec) << std::endl;
      return result;
    }

  int32_t sz = 0;
  ec = U_ZERO_ERROR;
  const UCharsetMatch **m = ucsdet_detectAll(det.get(), &sz, &ec);
  if(U_FAILURE(ec))
    {
      std::osyncstream(std::cout)
          << "MoneyAux::detectEncoding(3): " << u_errorName(ec) << std::endl;
      return result;
    }

  for(int32_t i = 0; i < sz; i++)
    {
      ec = U_ZERO_ERROR;
      const char *val = ucsdet_getName(m[i], &ec);
      if(U_FAILURE(ec))
        {
          std::osyncstream(std::cout)
              << "MoneyAux::detectEncoding(4): " << u_errorName(ec)
              << std::endl;
          continue;
        }
      if(val != nullptr)
        {
          std::string str(val);
          result.emplace_back(str);
        }
    }

  return result;
}

std::string
MoneyAux::convertToEncodign(const std::string &src, const std::string &from,
                            const std::string &to)
{
  std::string result;

  int32_t result_sz = static_cast<int32_t>(src.size());

  std::unique_ptr<char, std::function<void(char *)>> ptr(
      new char[result_sz + 1],
      [](char *ptr)
        {
          delete[] ptr;
        });

  const char *from_ptr = nullptr;
  if(from.size() > 0)
    {
      from_ptr = from.c_str();
    }
  const char *to_ptr = nullptr;
  if(to.size() > 0)
    {
      to_ptr = to.c_str();
    }

  for(;;)
    {
      UErrorCode ec = U_ZERO_ERROR;
      int32_t sz
          = ucnv_convert(to_ptr, from_ptr, ptr.get(), result_sz, src.c_str(),
                         static_cast<int32_t>(src.size()), &ec);
      bool stop = false;
      switch(ec)
        {
        case U_BUFFER_OVERFLOW_ERROR:
        case U_STRING_NOT_TERMINATED_WARNING:
          {
            result_sz = sz + 1;
            ptr = std::unique_ptr<char, std::function<void(char *)>>(
                new char[result_sz],
                [](char *ptr)
                  {
                    delete[] ptr;
                  });
            break;
          }
        case U_ZERO_ERROR:
          {
            stop = true;
            break;
          }
        default:
          {            
            if(U_FAILURE(ec))
              {
                std::osyncstream(std::cout)
                    << "MoneyAux::convertToEncodign: " << u_errorName(ec)
                    << std::endl;
                return result;
              }
            stop = true;
            break;
          }
        }
      if(stop)
        {
          break;
        }
    }

  char *p = ptr.get();
  result.reserve(result_sz - 1);
  for(size_t i = 0; i < result_sz - 1; i++)
    {
      result.push_back(p[i]);
    }

  return result;
}

double
MoneyAux::dateToJulianDay(int day, int month, int year, int hour, int minut,
                          double sec)
{
  double a1, m1, c1, a2;
  a1 = std::floor(static_cast<double>(month - 3) / 12.0);
  m1 = static_cast<double>(month) - 3.0 - 12.0 * a1;
  c1 = std::floor(static_cast<double>(year + a1) / 100.0);
  a2 = static_cast<double>(year) + a1 - 100.0 * c1;
  int J = static_cast<int>(std::floor(146097.0 * c1 / 4.0))
          + static_cast<int>(std::floor(36525.0 * a2 / 100.0))
          + static_cast<int>(std::floor((153.0 * m1 + 2.0) / 5.0)) + day
          + 1721119;
  double JD;
  if(J >= 0)
    {
      JD = static_cast<double>(J) + static_cast<double>(hour) / 24.0
           + static_cast<double>(minut) / 1440.0 + sec / 86400.0;
    }
  else
    {
      double k = static_cast<double>(hour) / 24.0
                 + static_cast<double>(minut) / 1440.0 + sec / 86400.0;
      k = 1 - k;
      JD = static_cast<double>(J) - k;
    }
  return JD;
}

void
MoneyAux::julianDateToDate(const double &JDN, int &day, int &month, int &year,
                           int &hour, int &minut, double &second)
{
  int J = static_cast<int>(JDN);
  int c1 = static_cast<int>(
      std::floor((4.0 * static_cast<double>(J) - 6884477.0) / 146097.0));
  int e1 = 4 * J - 6884477 - 146097 * c1;

  int a1 = (100 * static_cast<int>(std::floor(static_cast<double>(e1) / 4.0))
            + 99)
           / 36525;
  int e2 = 100 * static_cast<int>(std::floor(static_cast<double>(e1) / 4.0))
           + 99 - 36525 * a1;

  int m1
      = (5 * static_cast<int>(std::floor(static_cast<double>(e2) / 100.0)) + 2)
        / 153;
  int e3 = 5 * static_cast<int>(std::floor(static_cast<double>(e2) / 100.0))
           + 2 - 153 * m1;
  int a2 = static_cast<int>(std::floor(static_cast<double>(m1 + 2) / 12.0));
  int m2 = m1 + 2 - 12 * a2;
  year = 100 * c1 + a1 + a2;
  month = m2 + 1;
  day = static_cast<int>(std::floor(static_cast<double>(e3) / 5.0)) + 1;
  double F, p;
  F = std::modf(JDN, &p);
  if(F < 0.0)
    {
      F = 1 + F;
    }
  hour = static_cast<int>(F * 24.0);
  minut = static_cast<int>((F * 24.0 - static_cast<double>(hour)) * 60.0);
  second = ((F * 24.0 - static_cast<double>(hour)) * 60.0
            - static_cast<double>(minut))
           * 60.0;
}

template int64_t
MoneyAux::randomNumber();
template int32_t
MoneyAux::randomNumber();
template int16_t
MoneyAux::randomNumber();
template int8_t
MoneyAux::randomNumber();
template uint64_t
MoneyAux::randomNumber();
template uint32_t
MoneyAux::randomNumber();
template uint16_t
MoneyAux::randomNumber();
template uint8_t
MoneyAux::randomNumber();
template <typename T>
T
MoneyAux::randomNumber()
{
  std::numeric_limits<T> lim;
  std::uniform_int_distribution<T> dist(lim.min() + 1, lim.max());
  std::chrono::time_point<std::chrono::high_resolution_clock> ctm_p
      = std::chrono::high_resolution_clock::now();
  uint64_t ctm = std::chrono::duration_cast<std::chrono::nanoseconds>(
                     ctm_p.time_since_epoch())
                     .count();
  std::mt19937_64 lrng(ctm);
  return dist(lrng);
}

std::filesystem::path
MoneyAux::randomFileName()
{
  std::filesystem::path result;

  uint64_t num = randomNumber<uint64_t>();
  std::stringstream strm;
  strm.imbue(std::locale("C"));
  strm << std::hex << num;
  std::string str = strm.str();
  result = std::u8string(str.begin(), str.end());

  return result;
}
