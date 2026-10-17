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
#ifndef PPM_H
#define PPM_H

#include <gmpxx.h>

class PPM
{
public:
  PPM();

  PPM(const PPM &other);

  PPM(PPM &&other);

  PPM &
  operator=(const PPM &other);

  PPM &
  operator=(PPM &&other);

  void
  update(const PPM &other);

  void
  set(const mpf_class &turnover, const mpf_class &money_turnover);

  double date = 0.0;
  mpf_class turnover = 0.0;
  mpf_class money_turnover = 0.0;
  mpf_class current_ppm = 0.0;
  mpf_class total_ppm = 0.0;
};

#endif // PPM_H
