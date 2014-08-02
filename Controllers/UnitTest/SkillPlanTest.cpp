/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/make_shared.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Eve-Xin/Controllers/SkillTree.h>

#include "Eve-Xin/Controllers/SkillPlan.h"

using namespace EveXin;

class SkillPlanTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(SkillPlanTest);
	CPPUNIT_TEST(testAdd);
	CPPUNIT_TEST(testRemove);
	CPPUNIT_TEST(testRemoveAdd);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp() {
		allSkills_ = boost::make_shared<SkillTree>();
		plan_ = boost::make_shared<SkillPlan>(SkillItem::ref(), "id", "name", allSkills_);
	}

	void testAdd() {
		CPPUNIT_ASSERT(false);
	}

	void testRemove() {
		
	}

	void testRemoveAdd() {
		
	}

private:
	boost::shared_ptr<SkillTree> allSkills_;
	SkillPlan::ref plan_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SkillPlanTest);
