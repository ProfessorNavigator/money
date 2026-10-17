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
#ifndef COEFFMODEL_H
#define COEFFMODEL_H

#include <QAbstractTableModel>
#include <tuple>
#include <vector>

class CoeffModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  CoeffModel(QObject *parent = nullptr);

  int
  rowCount(const QModelIndex &parent = QModelIndex()) const override;

  int
  columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant
  data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  QVariant
  headerData(int section, Qt::Orientation orientation,
             int role = Qt::DisplayRole) const override;

  void
  addCoef(const double &date_begin, const double &date_end,
          const double &coef);

  void
  removeCoef(const QModelIndex &index);

  void
  removeAll();

  std::vector<std::tuple<double, double, double>>
  getCoef();

private:
  /*0 - date begin, 1 - date end, 2 - coefficient*/
  std::vector<std::tuple<double, double, double>> model;
};

#endif // COEFFMODEL_H
