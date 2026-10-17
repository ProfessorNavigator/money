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

#include <CoeffModel.h>
#include <MoneyAux.h>

CoeffModel::CoeffModel(QObject *parent) : QAbstractTableModel(parent)
{
}

int
CoeffModel::rowCount(const QModelIndex &parent) const
{
  if(parent.isValid())
    {
      return 0;
    }

  return static_cast<int>(model.size());
}

int
CoeffModel::columnCount(const QModelIndex &parent) const
{
  if(parent.isValid())
    {
      return 0;
    }

  return 3;
}

QVariant
CoeffModel::data(const QModelIndex &index, int role) const
{
  QVariant result;

  if(!index.isValid())
    {
      return result;
    }

  if(static_cast<size_t>(index.row()) >= model.size())
    {
      return result;
    }

  switch(index.column())
    {
    case 0:
      {
        switch(role)
          {
          case Qt::DisplayRole:
            {
              int day, month, year, hour, minute;
              double sec;
              MoneyAux::julianDateToDate(std::get<0>(model[index.row()]), day,
                                         month, year, hour, minute, sec);
              QString val;
              if(day < 10)
                {
                  val = "0";
                }
              val += QString::number(day);
              val += ".";
              if(month < 10)
                {
                  val += "0";
                }
              val += QString::number(month);
              val += ".";
              val += QString::number(year);

              result = QVariant(val);
              break;
            }
          case Qt::TextAlignmentRole:
            {
              result = QVariant(Qt::AlignCenter);
              break;
            }
          default:
            break;
          }
        break;
      }
    case 1:
      {
        switch(role)
          {
          case Qt::DisplayRole:
            {
              int day, month, year, hour, minute;
              double sec;
              MoneyAux::julianDateToDate(std::get<1>(model[index.row()]), day,
                                         month, year, hour, minute, sec);
              QString val;
              if(day < 10)
                {
                  val = "0";
                }
              val += QString::number(day);
              val += ".";
              if(month < 10)
                {
                  val += "0";
                }
              val += QString::number(month);
              val += ".";
              val += QString::number(year);

              result = QVariant(val);
              break;
            }
          case Qt::TextAlignmentRole:
            {
              result = QVariant(Qt::AlignCenter);
              break;
            }
          default:
            break;
          }
        break;
      }
    case 2:
      {
        switch(role)
          {
          case Qt::DisplayRole:
            {
              QString val
                  = QString::number(std::get<2>(model[index.row()]), 'f', 10);
              result = QVariant(val);
              break;
            }
          case Qt::TextAlignmentRole:
            {
              result = QVariant(Qt::AlignCenter);
              break;
            }
          default:
            break;
          }
        break;
      }
    default:
      break;
    }

  return result;
}

QVariant
CoeffModel::headerData(int section, Qt::Orientation orientation,
                       int role) const
{
  QVariant result;

  switch(orientation)
    {
    case Qt::Horizontal:
      {
        switch(role)
          {
          case Qt::DisplayRole:
            {
              switch(section)
                {
                case 0:
                  {
                    QString val(tr("Begin date"));
                    result = QVariant(val);
                    break;
                  }
                case 1:
                  {
                    QString val(tr("End date"));
                    result = QVariant(val);
                    break;
                  }
                case 2:
                  {
                    QString val(tr("Coefficient"));
                    result = QVariant(val);
                  }
                default:
                  break;
                }
              break;
            }
          case Qt::TextAlignmentRole:
            {
              result = QVariant(Qt::AlignCenter);
              break;
            }
          default:
            break;
          }
        break;
      }
    case Qt::Vertical:
      {
        switch(role)
          {
          case Qt::DisplayRole:
            {
              QString val = QString::number(section + 1);
              result = QVariant(val);
              break;
            }
          case Qt::TextAlignmentRole:
            {
              result = QVariant(Qt::AlignCenter);
              break;
            }
          default:
            break;
          }
        break;
      }
    default:
      break;
    }

  return result;
}

void
CoeffModel::addCoef(const double &date_begin, const double &date_end,
                    const double &coef)
{
  beginInsertRows(QModelIndex(), static_cast<int>(model.size()),
                  static_cast<int>(model.size()));
  model.push_back(std::make_tuple(date_begin, date_end, coef));
  endInsertRows();
}

void
CoeffModel::removeCoef(const QModelIndex &index)
{
  if(!index.isValid())
    {
      return void();
    }
  if(static_cast<size_t>(index.row()) >= model.size())
    {
      return void();
    }

  beginRemoveRows(QModelIndex(), index.row(), index.row());
  model.erase(model.begin() + index.row());
  endRemoveRows();
}

void
CoeffModel::removeAll()
{
  beginResetModel();
  model.clear();
  endResetModel();
}

std::vector<std::tuple<double, double, double>>
CoeffModel::getCoef()
{
  return model;
}
