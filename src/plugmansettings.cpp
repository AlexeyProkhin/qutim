#include "plugmansettings.h"
#include "plugxmlhandler.h"
#include <QListWidgetItem>
#include "pluginstaller.h"
#include <QDebug>
plugmanSettings::plugmanSettings(const QString &profile_name)
: m_package_model(new plugPackageModel)
{
 	m_plug_manager = new plugManager (this);
	setupUi(this);
	m_profile_name = profile_name;
	PackagesTab->layout()->addWidget(m_plug_manager); 
}

plugmanSettings::~plugmanSettings()
{
	delete(m_plug_manager);
}

plugManager* plugmanSettings::getPlugManager() {
	return m_plug_manager;
}

void plugmanSettings::setPlugManager(plugManager* plug_manager) {
	m_plug_manager = plug_manager;
}
