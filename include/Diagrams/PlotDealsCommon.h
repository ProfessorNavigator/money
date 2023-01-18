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

#ifndef PLOTDEALSCOMMON_H_
#define PLOTDEALSCOMMON_H_

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

class PlotDealsCommon
{
  public:
    PlotDealsCommon(std::string file, int Height, int Width,
                    std::vector<std::tuple<std::string, double, double, double, double>> *Plotdate);
    virtual
    ~PlotDealsCommon();
    int
    Draw(mglGraph *gr);
  private:
    void
    calcForDraw();
    std::filesystem::path filename;
    int height, width;
    std::string datebeg;
    std::string dateend;
    std::string date1;
    std::string datehalf;
    std::string date2;
    std::vector<double> MRSP;
    std::vector<double> Tc;
    std::vector<double> Dc;
    std::vector<std::tuple<std::string, double, double, double, double>> *plotdate;
    std::vector<std::string> usdv;
    std::vector<std::string> eurv;
    std::vector<std::string> cnyv;
    std::vector<double> sharesvol;
    std::string yname;
};

#endif /* PLOTDEALSCOMMON_H_ */
