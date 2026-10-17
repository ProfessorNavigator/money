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

#include <AddCoefWindow.h>
#include <MoneyAux.h>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <ctime>

AddCoefWindow::AddCoefWindow(QWidget *parent) : QWidget(parent)
{
  this->setWindowFlag(Qt::Window);
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowModality(Qt::WindowModal);

  createWindow();
}

void
AddCoefWindow::createWindow()
{
  QGridLayout *grid = new QGridLayout;
  this->setLayout(grid);

  QLabel *lab = new QLabel;
  lab->setText(tr("Begin date") + ":");
  grid->addWidget(lab, 0, 0, Qt::AlignLeft);

  QLineEdit *begin_date = new QLineEdit;
  begin_date->setAlignment(Qt::AlignCenter);
  QString mask("00.00.0000");
  begin_date->setInputMask(mask);
  QFontMetrics fm = begin_date->fontMetrics();
  QRect rect = fm.boundingRect(mask);
  QMargins mg = begin_date->textMargins();
  begin_date->setFixedWidth(rect.width() + mg.left() + mg.right()
                            + 2 * fm.averageCharWidth());
  grid->addWidget(begin_date, 1, 0, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(tr("End date") + ":");
  grid->addWidget(lab, 0, 1, Qt::AlignLeft);

  QLineEdit *end_date = new QLineEdit;
  end_date->setAlignment(Qt::AlignCenter);
  end_date->setInputMask(mask);
  fm = end_date->fontMetrics();
  rect = fm.boundingRect(mask);
  mg = end_date->textMargins();
  end_date->setFixedWidth(rect.width() + mg.left() + mg.right()
                          + 2 * fm.averageCharWidth());
  grid->addWidget(end_date, 1, 1, Qt::AlignLeft);

  lab = new QLabel;
  lab->setText(tr("Coefficient") + ":");
  grid->addWidget(lab, 2, 0, 1, 2, Qt::AlignCenter);

  QLineEdit *coef = new QLineEdit;
  coef->setAlignment(Qt::AlignCenter);
  fm = coef->fontMetrics();
  rect = fm.boundingRect(mask);
  mg = coef->textMargins();
  coef->setFixedWidth(mg.left() + mg.right() + 7 * fm.averageCharWidth());
  grid->addWidget(coef, 3, 0, 1, 2, Qt::AlignCenter);

  QHBoxLayout *h_box = new QHBoxLayout;
  grid->addLayout(h_box, 4, 0, 1, 2);

  QPushButton *add = new QPushButton;
  add->setText(tr("Add"));
  connect(add, &QPushButton::clicked, this,
          [this, begin_date, end_date, coef]
            {
              std::stringstream strm;
              strm.str(begin_date->text().toStdString());
              std::tm tm = {};
              strm >> std::get_time(&tm, "%d.%m.%Y");
              double db = MoneyAux::dateToJulianDay(
                  tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, 0, 0, 0.0);

              strm.clear();
              strm.str(end_date->text().toStdString());
              tm = {};
              strm >> std::get_time(&tm, "%d.%m.%Y");
              double de = MoneyAux::dateToJulianDay(
                  tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, 0, 0, 0.0);

              if(de < db)
                {
                  return void();
                }

              double c = coef->text().toDouble();
              if(c <= 0.0)
                {
                  return void();
                }
              emit signalCoef(db, de, c);
              this->close();
            });
  h_box->addWidget(add, 0, Qt::AlignCenter);

  QPushButton *cancel = new QPushButton;
  cancel->setText(tr("Cancel"));
  connect(cancel, &QPushButton::clicked, this, &AddCoefWindow::close);
  h_box->addWidget(cancel, 0, Qt::AlignCenter);
}
