#ifndef FLAP_H
#define FLAP_H

#include "dataunit.h"

class QIODevice;

class FLAP : public DataUnit
{
public:
	FLAP(quint8 channel = 0x02);
	inline quint8 channel() const { return m_channel; }
	inline void setChannel(quint8 channel) { m_channel = channel; }
	inline quint16 seqNum() const { return m_sequence_number; }
	inline void setSeqNum(quint8 seqnum) { m_sequence_number = seqnum; }
	QByteArray toByteArray() const;
	QByteArray header() const;
	bool readData(QIODevice *dev);
	inline bool isFinished() const { qDebug("%d", m_length); return m_state == Finished; }
	void clear();
private:
	enum State { ReadHeader, ReadData, Finished } m_state;
	quint8 m_channel;
	quint16 m_sequence_number;
	quint16 m_length;
};

#endif // FLAP_H
