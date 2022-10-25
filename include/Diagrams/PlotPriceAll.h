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

#ifndef PLOTPRICEALL_H_
#define PLOTPRICEALL_H_

#include <mgl2/mgl.h>
#include <string>
#include <filesystem>
#include <vector>
#include <gmpxx.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <libintl.h>
#include "AuxFunc.h"

class PlotPriceAll
{
  public:
    PlotPriceAll(
      std::string file,
      int Height,
      int Width,
      std::vector<std::tuple<std::string, double, double, double, double>> *Plotdate);
    virtual
    ~PlotPriceAll();
    int
    Draw(mglGraph *gr);
  private:
    void
    calcForDraw();
    std::filesystem::path filepath;
    int height;
    int width;
    std::vector<double> Pricemid;
    std::vector<double> Pricebeg;
    std::vector<double> Priceend;
    std::vector<double> Tc;
    std::string datebeg;
    std::string dateend;
    std::vector<std::tuple<std::string, double, double, double, double>> *plotdate;
    std::vector<std::string> usdv;
    std::vector<std::string> eurv;
};

#endif /* PLOTPRICEALL_H_ */
