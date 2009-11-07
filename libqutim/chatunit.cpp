#include "chatunit_p.h"

namespace qutim_sdk_0_3
{
	ChatUnit::ChatUnit(Account *account) : QObject(account), p(new ChatUnitPrivate)
	{
		p->account = account;
	}

	ChatUnit::ChatUnit(Account *account, ChatUnitPrivate *d) : p(d)
	{
		p->account = account;
	}

	ChatUnit::~ChatUnit()
	{
	}

	Account *ChatUnit::account()
	{
		return p->account;
	}

	const Account *ChatUnit::account() const
	{
		return p->account;
	}
}
