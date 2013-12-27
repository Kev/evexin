/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QMainWindow>

#include <Eve-Xin/Controllers/Character.h>

class QComboBox;

namespace EveXin {
	class DataController;
	class QtCharacterPane;
	class QtMainWindow : public QMainWindow {
		Q_OBJECT
		public:
			QtMainWindow(boost::shared_ptr<DataController>);
			~QtMainWindow();

		private slots:
			void handleAddKey();
			void handleCharacterSelected(int index);
			private:
			void handleCharacterListUpdated();
			void handleCharacterDataUpdated(const std::string& id);
			Character::ref selectedCharacter();
		private:
			QComboBox* characterComboBox_;
			QtCharacterPane* characterPane_;
			boost::shared_ptr<DataController> dataController_;

	};

}
