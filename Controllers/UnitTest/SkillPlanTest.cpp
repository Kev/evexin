/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Eve-Xin/Controllers/SkillTree.h>

#include <Eve-Xin/Controllers/SkillPlan.h>

using namespace EveXin;

class SkillPlanTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(SkillPlanTest);
	CPPUNIT_TEST(testAdd_One);
	CPPUNIT_TEST(testAdd_Levels);
	CPPUNIT_TEST(testAdd_Dependencies);
	CPPUNIT_TEST(testRemove_All);
	CPPUNIT_TEST(testRemove_Last);
	CPPUNIT_TEST(testRemove_Some);
	CPPUNIT_TEST(testRemoveAdd);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp() {
		allSkills_ = boost::make_shared<SkillTree>();
		SkillItem::ref group = boost::make_shared<SkillItem>(allSkills_, "chainskills", "chainskills");
		allSkills_->addChild(group);
		for (int i = 1; i < 11; i++) {
			std::string id = "chained" + boost::lexical_cast<std::string>(i);
			Skill::ref skill = allSkills_->getSkill(id);
			SkillItem::ref skillItem = boost::make_shared<SkillItem>(group, skill);
			group->addChild(skillItem);
			std::vector<SkillLevel::ref> dependencies;
			for (int j = 1; j < i; j++) {
				std::string dependencyID = "chained" + boost::lexical_cast<std::string>(j);
				int dependencyLevel = (j + 1) / 2;
				SkillLevel::ref dependency = boost::make_shared<SkillLevel>(skillItem, allSkills_->getSkill(dependencyID), dependencyLevel);
				skillItem->addChild(dependency);
				dependencies.push_back(dependency);
			}
			skill->populate(group->getID(), group, "name " + id, "lalala", 2, SkillAttribute::Intelligence, SkillAttribute::Memory, dependencies);
		}
		plan_ = boost::make_shared<SkillPlan>(SkillItem::ref(), "id", "name", allSkills_);
	}

	void testAdd_One() {
		CPPUNIT_ASSERT_EQUAL(size(0), plan_->getChildren().size());
		plan_->addSkill(allSkills_->getSkill("chained1"), 1);
		CPPUNIT_ASSERT_EQUAL(size(1), plan_->getChildren().size());
	}

	void testAdd_Levels() {
		testAdd_One();
		plan_->addSkill(allSkills_->getSkill("chained1"), 4);
		CPPUNIT_ASSERT_EQUAL(size(4), plan_->getChildren().size());
	}

	void testAdd_Dependencies() {
		CPPUNIT_ASSERT_EQUAL(size(0), plan_->getChildren().size());
		plan_->addSkill(allSkills_->getSkill("chained4"), 2);
		std::vector<SkillItem::ref> plannedSkills = plan_->getChildren();

		CPPUNIT_ASSERT_EQUAL(std::string("chained1"), plannedSkills[0]->getID());
		CPPUNIT_ASSERT_EQUAL(1, level(plannedSkills[0]));

		CPPUNIT_ASSERT_EQUAL(std::string("chained2"), plannedSkills[1]->getID());
		CPPUNIT_ASSERT_EQUAL(1, level(plannedSkills[1]));

		CPPUNIT_ASSERT_EQUAL(std::string("chained3"), plannedSkills[2]->getID());
		CPPUNIT_ASSERT_EQUAL(1, level(plannedSkills[2]));

		CPPUNIT_ASSERT_EQUAL(std::string("chained3"), plannedSkills[3]->getID());
		CPPUNIT_ASSERT_EQUAL(2, level(plannedSkills[3]));

		CPPUNIT_ASSERT_EQUAL(std::string("chained4"), plannedSkills[4]->getID());
		CPPUNIT_ASSERT_EQUAL(1, level(plannedSkills[4]));

		CPPUNIT_ASSERT_EQUAL(std::string("chained4"), plannedSkills[5]->getID());
		CPPUNIT_ASSERT_EQUAL(2, level(plannedSkills[5]));

		CPPUNIT_ASSERT_EQUAL(size(6), plannedSkills.size());
	}

	void testRemove_Last() {
		testAdd_Dependencies();
		plan_->removeSkill("chained4", 2);
		std::vector<SkillItem::ref> plannedSkills = plan_->getChildren();
		CPPUNIT_ASSERT_EQUAL(std::string("chained4"), plannedSkills[4]->getID());
		CPPUNIT_ASSERT_EQUAL(1, level(plannedSkills[4]));

		CPPUNIT_ASSERT_EQUAL(size(5), plannedSkills.size());
	}

	void testRemove_All() {
		testAdd_Dependencies();
		plan_->removeSkill("chained1", 1);
		CPPUNIT_ASSERT_EQUAL(size(0), plan_->getChildren().size());
	}

	void testRemove_Some() {
		testAdd_Dependencies();
		plan_->removeSkill("chained3", 2);

		std::vector<SkillItem::ref> plannedSkills = plan_->getChildren();
		CPPUNIT_ASSERT_EQUAL(std::string("chained3"), plannedSkills[2]->getID());
		CPPUNIT_ASSERT_EQUAL(1, level(plannedSkills[2]));
		CPPUNIT_ASSERT_EQUAL(size(3), plan_->getChildren().size());
	}

	void testRemoveAdd() {
		testRemove_Some();
		plan_->addSkill(allSkills_->getSkill("chained4"), 1);
		std::vector<SkillItem::ref> plannedSkills = plan_->getChildren();
		CPPUNIT_ASSERT_EQUAL(std::string("chained4"), plannedSkills[4]->getID());
		CPPUNIT_ASSERT_EQUAL(1, level(plannedSkills[4]));

		CPPUNIT_ASSERT_EQUAL(size(5), plannedSkills.size());
	}

private:
	size_t size(int i) {
		return static_cast<size_t>(i);
	}

	int level(SkillItem::ref skill) {
		SkillLevel::ref levelItem = boost::dynamic_pointer_cast<SkillLevel>(skill);
		return levelItem ? levelItem->getLevel() : 0;
	}

	boost::shared_ptr<SkillTree> allSkills_;
	SkillPlan::ref plan_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SkillPlanTest);
