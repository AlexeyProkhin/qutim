#include "kineticpopupwidget.h"
#include <QVBoxLayout>
#include "kineticpopupsmanager.h"
#include <QMouseEvent>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QDesktopWidget>

KineticPopupWidget::KineticPopupWidget ( const QString &styleSheet, const QString &content )
{
    setTheme ( styleSheet,content );
    //init browser
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    this->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff);
    this->setFrameShape ( QFrame::NoFrame );
    this->setWindowFlags(KineticPopupsManager::self()->widgetFlags);
    //this->resize(NotificationsManager::self()->defaultSize);

    //init transparent
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setAttribute(Qt::WA_NoSystemBackground, false);
    this->ensurePolished(); // workaround Oxygen filling the background
    this->setAttribute(Qt::WA_StyledBackground, false);
}


QSize KineticPopupWidget::setData ( const QString& title, const QString& body, const QString& imagePath )
{
    QString data = content;
    data.replace ( "{title}", title );
    data.replace ( "{body}", body );
    data.replace ( "{imagepath}",Qt::escape ( imagePath ) );
    this->document()->setHtml(data);
    this->document()->setTextWidth(KineticPopupsManager::self()->defaultSize.width());
    int width = KineticPopupsManager::self()->defaultSize.width();
    int height = this->document()->size().height();

    return QSize(width,height);


}


void KineticPopupWidget::setTheme ( const QString& styleSheet, const QString& content )
{
    this->content = content;
    this->setStyleSheet ( styleSheet );
}


void KineticPopupWidget::mouseReleaseEvent ( QMouseEvent* ev )
{
    if (ev->button() == KineticPopupsManager::self()->action1Trigger) {
        emit action1Activated();
    }
    else if (ev->button() == KineticPopupsManager::self()->action2Trigger)
        emit action2Activated();
    else
        return;
    disconnect(SIGNAL(action1Activated()));
    disconnect(SIGNAL(action2Activated()));
}
