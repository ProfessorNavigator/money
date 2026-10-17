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

#include <Security.h>

Security::Security()
{
}

Security::Security(const Security &other)
{
  id = other.id;
  short_name = other.short_name;
  name = other.name;
  board_id = other.board_id;
  history_from = other.history_from;
  history_till = other.history_till;
}

Security::Security(Security &&other)
{
  id = std::move(other.id);
  short_name = std::move(other.short_name);
  name = std::move(other.name);
  board_id = std::move(other.board_id);
  history_from = std::move(other.history_from);
  history_till = std::move(other.history_till);
}

Security &
Security::operator=(const Security &other)
{
  if(this != &other)
    {
      id = other.id;
      short_name = other.short_name;
      name = other.name;
      board_id = other.board_id;
      history_from = other.history_from;
      history_till = other.history_till;
    }
  return *this;
}

Security &
Security::operator=(Security &&other)
{
  if(this != &other)
    {
      id = std::move(other.id);
      short_name = std::move(other.short_name);
      name = std::move(other.name);
      board_id = std::move(other.board_id);
      history_from = std::move(other.history_from);
      history_till = std::move(other.history_till);
    }
  return *this;
}
