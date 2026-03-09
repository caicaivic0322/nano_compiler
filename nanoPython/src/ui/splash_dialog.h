#ifndef SPLASH_DIALOG_H
#define SPLASH_DIALOG_H

#include <QDialog>
#include <QTimer>

class QLabel;
class QPushButton;

class SplashDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SplashDialog(QWidget *parent = nullptr);
    ~SplashDialog();

private slots:
    void onCloseClicked();
    void onTimeout();

private:
    void setupUI();

    QLabel *m_imageLabel;
    QLabel *m_textLabel;
    QPushButton *m_closeButton;
    QTimer *m_autoCloseTimer;
};

#endif // SPLASH_DIALOG_H
