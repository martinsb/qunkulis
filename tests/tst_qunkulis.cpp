
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

void TestQunkulis::testExecWithVariables()
{
	VirtualMachine* vm = m_db.compile("$c = $a + $b");
	QVERIFY(vm);
	vm->reset();
	vm->setVariable("a", 2);
	vm->setVariable("b", 3);
	QVERIFY(vm->execute());
	QVariant result = vm->extractVariable("c");	
	QCOMPARE(result.toInt(), 5);
	delete vm;
}

void TestQunkulis::testExecWithArguments()
{
    VirtualMachine* vm = m_db.compile("$c = $argv[0] + $argv[1];");
    QVERIFY(vm);
    vm->reset();
    vm->addArgument("3");
    vm->addArgument("2");
    QVERIFY(vm->execute());
    QVariant result = vm->extractVariable("c");
    QCOMPARE(result.toInt(), 5);
    delete vm;
}

#define INSERT_PROG \
    "if (!db_exists('users')) {" \
    "  db_create('users');" \
    "}" \
    "$zRec = { name: 'foo', age: 27 };" \
    "db_store('users', $zRrec);" \
    "$num = db_total_records('users');"
void TestQunkulis::testScriptInsertRecord()
{
    VirtualMachine* vm = m_db.compile(INSERT_PROG);
    QVERIFY(vm);
    vm->reset();
    QVERIFY(vm->execute());
    QVariant result = vm->extractVariable("num");
    QCOMPARE(result.toInt(), 1);
    delete vm;   
}

QTEST_MAIN(TestQunkulis)
