#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include <stdio.h>
#include <stdlib.h>
#include "widgetviewimage.h"
#include "widgetviewfall.h"
#include "widgetviewplot.h"
#include "widgetviewplotfft.h"
#include "widgetviewplotpixel.h"
#include "tprocess.h"
#include "mainprocess.h"
//#include "cssl_lib.h"
#include "InoSettings.h"

#ifdef __unix__
#include "tircis_proc_start.h"
//#include <unistd.h> //for usleep
#endif

/*#ifdef _WIN32
//#include "windows.h"
#define uint8_t unsigned char
#endif*/


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
#ifdef __unix__
    friend void* runAcquisition(void *args);
    friend void* runWaterfall(void *args);
#endif
    //void FindDevices(int *RotationAPortNum, int *BbPortNum);
    //void SaveHeader(void);

    long size;
    QTimer TimerVideo;
    QTimer TimerProgress;
    QTimer TimerLogEntry;
    QTimer TimerBB;
    QTimer TimerRotationA;
    QTimer TimerFocus;
    //bool m_bUseBBUnit2;
    bool m_bAllPositionsFlagOn;
    TProcess *TProc ;
    void enableButtons(bool ButtonState);
    MainProcess *mainProc;
    QString ScanFile ;

private:
    Ui::MainWindow *ui;
    InoSettings *InoSettingsDialog;
    uint8_t CurrentTab;
    bool readProcessFile () ;


