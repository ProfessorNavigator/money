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
#ifndef BOARD_H
#define BOARD_H

#include <string>

class Board
{
public:
  Board();

  Board(const Board &other);

  Board(Board &&other);

  Board &
  operator=(const Board &other);

  Board &
  operator=(Board &&other);

  std::string id;
  std::string name;
};

#endif // BOARD_H
