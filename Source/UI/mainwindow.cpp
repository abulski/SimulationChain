#include "mainwindow.h"
#include "SLogic.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), x1a(0), x1b(0), x2(0)
{
    ui->setupUi(this);
    // ADD FIRST GRAPH INTO FIRST PLOT
    ui->customPlot->addGraph();
    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(-0.01, 1);
    ui->customPlot->yAxis->setRange(-0.01, 1.1);
    ui->customPlot->replot();

    // ADD SECOND GRAPH INTO FIRST PLOT
    ui->customPlot->addGraph();
    QColor c(200,0,0);
    QPen *p = new QPen(c);
    ui->customPlot->graph(1)->setPen(*p);

    ui->customPlot_2->addGraph();
    // give the axes some labels:
    ui->customPlot_2->xAxis->setLabel("x");
    ui->customPlot_2->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    ui->customPlot_2->xAxis->setRange(-0.01, 1);
    ui->customPlot_2->yAxis->setRange(-0.01, 1.1);
    ui->customPlot_2->replot();
    // set a cool color
    QColor c2(100,200,100);
    QPen *p2 = new QPen(c2);
    ui->customPlot_2->graph(0)->setPen(*p2);

    ///temp
    m_nInterval = 10;

    SLogic::GetInstance().SetGUIInstance(this);
}


MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::addPoint1(){
    /*
    ui->customPlot->graph(0)->addData(ui->Xcoordinate->value(),ui->Ycoordinate->value());
    ui->customPlot->graph(0)->rescaleAxes(1,0);
    ui->customPlot->replot();
    */
}

void MainWindow::addPoint1(double x, double y, int gr){
    ui->customPlot->graph(gr)->addData(x,y);
    //ui->customPlot->graph(gr)->rescaleAxes(1,1);
    ui->customPlot->graph(gr)->rescaleAxes(1,1);
    ui->customPlot->xAxis->setRange(x1a-1.0, x1a);
    ui->customPlot->replot();
}

void MainWindow::AddPointToOutputSignal(double y)
{
    this->addPoint1(x1a,y,0);
    x1a+=(ui->periodSpinBox->value()/1000.0);
}

void MainWindow::AddPointToGeneratorSignal(double y)
{
    this->addPoint1(x1b,y,1);
    x1b+=(ui->periodSpinBox->value()/1000.0);
}


void MainWindow::addPoint2(){
    /*
    ui->customPlot_2->graph(0)->addData(ui->Xcoordinate->value(),ui->Ycoordinate->value());
    ui->customPlot_2->graph(0)->rescaleAxes(1,0);
    ui->customPlot_2->replot();
    */
}
void MainWindow::addPoint2(double x, double y){
    ui->customPlot_2->graph(0)->addData(x,y);
    //ui->customPlot_2->graph(0)->rescaleAxes(1,0);
    ui->customPlot_2->graph(0)->rescaleAxes(0,1);
    ui->customPlot_2->xAxis->setRange(x2-1.0, x2);
    ui->customPlot_2->replot();
}

void MainWindow::AddPointToControlSignal(double y)
{
    this->addPoint2(x2,y);
    x2+=(ui->periodSpinBox->value()/1000.0);
}

void MainWindow::DisplayTheta(QString str)
{
    ui->thetaTextEdit->setText(str);
}

void MainWindow::ResetXAxis()
{
    x1a = 0;
    x1b = 0;
    x2 = 0;
}

void MainWindow::on_startButton_clicked()
{
    static bool isStart = true;

    if(isStart)
        ui->startButton->setText(QString("Stop"));
    else
        ui->startButton->setText(QString("Start"));

    isStart = !isStart;

    SLogic::GetInstance().ToggleSimulation();
}

void MainWindow::on_saveButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),"C://","All Files (*.*);;Text files (*.txt)");
    SLogic::GetInstance().SaveSimChain(fileName.toStdString());
}

void MainWindow::on_loadButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"C://","All Files (*.*);;Text files (*.txt)");
    SLogic::GetInstance().LoadSimChain(fileName.toStdString());
    on_resetButton_clicked();
}


void MainWindow::on_resetButton_clicked()
{
    //ui->simulationTree->clear();

    // clear the plots
    ui->customPlot->graph(0)->clearData();
    ui->customPlot->graph(1)->clearData();
    ui->customPlot_2->graph(0)->clearData();
    ui->customPlot->xAxis->setRange(-0.01, 1);
    ui->customPlot->yAxis->setRange(-0.01, 1.1);
    ui->customPlot_2->xAxis->setRange(-0.01, 1);
    ui->customPlot_2->yAxis->setRange(-0.01, 1.1);
    ui->customPlot->replot();
    ui->customPlot_2->replot();

    // reset axes
    ResetXAxis();

    // reset the simulation
    SLogic::GetInstance().ResetSimulation();
}