private slots:
#define LOLO
#ifdef LOLO
    void onTimer();
    void onProgress();
    void onLogEntry();
    //void MonitorTemperatureBbUnit1();
    void MonitorPositionA();
    //void MonitorFocusPosition();
    void ShutDown();
    void on_pushButtonExit_clicked();
    //void on_pushButtonSetCold_clicked();
    //void on_pushButtonSetHot_clicked();
    //void on_pushButtonSetRotationAPosition_clicked();
    //void on_pushButtonReopenSerialPorts_clicked();
    //void on_pushButtonCollectScan_clicked();
    //void on_pushButtonWaveCal_clicked();
    //void on_pushButtonViewRange_clicked();
    //void on_pushButtonMoveHome_clicked();
    //void on_pushButtonMoveBB_clicked();
    //void on_pushButtonSaveCold_clicked();
    //void on_pushButtonSaveHot_clicked();
    void on_pushButtonAbort_clicked();
    void on_pushButtonMagnifyBench_clicked();
    void on_pushButton_FFC_clicked();
    void on_tabWidget_currentChanged(int index);

    void on_pushButtonAcquireManualBench_clicked();
    void on_pushButtonAutoAcquireBench_clicked();
    void on_pushButtonWaveCalBench_clicked();

    void on_pushButtonAutoAcquireTripod_clicked();
    void on_pushButtonWaveCalTripod_clicked();

    void on_pushButtonAutoAcquireFlight_clicked();
    void on_pushButtonWaveCalFlight_clicked();


    void on_pushButtonSavePaddleColdBench_clicked();
    void on_pushButtonSavePaddleColdTripod_clicked();
    void on_pushButtonSavePaddleColdFlight_clicked();
    void on_pushButtonSavePaddleHotBench_clicked();
    void on_pushButtonSavePaddleHotTripod_clicked();
    void on_pushButtonSavePaddleHotFlight_clicked();
    void on_pushButtonSaveRotorColdBench_clicked();
    void on_pushButtonSaveRotorColdTripod_clicked();
    void on_pushButtonSaveRotorColdFlight_clicked();
    void on_pushButtonSaveRotorHotBench_clicked();
    void on_pushButtonSaveRotorHotTripod_clicked();
    void on_pushButtonSaveRotorHotFlight_clicked();
    void on_pushButtonSaveBbExtBench_clicked();


    void on_pushButtonSetPaddleColdBench_clicked();
    void on_pushButtonSetPaddleColdTripod_clicked();
    void on_pushButtonSetRotorColdBench_clicked();
    void on_pushButtonSetRotorColdTripod_clicked();
    void on_pushButtonSetExtBench_clicked();

    void on_pushButtonSetPaddleHotBench_clicked();
    void on_pushButtonSetPaddleHotTripod_clicked();
    void on_pushButtonSetRotorHotBench_clicked();
    void on_pushButtonSetRotorHotTripod_clicked();
    //void on_pushButtonSetExtHotBench_clicked();

    void on_pushButtonSetPaddleColdFlight_clicked();
    void on_pushButtonSetPaddleHotFlight_clicked();
    void on_pushButtonSetRotorColdFlight_clicked();
    void on_pushButtonSetRotorHotFlight_clicked();

    void on_pushButtonHomeRotationBench_clicked();
    void on_pushButtonMoveRotationBench_clicked();
    void on_pushButtonHomeGoniometerBench_clicked();
    void on_pushButtonMoveGoniometerBench_clicked();

    void on_pushButtonHomeRotationTripod_clicked();
    void on_pushButtonMoveRotationTripod_clicked();
    void on_pushButtonHomeGoniometerTripod_clicked();
    void on_pushButtonMoveGoniometerTripod_clicked();


    void on_radioButtonWave8Tripod_clicked();
    void on_radioButtonWave9Tripod_clicked();
    void on_radioButtonWave10Tripod_clicked();

    void on_radioButtonWave8Flight_clicked();
    void on_radioButtonWave9Flight_clicked();
    void on_radioButtonWave10Flight_clicked();

    void on_radioButtonPaddleColdT30_clicked();
    void on_radioButtonPaddleColdT35_clicked();
    void on_radioButtonPaddleColdT40_clicked();
    void on_radioButtonPaddleColdT45_clicked();
    void on_radioButtonPaddleColdT50_clicked();

    void on_radioButtonPaddleHotT30_clicked();
    void on_radioButtonPaddleHotT35_clicked();
    void on_radioButtonPaddleHotT40_clicked();
    void on_radioButtonPaddleHotT45_clicked();
    void on_radioButtonPaddleHotT50_clicked();

    void on_radioButtonRotorColdT30_clicked();
    void on_radioButtonRotorColdT35_clicked();
    void on_radioButtonRotorColdT40_clicked();
    void on_radioButtonRotorColdT45_clicked();
    void on_radioButtonRotorColdT50_clicked();

    void on_radioButtonRotorHotT30_clicked();
    void on_radioButtonRotorHotT35_clicked();
    void on_radioButtonRotorHotT40_clicked();
    void on_radioButtonRotorHotT45_clicked();
    void on_radioButtonRotorHotT50_clicked();
    void on_pushButtonProcessData_clicked();

    void on_radioButtonPaddleColdBench_clicked();
    void on_radioButtonPaddleHotBench_clicked();
    void on_radioButtonPaddleAmbientBench_clicked();
    void on_radioButtonPaddleClearBench_clicked();
    void on_radioButtonRotorColdBench_clicked();
    void on_radioButtonRotorHotBench_clicked();
    void on_radioButtonRotorAmbientBench_clicked();
    void on_radioButtonRotorClearBench_clicked();
    void on_radioButtonPaddleColdTripod_clicked();
    void on_radioButtonPaddleHotTripod_clicked();
    void on_radioButtonPaddleAmbientTripod_clicked();
    void on_radioButtonPaddleClearTripod_clicked();
    void on_radioButtonRotorColdTripod_clicked();
    void on_radioButtonRotorHotTripod_clicked();
    void on_radioButtonRotorAmbientTripod_clicked();
    void on_radioButtonRotorClearTripod_clicked();
    void on_radioButtonPaddleColdFlight_clicked();
    void on_radioButtonPaddleHotFlight_clicked();
    void on_radioButtonPaddleAmbientFlight_clicked();
    void on_radioButtonPaddleClearFlight_clicked();
    void on_radioButtonRotorColdFlight_clicked();
    void on_radioButtonRotorHotFlight_clicked();
    void on_radioButtonRotorAmbientFlight_clicked();
    void on_radioButtonRotorClearFlight_clicked();
    void on_pushButton_RunScript_clicked();
    void on_radioButtonPaddleColdOnBench_clicked();
    void on_radioButtonPaddleColdOffBench_clicked();
    void on_radioButtonRotorColdOnBench_clicked();
    void on_radioButtonRotorColdOffBench_clicked();
    //void on_pushButton_GetTemperature_clicked();
    void on_radioButtonPaddleHotOnBench_clicked();
    void on_radioButtonPaddleHotOffBench_clicked();
    void on_radioButtonRotorHotOnBench_clicked();
    void on_radioButtonRotorHotOffBench_clicked();
    void on_pushButtonProcessDataBench_clicked();
    void on_lineEditWavelengthBench_editingFinished();

#endif
    void on_pushButtonSaveBbExt2Bench_clicked();
    void on_pushButtonSetExt2Bench_clicked();
    void on_pushButtonGroundAlign_clicked();
    void on_pushButtonAirAlign_clicked();
    void on_pushButtonAcquireManualStopBench_clicked();
    void on_pushButton_ShowRaw_clicked();
    void on_radioButtonImageAutoBench_toggled(bool checked);
    void on_radioButtonFftAutoBench_toggled(bool checked);
    void on_radioButtonPixelAutoBench_toggled(bool checked);
    void on_pushButtonCamerasettings_clicked();
    void on_pushButtonScanStage_clicked();
    void on_pushButtonAcquireManualFlight_clicked();
    void on_pushButtonAcquireManualStopFlight_clicked();
    void on_checkBox_clicked();
};

#endif // MAINWINDOW_H
