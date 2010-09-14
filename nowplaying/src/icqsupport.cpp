#include "icqsupport.h"
#include <qutim/event.h>
#include <qutim/account.h>
#include <QApplication>
#include "nowplaying.h"
#include "ui_icqsettings.h"

namespace qutim_sdk_0_3 {

namespace nowplaying {

	IcqTuneStatus::IcqTuneStatus()
	{
	}

	IcqTuneStatus::IcqTuneStatus(Account *account, IcqTuneStatus *factory) :
			AccountTuneStatus(account, factory), m_icqFactory(factory)
	{
		icqChangeXstatusEvent = Event::registerType("icq-change-xstatus");
	}

	void IcqTuneStatus::setStatus(const TrackInfo &info)
	{
		OscarSettings config = (NowPlaying::instance()->forAllAccounts()) ?
					   m_settings :
					   m_icqFactory->m_settings;
		if (config.deactivated)
			return;
		QString msg = message(info);
		QVariantHash xstatus = m_account->status().extendedInfo("xstatus");
		if (config.setsCurrentStatus && xstatus.value("description").toString() != msg) {
			return;
		} else if (config.setsMusicStatus &&
				   xstatus.value("name").toString() == "music" &&
				   xstatus.value("description").toString() != msg)
		{
			return;
		}

		QVariantHash extStatus;
		extStatus.insert("title", tr("Listening"));
		extStatus.insert("name", "listening_to_music");
		extStatus.insert("description", msg);
		Event ev(icqChangeXstatusEvent);
		ev.args[0] = extStatus;
		qApp->sendEvent(m_account, &ev);
	}

	void IcqTuneStatus::removeStatus()
	{
		Event ev(icqChangeXstatusEvent);
		qApp->sendEvent(m_account, &ev);
	}

	void IcqTuneStatus::loadSettings()
	{
		Config cfg = config(m_account ? m_account->id() : "oscar");
		m_settings.deactivated = cfg.value("deactivated", false);
		m_settings.setsCurrentStatus = cfg.value("setsCurrentStatus", false);
		m_settings.setsMusicStatus = cfg.value("setsMusicStatus", true);
		m_settings.mask_1 = cfg.value("mask1", QString("Now playing: %artist - %title"));
		m_settings.mask_2 = cfg.value("mask2", QString("%artist - %title"));
	}

	AccountTuneSettings *IcqTuneStatus::settingsWidget()
	{
		return new IcqTuneSettings();
	}

	AccountTuneStatus *IcqTuneStatus::construct(Account *account, AccountTuneStatus *factory)
	{
		return new IcqTuneStatus(account, qobject_cast<IcqTuneStatus*>(factory));
	}

	QString IcqTuneStatus::message(const TrackInfo &info)
	{
		OscarSettings config = (NowPlaying::instance()->forAllAccounts()) ?
					   m_settings :
					   m_icqFactory->m_settings;
		QString message = (config.setsCurrentStatus) ? config.mask_1 : config.mask_2;
		message.replace("%artist", info.artist).replace("%title", info.title).
				replace("%album", info.album).replace("%track", info.track_number).
				replace("%file", info.file_name).replace("%uri", info.uri).
				replace("%time", info.time);
		return message;
	}

	IcqTuneSettings::IcqTuneSettings() :
		ui(new Ui::IcqSettings)
	{
		ui->setupUi(this);
	}

	IcqTuneSettings::~IcqTuneSettings()
	{
		delete ui;
	}

	void IcqTuneSettings::loadState(AccountTuneStatus *acc)
	{
		IcqTuneStatus *account = qobject_cast<IcqTuneStatus*>(acc);
		if (!account)
			return;
		OscarSettings settings = m_settings.contains(account) ?
						 m_settings.value(account) :
						 account->settings();
		ui->oscar_deactivated->setChecked(settings.deactivated);
		ui->oscar_change_current->setChecked(settings.setsCurrentStatus);
		ui->oscar_change_music_status->setChecked(settings.setsMusicStatus);
		ui->oscar_mask_1->setText(settings.mask_1);
		ui->oscar_mask_2->setText(settings.mask_2);
	}

	void IcqTuneSettings::saveState(AccountTuneStatus *acc)
	{
		IcqTuneStatus *account = qobject_cast<IcqTuneStatus*>(acc);
		if (!account)
			return;
		OscarSettings settings;
		settings.deactivated = ui->oscar_deactivated->isChecked();
		settings.setsCurrentStatus = ui->oscar_change_current->isChecked();
		settings.setsMusicStatus = ui->oscar_change_music_status->isChecked();
		settings.mask_1 = ui->oscar_mask_1->text();
		settings.mask_2 = ui->oscar_mask_2->text();
		m_settings.insert(account, settings);
	}

	void IcqTuneSettings::clearStates()
	{
		m_settings.clear();
	}

	void IcqTuneSettings::saveConfigs()
	{
		QHashIterator<IcqTuneStatus*, OscarSettings> itr(m_settings);
		while (itr.hasNext()) {
			itr.next();
			Account *acc = itr.key()->account();
			const OscarSettings &settings = itr.value();
			Config cfg = config(acc ? acc->id() : "oscar");
			cfg.setValue("deactivated", settings.deactivated);
			cfg.setValue("sets_current_status", settings.setsCurrentStatus);
			cfg.setValue("sets_music_status", settings.setsMusicStatus);
			cfg.setValue("mask_1", settings.mask_1);
			cfg.setValue("mask_2", settings.mask_2);
		}

	}

} }
