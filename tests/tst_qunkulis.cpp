
#include "tst_qunkulis.h"

#include "database.h"
#include "virtualmachine.h"

void TestQunkulis::init()
{
	QVERIFY(m_db.open(":mem:", InMemory));
}

void TestQunkulis::cleanup()
{
	m_db.close();
}

void TestQunkulis::testKeyValueStore() 
{    
    QVERIFY(m_db.store("foo", "bar"));
    QString stored = m_db.fetch("foo");
    QCOMPARE(stored, QString("bar"));
}

void TestQunkulis::testCompile()
{
	VirtualMachine* vm = m_db.compile("$a = 5;");
	QVERIFY(vm);
	delete vm;
}

void TestQunkulis::testVMExecution()
{
	VirtualMachine* vm = m_db.compile("$c = $a + $b");
	QVERIFY(vm);
	vm->reset();
	vm->setParameter("a", 2);
	vm->setParameter("b", 3);
	QVERIFY(vm->execute());
	QVariant result = vm->extractVariable("c");	
	QCOMPARE(result.toInt(), 5);
	delete vm;
}

QTEST_MAIN(TestQunkulis)
