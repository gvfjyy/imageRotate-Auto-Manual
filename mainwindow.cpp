#include "mainwindow.h"
#include "ui_mainwindow.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->pushButton_page1_1,SIGNAL(clicked()),this,SLOT(selectFiles()));
    QObject::connect(ui->pushButton_page2_1,SIGNAL(clicked()),this,SLOT(rotate_clicked()));
    QObject::connect(ui->pushButton_page3_1,SIGNAL(clicked()),this,SLOT(saveRotateFile()));
    QObject::connect(ui->tableWidget_page3_1,SIGNAL(cellClicked(int,int)),this,SLOT(redirectToRotate(int,int)));
    QObject::connect(ui->pushButton_page4_1,SIGNAL(clicked()),this,SLOT(returnToPage3()));
    QObject::connect(ui->pushButton_page4_2,SIGNAL(clicked()),this,SLOT(recover()));
    QObject::connect(ui->pushButton_page4_3,SIGNAL(clicked()),this,SLOT(saveArtRotatedImage()));
    QObject::connect(ui->pushButton1,SIGNAL(clicked()),this,SLOT(pushButton1_clicked()));
    QObject::connect(ui->pushButton2,SIGNAL(clicked()),this,SLOT(pushButton2_clicked()));
    QObject::connect(ui->pushButton3,SIGNAL(clicked()),this,SLOT(pushButton3_clicked()));
    QObject::connect(ui->pushButton4,SIGNAL(clicked()),this,SLOT(pushButton4_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::pushButton1_clicked(){
    Mat src = imread("clickedImage.jpg");
    Mat resultImage;
    rotate_arbitrarily_angle(src, resultImage, 90);
    imwrite("clickedImage.jpg", resultImage);
    updateRotate();
    return;
}

void MainWindow::pushButton2_clicked(){
    Mat src = imread("clickedImage.jpg");
    Mat resultImage;
    rotate_arbitrarily_angle(src, resultImage, 270);
    imwrite("clickedImage.jpg", resultImage);
    updateRotate();
    return;
}

void MainWindow::pushButton3_clicked(){
    Mat src = imread("clickedImage.jpg");
    Mat resultImage;
    srcSizeRotate(src, resultImage, 10);
    imwrite("clickedImage.jpg", resultImage);
    updateRotate();
    return;
}

void MainWindow::pushButton4_clicked(){
    Mat src = imread("clickedImage.jpg");
    Mat resultImage;
    srcSizeRotate(src, resultImage, 350);
    imwrite("clickedImage.jpg", resultImage);
    updateRotate();
    return;
}


void MainWindow::srcSizeRotate(const Mat &srcImage, Mat &destImage, double angle)
{
    Point2f center(srcImage.cols / 2, srcImage.rows / 2);//中心
    Mat M = getRotationMatrix2D(center, angle, 1);//计算旋转的仿射变换矩阵
    warpAffine(srcImage, destImage, M, Size(srcImage.cols, srcImage.rows));//仿射变换
    return;
}


bool MainWindow::imageAutoRotate(QString filename, QString savePath){
    //读取图像&&预处理
    Mat src = imread(filename.toStdString());
    Mat resizeImage = Mat::zeros(512, 512, CV_8UC3);
    cout<<src.size()<<endl;
    cout<<resizeImage.size()<<endl;
    if(!src.data) return false;
    cv::resize(src, resizeImage, resizeImage.size());
    Mat binaryImage, midImage,dstImage,resultImage;
    threshold(resizeImage, binaryImage, 100, 255, CV_THRESH_BINARY);//二值化，第三个参数为阈值
    Canny(binaryImage, midImage, 50, 150, 3);//输出的为灰度图
    cvtColor(midImage, dstImage, CV_GRAY2BGR);


    //通过霍夫变换检测直线
    vector<Vec2f> lines;
    HoughLines(midImage, lines, 1, CV_PI / 180, 45, 0, 0);
    if (lines.size() == 0) {
        return false;
    }

    //确定旋转角度
    vector<double> adjustAngleToLeft;
    vector<double> adjustAngleToRight;
    for (size_t i = 0; i < lines.size(); i++)
    {
        double angleWithVerticalAxisInRight = lines[i][1]* 57.29578;
        if (angleWithVerticalAxisInRight>=0 && angleWithVerticalAxisInRight<=45) {
            adjustAngleToLeft.push_back(angleWithVerticalAxisInRight);
        }
        else if (angleWithVerticalAxisInRight>45 && angleWithVerticalAxisInRight<=90) {
            adjustAngleToRight.push_back(90 - angleWithVerticalAxisInRight);
        }
        else if (angleWithVerticalAxisInRight>90 && angleWithVerticalAxisInRight<=135) {
            adjustAngleToLeft.push_back(angleWithVerticalAxisInRight - 90);
        }
        else if (angleWithVerticalAxisInRight>135 && angleWithVerticalAxisInRight<=180) {
            adjustAngleToRight.push_back(180 - angleWithVerticalAxisInRight);
        }
    }

    double finalAngle;
    double min = 360, max = 0, sum = 0;
    if (adjustAngleToLeft.size() > adjustAngleToRight.size()) {
        for (int i = 0; i < adjustAngleToLeft.size(); i++) {
            if (adjustAngleToLeft[i] < min) min = adjustAngleToLeft[i];
            if (adjustAngleToLeft[i] > max) max = adjustAngleToLeft[i];
            sum += adjustAngleToLeft[i];
        }
        if ((max - min) < 20 ) {
            finalAngle = sum / adjustAngleToLeft.size();
        }
        else finalAngle = min;
        src = imread(filename.toStdString());
        rotate_arbitrarily_angle(src, resultImage, finalAngle);
    }
    else if (adjustAngleToLeft.size() <= adjustAngleToRight.size()) {
        for (int i = 0; i < adjustAngleToRight.size(); i++) {
            if (adjustAngleToRight[i] < min) min = adjustAngleToRight[i];
            if (adjustAngleToRight[i] > max) max = adjustAngleToRight[i];
            sum += adjustAngleToRight[i];
        }
        if ((max - min) < 20) {
            finalAngle = sum / adjustAngleToRight.size();
        }
        else finalAngle = min;
        src = imread(filename.toStdString());
        rotate_arbitrarily_angle(src, resultImage, 360-finalAngle);
    }
    imwrite(savePath.toStdString(), resultImage);
    return true;
}



void MainWindow::returnToPage3(){
    ui->stackedWidget->setCurrentIndex(2);
    refreshTableWidget_page3_1();
    return;
}


void MainWindow::recover(){
    QFile::copy(this->clickedImage, "clickedImage.jpg");
    QFile::remove("clickedImage.jpg");
    QFile::copy(this->clickedImage, "clickedImage.jpg");
    cout<<"OK"<<endl;
    updateRotate();
    return;
}


void MainWindow::saveArtRotatedImage(){
    cout<<"OK"<<endl;
    QFile::copy("clickedImage.jpg",this->clickedImage);
    QFile::remove(this->clickedImage);
    QFile::copy("clickedImage.jpg",this->clickedImage);
    QMessageBox::information(NULL, "information", "save ok", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    return;
}


void MainWindow::redirectToRotate(int row,int col){
    int index=ui->tableWidget_page3_1->columnCount()*(ceil(row/2)) + col;
    this->clickedImage=this->rotateResult[index];
    QFile::copy(this->clickedImage, "clickedImage.jpg");
    QFile::remove("clickedImage.jpg");
    QFile::copy(this->clickedImage, "clickedImage.jpg");
    ui->stackedWidget->setCurrentIndex(3);
    updateRotate();
    return;
}


void MainWindow::updateRotate(){
    cout<<this->height()<<" "<<this->width()<<endl;
    QPixmap img=QPixmap("clickedImage.jpg");
    if(img.width() < (this->width()*0.5)){
        img=img.scaled(this->width()*0.5,img.height()*((this->width()*0.5)/img.width()));
    }
    if(img.height() < (this->height()*0.5)){
        img=img.scaled(img.width()*((this->height()*0.5)/img.height()),(this->height()*0.5));
    }
    ui->label_page3_1->setMinimumSize(img.width(),img.height());
    ui->label_page3_1->setMaximumSize(img.width(),img.height());
    ui->label_page3_1->setPixmap(img);
    return;
}





void MainWindow::saveRotateFile(){
    QString dstDir = QFileDialog::getExistingDirectory(
                this, "choose src Directory",
                 "/");

     if (dstDir.isEmpty())
     {
         QMessageBox::warning(NULL, "warning","choose fail",QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes);
         return;
     }
     else{
         for(int i=0;i<this->rotateResult.size();i++){
             copyFileToPath(this->rotateResult[i],dstDir+"/"+splitFileNameFromPath(this->rotateResult[i]),true);
         }
     }
     QMessageBox::warning(NULL, "note","save ok",QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes);
     return;
}

void MainWindow::rotate_clicked(){
    QString saveFolder = "D:/imageRotate/rotateResult";

    qDebug()<<saveFolder;

    //创建文件夹，已有则删除重建
    deleteDir(saveFolder);//发布时可更新为相对路径
    QDir dir;
    dir.mkpath(saveFolder);

    //图像旋转并保存至文件夹
    for(int i=0;i<this->filenames.size();i++){
        QString savePath=saveFolder;
        savePath.append("/");
        savePath.append(splitFileNameFromPath(this->filenames[i]));
        qDebug()<<savePath;
        if(imageAutoRotate(this->filenames[i],savePath)){
            this->rotateResult.append(savePath);
        }
    }

    //更新tablewidget
    ui->stackedWidget->setCurrentIndex(2);
    refreshTableWidget_page3_1();
    QMessageBox::warning(NULL, "note","finish",QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes);
    return;
}




QString MainWindow::splitFileNameFromPath(QString path){
    int indexOdDot=path.lastIndexOf("/");
    QString filename=path.mid(indexOdDot+1);
    return filename;
}
bool MainWindow::deleteDir(const QString &path)
{
    if (path.isEmpty()){
        return false;
    }
    QDir dir(path);
    if(!dir.exists()){
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList){ //遍历文件信息
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        }else{ // 递归删除
            deleteDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}

void MainWindow::selectFiles(){
    this->filenames=QFileDialog::getOpenFileNames(this,tr("文件选择"),tr("/home"),tr("图片文件(* png * jpg);;文本文件(* txt)"));
    //qDebug()<<"filenames:"<<this->filenames;
    if(this->filenames.size()==0) return;
    else{
        ui->stackedWidget->setCurrentIndex(1);
        refreshTableWidget_page2_1();
    }
    return;
}

void MainWindow::rotate_arbitrarily_angle(Mat &src, Mat &dst, float angle)
{
    float radian = (float)(angle / 180.0 * CV_PI);

    //填充图像
    int maxBorder = (int)(max(src.cols, src.rows)* 1.414); //即为sqrt(2)*max
    int dx = (maxBorder - src.cols) / 2;
    int dy = (maxBorder - src.rows) / 2;
    copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_CONSTANT);

    //旋转
    Point2f center((float)(dst.cols / 2), (float)(dst.rows / 2));
    Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0);//求得旋转矩阵
    warpAffine(dst, dst, affine_matrix, dst.size());

    //计算图像旋转之后包含图像的最大的矩形
    float sinVal = abs(sin(radian));
    float cosVal = abs(cos(radian));
    Size targetSize((int)(src.cols * cosVal + src.rows * sinVal),
        (int)(src.cols * sinVal + src.rows * cosVal));

    //剪掉多余边框
    int x = (dst.cols - targetSize.width) / 2;
    int y = (dst.rows - targetSize.height) / 2;
    Rect rect(x, y, targetSize.width, targetSize.height);
    dst = Mat(dst, rect);
}


void MainWindow::refreshTableWidget_page2_1(){
    ui->tableWidget_page2_1->clear();
    int imageSize=150;
    int num_col=int(ui->tableWidget_page2_1->width()/imageSize);
    int num_row=int(ceil(this->filenames.size()/num_col))*2;

    int height = imageSize;
    int width = int((ui->tableWidget_page2_1->width()-25)/num_col);

    ui->tableWidget_page2_1->setColumnCount(num_col);
    ui->tableWidget_page2_1->setRowCount(num_row);
    ui->tableWidget_page2_1->verticalHeader()->setVisible(false);
    ui->tableWidget_page2_1->horizontalHeader()->setVisible(false);
    ui->tableWidget_page2_1->setShowGrid(false);
    ui->tableWidget_page2_1->setIconSize(QSize(width,height));

    for(int i=0;i<num_col;i++){
        ui->tableWidget_page2_1->setColumnWidth(i,width);
    }
    for(int i=0;i<num_row;i++){
        if(i%2==0){
            ui->tableWidget_page2_1->setRowHeight(i,height);
        }
        else ui->tableWidget_page2_1->setRowHeight(i,25);
    }

    for(int k=0;k<this->filenames.size();k++){
        int i=int(floor(k/num_col))*2;
        int j=int(k%num_col);

        //加载图像
        QLabel* imageItem=new QLabel();
        QPixmap pixmap(this->filenames[k]);
        pixmap = pixmap.scaled(width-10, height-10, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
        imageItem->setPixmap(pixmap);
        imageItem->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_page2_1->setCellWidget(i,j,imageItem);

        //加载文件名
        QTableWidgetItem* textItem = new QTableWidgetItem(splitFileNameFromPath(this->filenames[k]));
        textItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_page2_1->setItem(i+1,j,textItem);
    }
    return;
}

void MainWindow::refreshTableWidget_page3_1(){
    ui->tableWidget_page3_1->clear();
    int imageSize=150;
    int num_col=int(ui->tableWidget_page3_1->width()/imageSize);
    int num_row=int(ceil(this->rotateResult.size()/num_col))*2;

    int height = imageSize;
    int width = int((ui->tableWidget_page3_1->width()-25)/num_col);

    ui->tableWidget_page3_1->setColumnCount(num_col);
    ui->tableWidget_page3_1->setRowCount(num_row);
    ui->tableWidget_page3_1->verticalHeader()->setVisible(false);
    ui->tableWidget_page3_1->horizontalHeader()->setVisible(false);
    ui->tableWidget_page3_1->setShowGrid(false);
    ui->tableWidget_page3_1->setIconSize(QSize(width,height));

    for(int i=0;i<num_col;i++){
        ui->tableWidget_page3_1->setColumnWidth(i,width);
    }
    for(int i=0;i<num_row;i++){
        if(i%2==0){
            ui->tableWidget_page3_1->setRowHeight(i,height);
        }
        else ui->tableWidget_page3_1->setRowHeight(i,25);
    }

    for(int k=0;k<this->rotateResult.size();k++){
        int i=int(floor(k/num_col))*2;
        int j=int(k%num_col);

        //加载图像
        QLabel* imageItem=new QLabel();
        QPixmap pixmap(this->rotateResult[k]);
        pixmap = pixmap.scaled(width-10, height-10, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
        imageItem->setPixmap(pixmap);
        imageItem->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_page3_1->setCellWidget(i,j,imageItem);

        //加载文件名
        QTableWidgetItem* textItem = new QTableWidgetItem(splitFileNameFromPath(this->rotateResult[k]));
        textItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_page3_1->setItem(i+1,j,textItem);
    }
    return;
}



void MainWindow::resizeEvent(QResizeEvent *size){
    if(ui->stackedWidget->currentIndex()==1){
        refreshTableWidget_page2_1();
    }
    else if(ui->stackedWidget->currentIndex()==2){
        refreshTableWidget_page3_1();
    }
    return;
}


bool MainWindow::copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\","/");
    if (sourceDir == toDir){
        return true;
    }
    if (!QFile::exists(sourceDir)){
        return false;
    }
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist){
        if(coverFileIfExist){
            createfile->remove(toDir);
        }
    }//end if

    if(!QFile::copy(sourceDir, toDir))
    {
        return false;
    }
    return true;
}
