#ifndef E57INSPECTOR_WELCOME_H
#define E57INSPECTOR_WELCOME_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Welcome;
}
QT_END_NAMESPACE

class Welcome : public QWidget
{
    Q_OBJECT

public:
    explicit Welcome(QWidget* parent = nullptr);
    ~Welcome() override;

private:
    Ui::Welcome* ui;
};

#endif // E57INSPECTOR_WELCOME_H
