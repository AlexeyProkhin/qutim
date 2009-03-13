#include "plugman.h"
#include <QFileDialog>
#include <QDebug>
#include <QAction>

bool plugMan::init ( PluginSystemInterface *plugin_system )
{
    qRegisterMetaType<TreeModelItem> ( "TreeModelItem" );

    PluginInterface::init ( plugin_system );
    plug_icon = new QIcon ( ":/icons/plugin.png" );
    download_icon = QIcon ( ":/icons/open.png");
    m_plugin_system = plugin_system;
    TreeModelItem contact;
    plug_action = new QAction(download_icon,tr("Install package from file"),this);
    m_plugin_system->registerMainMenuAction(plug_action);
    connect(plug_action, SIGNAL(triggered()), this, SLOT(on_installfromfileBtn_clicked()));

    return true;
}

void plugMan::release()
{
//	x3 =)
}

void plugMan::processEvent ( PluginEvent  &event)
{
	eventitem = *(TreeModelItem*)(event.args.at(0));
}

QWidget *plugMan::settingsWidget()
{
	settingswidget = new plugmanSettings(m_profile_name);
	connect(settingswidget, SIGNAL(installfromfileclick()), this,  SLOT(on_installfromfileBtn_clicked()));
    return settingswidget;
}

void plugMan::setProfileName ( const QString &profile_name )
{
    m_profile_name = profile_name;
}

QString plugMan::name()
{
    return "Plugin's manager";
}

QString plugMan::description()
{
    return "x3 (=";
}

QIcon *plugMan::icon()
{
    return plug_icon;
}

QString plugMan::type()
{
    return "demo";
}


void plugMan::removeSettingsWidget()
{
    //X3
}

void plugMan::saveSettings()
{
    //X3
}

void plugMan::on_installfromfileBtn_clicked()
{
    QString path = QFileDialog::getOpenFileName(0,tr("Install package from file"),".",
                   tr("Archives (*.zip);;XML files (*.xml)"));
	plugInstaller *plug_install = new plugInstaller;
	plug_install->setParent(this);
    if ((path.section(".",-1))=="zip")
    {
        plug_install->installFromFile(path);
    }
    else if ((path.section(".",-1))=="xml")
    {
        plug_install->installFromXML(path);
    }	
}

Q_EXPORT_PLUGIN2 ( plugman,plugMan );


