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

#include <Board.h>

Board::Board()
{
}

Board::Board(const Board &other)
{
  id = other.id;
  name = other.name;
}

Board::Board(Board &&other)
{
  id = std::move(other.id);
  name = std::move(other.name);
}

Board &
Board::operator=(const Board &other)
{
  if(this != &other)
    {
      id = other.id;
      name = other.name;
    }

  return *this;
}

Board &
Board::operator=(Board &&other)
{
  if(this != &other)
    {
      id = std::move(other.id);
      name = std::move(other.name);
    }

  return *this;
}
