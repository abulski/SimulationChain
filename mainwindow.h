/*! \class MainWindow
* This class allows to create the object that is the main GUI window.
* Using methods AddPointTo* it enables adding points to displayed plots.
* \note
* Uses QT library with the QCustomPlot external widget for plot display.
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/foreach.hpp"

#include <QMainWindow>
#include "ui_mainwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /// \brief Reset x axises to start plotting from x1=0 and x2=0.
    void ResetXAxis();

public Q_SLOTS:
    /// \brief Adds a point into first plot, first graph (x is set to x+interval)
    /// \param[in] y Sample to display.
    void AddPointToOutputSignal(double y);

    /// \brief Adds a point into first plot, second graph (x is set to x+interval)
    /// \param[in] y Sample to display.
    void AddPointToGeneratorSignal(double y);

    /// \brief Adds a point into second plot, first graph (x is set to x+interval)
    /// \param[in] y Sample to display.
    void AddPointToControlSignal(double y); ///specifies x to be x+1

    /// \brief Displays theta value
    /// \param[in] str Nominator and denominator in the form of string.
    void DisplayTheta(QString str);

    /// \brief Creates a root for simulation tree view
    /// @param[in] sName - name of the node
    /// @param[in] sType - value of the node
    /// @param[in] treeNumber - number of widget (0 - simulation tree, 1 - property tree)
    void AddTreeWidgetRoot(const QString &, const QString &, const int);

    /// \brief Adds element to the simulation tree view
    /// @param[in] sParentName - name of the parent node
    /// @param[in] sName - name of the node
    /// @param[in] sType - value of the node
    /// @param[in] treeNumber - number of widget (0 - simulation tree, 1 - property tree)
    void AddTreeWidgetElement(const QString &,const QString &,const QString &, const int);

    /// \brief Clears a given tree widget
    /// @param[in] treeNumber - number of widget (0 - simulation tree, 1 - property tree)s
    void ClearTreeWidget(const int treeNumber);

private Q_SLOTS:
    void on_startButton_clicked();

    void on_saveButton_clicked();///Slot of the button that allows to
                                 ///save the object parameters to a file.

    void on_loadButton_clicked();///Slot of the button that allows to
                                 ///load the object parameters from a file.

    void on_resetButton_clicked();

    void on_timeSpinBox_editingFinished();

    void on_periodSpinBox_editingFinished();

    void on_nomDegSpinBox_editingFinished();

    void on_denomDegSpinBox_editingFinished();

    void on_delayBoxSpinBox_editingFinished();

    void on_histLenSpinBox_editingFinished();

    void on_actionLoad_Parameters_triggered();

    void on_actionSave_Parameters_triggered();

    void on_simulationTree_itemSelectionChanged();

    void on_propertyTree_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_propertyTree_itemChanged(QTreeWidgetItem *item, int column);

private:
    ///"AddPoint()" allows to add points that are
    ///specified in the double bin box to the plot
    void addPoint1();
    ///allows to add any point
    ///specified by the "x" and
    /// "y" variables to the plot.
    void addPoint1(double x, double y, int gr);

    void addPoint2();
    void addPoint2(double x, double y);

    /// sends parameter values from ui controls to logic singleton
    void changeIdentificationParams();

    Ui::MainWindow *ui;
    double x1a;
    double x1b;
    double x2;
    int m_nInterval;
    bool m_bSootheChangeSignal;
};

#endif // MAINWINDOW_H
