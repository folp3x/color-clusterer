#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QGraphicsScene>
#include <QList>

#include "color_spaces/lab.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void onBtnChooseImgClicked();

private:
  Ui::MainWindow *ui;

  static inline const QString ExampleImgFilePath = ":/examples/example.jpg";

  static const size_t MaxPaletteLayoutCols = 6;

  QGraphicsScene *m_scene = nullptr;

  static QList<Lab> getPixels(const QImage &img);

  void showPallete(const QList<Lab> &palette, const QList<size_t> &occurrences,
                   size_t pixelCount);
  void showImage(const QImage &img);
  void loadExample();
  void clearPaletteLayout();
};
#endif // MAINWINDOW_H
