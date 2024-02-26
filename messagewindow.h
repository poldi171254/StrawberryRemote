#ifndef MESSAGEWINDOW_H
#define MESSAGEWINDOW_H

#include <QWidget>

namespace Ui {
class MessageWindow;
}

class MessageWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MessageWindow(QWidget *parent = nullptr);
    ~MessageWindow();

public slots:
    void Init();
    void DisplayText(QString);

signals:
    void Ok();

private:
    Ui::MessageWindow *ui_;
};

#endif // MESSAGEWINDOW_H
