#ifndef BBDIALOG_H
#define BBDIALOG_H

#include <QDialog>

namespace Ui {
class BBDialog;
}

class BBDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BBDialog(QWidget *parent = 0);
    ~BBDialog();
    float bbTemp ;
    bool hotFlag ;
    void setTemp(float) ;

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_bbacqAcqButton_clicked();

private:
    Ui::BBDialog *ui;
};

#endif // BBDIALOG_H
