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
#ifndef FILEREADER_H
#define FILEREADER_H

#include <Board.h>
#include <Deal.h>
#include <PPM.h>
#include <Security.h>
#include <TradeDay.h>
#include <filesystem>
#include <vector>

class FileReader
{
public:
  FileReader();

  std::vector<TradeDay>
  readAll(const std::filesystem::path &fp);

  std::vector<Security>
  readSecurities(const std::filesystem::path &fp);

  std::vector<PPM>
  readPPM(const std::filesystem::path &fp);

  std::vector<Board>
  readBoards(const std::filesystem::path &fp);

  std::vector<Deal>
  readDeals(const std::filesystem::path &fp);
};

#endif // FILEREADER_H
