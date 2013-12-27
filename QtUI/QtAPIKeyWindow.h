/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QWidget>

class QLineEdit;
class QPushButton;

namespace EveXin {
	class DataController;

	class QtAPIKeyWindow : public QWidget {
		Q_OBJECT
		public:
			QtAPIKeyWindow(boost::shared_ptr<DataController> dataController);
		private slots:
			void handleAddKey();
		private:
			boost::shared_ptr<DataController> dataController_;
			QLineEdit* keyEdit_;
			QLineEdit* verEdit_;
			QPushButton* addButton_;
	};

}

