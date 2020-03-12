#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QtCore"
#include <QMainWindow>
#include "QFileDialog"
#include "QDebug"
#include "QMessageBox"
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;


public Q_SLOTS:
    void selectFiles();
    void rotate_clicked();
    void saveRotateFile();
    void redirectToRotate(int row,int col);
    void returnToPage3();
    void saveArtRotatedImage();
    void recover();
    void pushButton1_clicked();
    void pushButton2_clicked();
    void pushButton3_clicked();
    void pushButton4_clicked();


public:
    void updateRotate();
    void refreshTableWidget_page2_1();
    void refreshTableWidget_page3_1();
    bool copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist);
    void resizeEvent(QResizeEvent* size);
    bool imageAutoRotate(QString filename, QString savePath);
    void rotate_arbitrarily_angle(Mat &src, Mat &dst, float angle);
    QString splitFileNameFromPath(QString path);
    bool deleteDir(const QString &path);
    void srcSizeRotate(const Mat &srcImage, Mat &destImage, double angle);


private:
    QStringList filenames;
    QStringList rotateResult;
    QString clickedImage;



};
#endif // MAINWINDOW_H
