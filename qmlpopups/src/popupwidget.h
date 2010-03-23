#ifndef WIDGET_H
#define WIDGET_H

#include <QtDeclarative/QDeclarativeView>
#include "qmlpopups.h"
#include <QVariant>
namespace QmlPopups
{
	enum PopupWidgetFlag
	{
		None = 0x0,
		Preview = 0x1,
		AeroBackground = 0x2,
		Transparent = 0x4,
		AeroBlurBehind = 0x6,
	};
	Q_DECLARE_FLAGS(PopupWidgetFlags,PopupWidgetFlag)

	class PopupWidget : public QDeclarativeView
	{
		Q_OBJECT
	public:
		explicit PopupWidget();
		virtual QSize sizeHint() const;
		void setData(const QString& title,
					const QString& body,
					QObject *sender,
					const QVariant &data); //size of textbrowser
	public slots:
		void onTimeoutReached();
		void activate();
		void ignore();
	signals:
		void sizeChanged(const QSize &size);
		void activated();
	private slots:
		void onSceneResized(QSize size);
	private:
		QSize m_size_hint;
		QObject *m_sender;
		QVariant m_data;
		int m_timer_id;
	};
}
#endif // WIDGET_H
