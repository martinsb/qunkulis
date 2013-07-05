#ifndef _TST_QUNKULIS_H
#define _TST_QUNKULIS_H

#include <QtTest/QtTest>

#include "database.h"

using namespace UnqliteQt;

class TestQunkulis : public QObject
{
    Q_OBJECT

private:
	Database m_db;

private slots:
	void init();
	void cleanup();

    void testKeyValueStore(); 

    void testCompile();

    void testExecWithVariables();

    void testExecWithArguments();

    void testScriptInsertRecord();
};

#endif //_TST_QUNKULIS_H