void MainWindow::on_timeSpinBox_editingFinished()
{
    int nTime = ui->timeSpinBox->value();
    int nPeriod = ui->periodSpinBox->value();
    SLogic::GetInstance().SetSimulationParameters(nTime, nPeriod);
}

void MainWindow::on_periodSpinBox_editingFinished()
{
    int nTime = ui->timeSpinBox->value();
    int nPeriod = ui->periodSpinBox->value();
    SLogic::GetInstance().SetSimulationParameters(nTime, nPeriod);
}

void MainWindow::changeIdentificationParams()
{
    SLogic::GetInstance().ChangeIdentificationParams(
                ui->nomDegSpinBox->value(),
                ui->denomDegSpinBox->value(),
                ui->delayBoxSpinBox->value(),
                ui->histLenSpinBox->value(),
                ui->forgettingFactorSpinBox->value());
}

void MainWindow::on_nomDegSpinBox_editingFinished()
{
    changeIdentificationParams();
}

void MainWindow::on_denomDegSpinBox_editingFinished()
{
    changeIdentificationParams();
}

void MainWindow::on_delayBoxSpinBox_editingFinished()
{
    changeIdentificationParams();
}

void MainWindow::on_histLenSpinBox_editingFinished()
{
    changeIdentificationParams();
}

void MainWindow::on_actionLoad_Parameters_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"....//SimplePlot","All Files (*.*);;Text files (*.txt)");
    SLogic::GetInstance().LoadSimChain(fileName.toStdString());
    on_resetButton_clicked();
}

void MainWindow::on_actionSave_Parameters_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),"C://","All Files (*.*);;Text files (*.txt)");
    SLogic::GetInstance().SaveSimChain(fileName.toStdString());
}

void MainWindow::on_simulationTree_itemSelectionChanged()
{
    QList<QTreeWidgetItem*> selectedItems = ui->simulationTree->selectedItems();
    if(!selectedItems.count())
        return;

    QString qObjName = selectedItems[0]->text(0);
    std::string objName = qObjName.toStdString();

    // report selection changes
    SLogic::GetInstance().ObjectFocusChange(objName);
}

void MainWindow::AddTreeWidgetRoot(const QString& sName, const QString& sType, const int treeNumber)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,sName);
    item->setText(1,sType);
    if(treeNumber == 0)
    {
        ui->simulationTree->clear();
        ui->simulationTree->addTopLevelItem(item);
    }
    else
    {
        ui->propertyTree->clear();
        ui->propertyTree->addTopLevelItem(item);
    }
}

void MainWindow::AddTreeWidgetElement(const QString &sParent, const QString &sName, const QString &sType, const int treeNumber)
{
    QList<QTreeWidgetItem*> parentNode;
    m_bSootheChangeSignal = true;
    if(treeNumber == 0)
    {
        parentNode = ui->simulationTree->findItems(sParent, Qt::MatchExactly | Qt::MatchRecursive,0);
    }
    else
    {
        parentNode = ui->propertyTree->findItems(sParent, Qt::MatchExactly | Qt::MatchRecursive,0);
    }

    if(!parentNode.count())
        return;

    QTreeWidgetItem* item = new QTreeWidgetItem(parentNode[0]);
    item->setText(0,sName);
    item->setText(1,sType);

    if(treeNumber == 0)
    {
        ui->simulationTree->expandAll();
    }
    else
    {
        ui->propertyTree->expandAll();
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
    }
    m_bSootheChangeSignal = false;
}

void MainWindow::ClearTreeWidget(const int treeNumber)
{
    if(treeNumber == 0)
        ui->simulationTree->clear();
    else
    {
        ui->propertyTree->clear();   
    }
}

void MainWindow::on_propertyTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(column == 1)
        ui->propertyTree->editItem(item, column);
}

void MainWindow::on_propertyTree_itemChanged(QTreeWidgetItem *item, int column)
{
    if(m_bSootheChangeSignal)
        return;
    //std::cout << item->text(0).toStdString() << ": " << item->text(1).toStdString() << std::endl;

    SLogic::GetInstance().SelectedObjectEdited(item->parent()->text(0).toStdString(),item->text(0).toStdString(), item->text(1).toStdString());
}
