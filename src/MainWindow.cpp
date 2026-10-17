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

#include <CommonGraphSelectionWindow.h>
#include <DealsGraphSelectionWindow.h>
#include <DownloadTechnicalInfoWindow.h>
#include <DownloadWindow.h>
#include <LoadingDialog.h>
#include <MainWindow.h>
#include <PPMGraphSelectionWindow.h>
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  createWindow();
}

MainWindow::~MainWindow()
{
}

void
MainWindow::createWindow()
{
  QWidget *central_widget = new QWidget;
  this->setCentralWidget(central_widget);

  QVBoxLayout *v_box = new QVBoxLayout;
  central_widget->setLayout(v_box);

  QPushButton *common = new QPushButton;
  common->setText(tr("Common diagrams"));
  connect(common, &QPushButton::clicked, this,
          [this]
            {
              CommonGraphSelectionWindow *cgsw
                  = new CommonGraphSelectionWindow(this);
              cgsw->show();
            });
  v_box->addWidget(common);

  QPushButton *deals = new QPushButton;
  deals->setText(tr("Deals diagrams"));
  connect(deals, &QPushButton::clicked, this,
          [this]
            {
              LoadingDialog *ld = new LoadingDialog(this);
              ld->show();

              DealsGraphSelectionWindow *dgsw
                  = new DealsGraphSelectionWindow(this);
              connect(dgsw, &DealsGraphSelectionWindow::signalLoaded, ld,
                      [ld, dgsw]
                        {
                          dgsw->show();
                          ld->allowClose(true);
                          ld->close();
                        });
              dgsw->createWindow();
            });
  v_box->addWidget(deals);

  QPushButton *ppm_diagrams = new QPushButton;
  ppm_diagrams->setText(tr("PPM diagrams"));
  connect(ppm_diagrams, &QPushButton::clicked, this,
          [this]
            {
              PPMGraphSelectionWindow *pgs = new PPMGraphSelectionWindow(this);
              pgs->show();
            });
  v_box->addWidget(ppm_diagrams);

  QPushButton *download_data = new QPushButton;
  download_data->setText(tr("Download data"));
  connect(download_data, &QPushButton::clicked, this,
          [this]
            {
              DownloadTechnicalInfoWindow *dtiw
                  = new DownloadTechnicalInfoWindow(this);
              connect(dtiw, &DownloadTechnicalInfoWindow::signalFinished, this,
                      [this](const bool &correct)
                        {
                          if(correct)
                            {
                              DownloadWindow *dw = new DownloadWindow(this);
                              dw->show();
                            }
                        });
              dtiw->show();
            });
  v_box->addWidget(download_data);
}
