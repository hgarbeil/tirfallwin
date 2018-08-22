#include "InoSettings.h"
#include "ui_InoSettings.h"
#include "MICROXCAM-384I_CAPI.h"

bool m_bInitialized = false;
int iValVRES_HIGH, iValVCSH, iValVCSL, iValVREF, iValVCM1, iValVCM2, iValVCM3;


InoSettings::InoSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InoSettings)
{
    ui->setupUi(this);
}

InoSettings::~InoSettings()
{
    delete ui;
}

void InoSettings::on_pushButtonOK_clicked()
{
    hide();
}

void InoSettings::on_pushButtonLoadRpl_clicked()
{
    char RplFile[128];

    strcpy(RplFile, "C:\\Program Files (x86)\\INO\\MICROXCAM-384I\\!RPL\\RPL MXC-002 10-65deg High Dynamic Range 2017-05-02.rpl");
    int iRet = fn_LoadRPLFile (RplFile);
}

void InoSettings::on_pushButtonFpaTemperatureGet_clicked()
{
    double dVal;
    char sVal[32];

    fn_GetFPATemp(dVal);
    sprintf(sVal, "%.2f", dVal);
    ui->lineEditFpaTemperature->setText(sVal);
}

void InoSettings::on_pushButtonComputeOffset_clicked()
{
    int iNbrFrame = 16;
    bool bUseInternalShutter = false;
    bool bVerbose = true;

    int iRet = fn_TakeOffset (iNbrFrame , bUseInternalShutter, bVerbose);
}

void InoSettings::on_pushButtonLsyncPwGet_clicked()
{
    int iVal;
    char sVal[32];
    int iRet = fn_GetLsyncPw(iVal);
    sprintf(sVal, "%d", iVal);
    ui->lineEditLsyncPw->setText(sVal);
}

void InoSettings::on_pushButtonLsyncPwSet_clicked()
{
}

void InoSettings::on_pushButtonShcdsPwGet_clicked()
{
    int iValPW, iValStart;
    char sValPW[32], sValStart[32];

    int iRet = fn_GetSHCDSPWAndStart(iValPW,iValStart);
    sprintf(sValPW, "%d", iValPW);
    ui->lineEditShcdsPw->setText(sValPW);
    sprintf(sValStart, "%d", iValStart);
    ui->lineEditShcdsStart->setText(sValStart);
}

void InoSettings::on_pushButtonShcdsPwSet_clicked()
{

}

void InoSettings::on_pushButtonShvidPwGet_clicked()
{
    int iValPW, iValStart;
    char sValPW[32], sValStart[32];

    int iRet = fn_GetSHVIDPWAndStart(iValPW,iValStart);
    sprintf(sValPW, "%d", iValPW);
    ui->lineEditShvidPw->setText(sValPW);
    sprintf(sValStart, "%d", iValStart);
    ui->lineEditShvidStart->setText(sValStart);
}

void InoSettings::on_pushButtonShvidPwSet_clicked()
{
}

void InoSettings::on_pushButtonFpaGet_clicked()
{
    char sValVRES_HIGH[32], sValVCSH[32], sValVCSL[32], sValVREF[32], sValVCM1[32], sValVCM2[32], sValVCM3[32];

    fn_GetFPABias(iValVRES_HIGH, iValVCSH, iValVCSL, iValVREF, iValVCM1, iValVCM2, iValVCM3);
    \
    sprintf(sValVRES_HIGH, "%d", iValVRES_HIGH);
    ui->lineEditVresHigh->setText(sValVRES_HIGH);
    sprintf(sValVCSH, "%d", iValVCSH);
    ui->lineEditVcsh->setText(sValVCSH);
    sprintf(sValVCSL, "%d", iValVCSL);
    ui->lineEditVcsl->setText(sValVCSL);
    sprintf(sValVREF, "%d", iValVREF);
    ui->lineEditVref->setText(sValVREF);
    sprintf(sValVCM1, "%d", iValVCM1);
    ui->lineEditVcm1->setText(sValVCM1);
    sprintf(sValVCM2, "%d", iValVCM2);
    ui->lineEditVcm2->setText(sValVCM2);
    sprintf(sValVCM3, "%d", iValVCM3);
    ui->lineEditVcm3->setText(sValVCM3);
}

void InoSettings::on_pushButtonFpaSet_clicked()
{    
    int iValVRES_HIGHRet, iValVCSHRet, iValVCSLRet, iValVREFRet, iValVCM1Ret, iValVCM2Ret, iValVCM3Ret;
    char sValVRES_HIGH[32], sValVCSH[32], sValVCSL[32], sValVREF[32], sValVCM1[32], sValVCM2[32], sValVCM3[32];

    // Get values from text boxes
    iValVRES_HIGH = ui->lineEditVresHigh->text().toInt();
    iValVCSH = ui->lineEditVcsh->text().toInt();
    iValVCSL = ui->lineEditVcsl->text().toInt();
    iValVREF = ui->lineEditVref->text().toInt();
    iValVCM1 = ui->lineEditVcm1->text().toInt();
    iValVCM2 = ui->lineEditVcm2->text().toInt();
    iValVCM3 = ui->lineEditVcm3->text().toInt();

    fn_SetFPABias(iValVRES_HIGH, iValVCSH, iValVCSL, iValVREF, iValVCM1, iValVCM2, iValVCM3,
                  iValVRES_HIGHRet, iValVCSHRet, iValVCSLRet, iValVREFRet, iValVCM1Ret, iValVCM2Ret, iValVCM3Ret);

    sprintf(sValVRES_HIGH, "%d", iValVRES_HIGHRet);
    ui->lineEditVresHigh->setText(sValVRES_HIGH);
    sprintf(sValVCSH, "%d", iValVCSHRet);
    ui->lineEditVcsh->setText(sValVCSH);
    sprintf(sValVCSL, "%d", iValVCSLRet);
    ui->lineEditVcsl->setText(sValVCSL);
    sprintf(sValVREF, "%d", iValVREFRet);
    ui->lineEditVref->setText(sValVREF);
    sprintf(sValVCM1, "%d", iValVCM1Ret);
    ui->lineEditVcm1->setText(sValVCM1);
    sprintf(sValVCM2, "%d", iValVCM2Ret);
    ui->lineEditVcm2->setText(sValVCM2);
    sprintf(sValVCM3, "%d", iValVCM3Ret);
    ui->lineEditVcm3->setText(sValVCM3);
}

void InoSettings::ShowStats()
{
    char sValVCSL[32];

    on_pushButtonFpaTemperatureGet_clicked();
    on_pushButtonLsyncPwGet_clicked();
    on_pushButtonShcdsPwGet_clicked();
    on_pushButtonShvidPwGet_clicked();
    on_pushButtonFpaGet_clicked();

//    iValVCSL = 1798;
//    sprintf(sValVCSL, "%d", iValVCSL);
//    ui->lineEditVcsl->setText(sValVCSL);

//    on_pushButtonFpaSet_clicked();
}
