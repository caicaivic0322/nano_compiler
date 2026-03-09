#include "splash_dialog.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QApplication>
#include <QScreen>

SplashDialog::SplashDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("关于");
    setModal(true);

    // 设置窗口大小
    setFixedSize(300, 350);

    // 移除问号按钮
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();

    // 设置自动关闭定时器（5秒后自动关闭）
    m_autoCloseTimer = new QTimer(this);
    connect(m_autoCloseTimer, &QTimer::timeout, this, &SplashDialog::onTimeout);
    m_autoCloseTimer->start(5000);
}

SplashDialog::~SplashDialog()
{
}

void SplashDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 头像图片
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);

    // 加载图片
    QPixmap pixmap(":/images/me.jpg");
    if (!pixmap.isNull()) {
        // 缩放图片到合适大小（150x150）
        pixmap = pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_imageLabel->setPixmap(pixmap);
    } else {
        m_imageLabel->setText("[图片加载失败]");
        m_imageLabel->setStyleSheet("color: red;");
    }

    mainLayout->addWidget(m_imageLabel, 0, Qt::AlignCenter);

    // 文字标签
    m_textLabel = new QLabel("开发者：蔡老师", this);
    m_textLabel->setAlignment(Qt::AlignCenter);
    m_textLabel->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
        "color: #333333;"
        "margin-top: 10px;"
    );
    mainLayout->addWidget(m_textLabel);

    // 添加一些间距
    mainLayout->addStretch();

    // 关闭按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_closeButton = new QPushButton("确定", this);
    m_closeButton->setFixedWidth(80);
    m_closeButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}"
    );
    connect(m_closeButton, &QPushButton::clicked, this, &SplashDialog::onCloseClicked);
    buttonLayout->addWidget(m_closeButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    // 设置窗口居中显示
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

void SplashDialog::onCloseClicked()
{
    accept();
}

void SplashDialog::onTimeout()
{
    accept();
}
