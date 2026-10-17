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

#include <LoadingDialog.h>
#include <QLabel>
#include <QVBoxLayout>

LoadingDialog::LoadingDialog(QWidget *parent) : QWidget(parent)
{
  this->setWindowFlag(Qt::Window);
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowModality(Qt::WindowModal);

  allow_close.store(false, std::memory_order_relaxed);

  createWindow();
}

void
LoadingDialog::allowClose(const bool &allow)
{
  allow_close.store(allow, std::memory_order_relaxed);
}

void
LoadingDialog::closeEvent(QCloseEvent *event)
{
  if(allow_close.load())
    {
      event->accept();
    }
  else
    {
      event->ignore();
    }
}

void
LoadingDialog::createWindow()
{
  QVBoxLayout *v_box = new QVBoxLayout;
  this->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setText(tr("Loading") + "...");
  v_box->addWidget(lab, 0, Qt::AlignCenter);
}
