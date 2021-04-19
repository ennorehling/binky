#include "gamedata.h"
#include "CuTest.h"

static void test_stringtable(CuTest *tc)
{
    stringtable st = { 0 };
    CuAssertIntEquals(tc, 0, strings_put(&st, "VERSION"));
    CuAssertIntEquals(tc, 1, strings_put(&st, "Runde"));
    CuAssertStrEquals(tc, "Runde", strings_get(&st, 1));
    CuAssertIntEquals(tc, 1, strings_put(&st, "Runde"));
}

CuSuite *get_gamedata_suite(void)
{
    CuSuite *suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_stringtable);
    return suite;
}
