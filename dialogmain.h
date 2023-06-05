#ifndef DIALOGMAIN_H
#define DIALOGMAIN_H

#include <QDialog>

namespace Ui {
class Dialogmain;
}

class Dialogmain : public QDialog
{
    Q_OBJECT

public:
    explicit Dialogmain(QWidget *parent = nullptr);
    ~Dialogmain();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialogmain *ui;
};

#endif // DIALOGMAIN_H
