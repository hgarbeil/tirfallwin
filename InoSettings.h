#ifndef INOSETTINGS_H
#define INOSETTINGS_H

#include <QDialog>

namespace Ui {
class InoSettings;
}

class InoSettings : public QDialog
{
    Q_OBJECT

public:
    explicit InoSettings(QWidget *parent = 0);
    ~InoSettings();\
    void ShowStats();

private slots:
    void on_pushButtonOK_clicked();
    void on_pushButtonComputeOffset_clicked();
    void on_pushButtonLoadRpl_clicked();
    void on_pushButtonLsyncPwGet_clicked();
    void on_pushButtonLsyncPwSet_clicked();
    void on_pushButtonShcdsPwGet_clicked();
    void on_pushButtonShcdsPwSet_clicked();
    void on_pushButtonShvidPwGet_clicked();
    void on_pushButtonShvidPwSet_clicked();
    void on_pushButtonFpaGet_clicked();
    void on_pushButtonFpaSet_clicked();


    void on_pushButtonFpaTemperatureGet_clicked();

private:
    Ui::InoSettings *ui;
};

#endif // INOSETTINGS_H
