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
#ifndef SECURITY_H
#define SECURITY_H

#include <string>

class Security
{
public:
  Security();

  Security(const Security &other);

  Security(Security &&other);

  Security &
  operator=(const Security &other);

  Security &
  operator=(Security &&other);

  std::string id;
  std::string short_name;
  std::string name;
  std::string board_id;
  std::string history_from;
  std::string history_till;
};

#endif // SECURITY_H
