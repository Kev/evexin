/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QWidget>

class QTreeView;
class QPushButton;

namespace EveXin {
	class DataController;
	class Character;
	class QtSkillModel;
	class QtTreeView;
	class QtSkillPlannerWidget : public QWidget {
		Q_OBJECT
		public:
			QtSkillPlannerWidget(boost::shared_ptr<DataController> dataController, QWidget* parent);
			~QtSkillPlannerWidget();
			void setCharacter(boost::shared_ptr<Character> character);
		private slots:
			void handleCreatePlanClicked();
			void handleDeletePlanClicked();
		private:
			boost::shared_ptr<DataController> dataController_;
			boost::shared_ptr<Character> character_;
			QtSkillModel* allSkillsModel_;
			QtSkillModel* planModel_;
			QPushButton* createPlanButton_;
			QPushButton* deletePlanButton_;
			QtTreeView* planWidget_;
	};
}
