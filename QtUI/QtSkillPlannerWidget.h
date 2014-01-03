/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QWidget>

class QTreeView;

namespace EveXin {
	class DataController;
	class Character;
	class QtSkillModel;
	class QtSkillPlannerWidget : public QWidget {
		public:
			QtSkillPlannerWidget(boost::shared_ptr<DataController> dataController, QWidget* parent);
			~QtSkillPlannerWidget();
			void setCharacter(boost::shared_ptr<Character> character);
		private:
			boost::shared_ptr<DataController> dataController_;
			QTreeView* allSkillsWidget_;
			QTreeView* planWidget_;
			QtSkillModel* allSkillsModel_;
	};
}
