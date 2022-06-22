#include "enquiry.h"
#include "qdebug.h"
#include "qtextcodec.h"
#include "qjsonarray.h"
#include "qjsonobject.h"
#include "qtablewidget.h"
#include "qjsondocument.h"
#include "qnetworkreply.h"

Enquiry::Enquiry(QWidget* parent)
	: QWidget(parent), replyAPI(NULL)
{
	ui.setupUi(this);

	ui.tableWidget->setColumnCount(3);

	req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	req.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
	req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36");

	networkManager = new QNetworkAccessManager(this);
}

Enquiry::~Enquiry()
{	
	delete networkManager;
}

void Enquiry::replyFinishedAPI(){
	/*���tableWidget*/
	ui.tableWidget->clear();
	/*����õķ���ֵת��ΪJson������*/
	QTextCodec* codec = QTextCodec::codecForName("utf8");
	QString replyAPIText = codec->toUnicode(replyAPI->readAll());
	QJsonDocument replyAPIJson = QJsonDocument::fromJson(replyAPIText.toUtf8());
	/*��ȡJson��Array��ÿһ��Object*/
	QJsonArray jsonArray = replyAPIJson.array();
	ui.tableWidget->setRowCount(jsonArray.size());
	for (int objCount = 0; objCount < jsonArray.size(); objCount++) {
		QJsonValue jsonValue = jsonArray.at(objCount);
		QJsonObject jsonObject = jsonValue.toObject();
		/*����Object��ÿ��key���ҵ���Ҫ��key*/
		QStringList keys = jsonObject.keys();
		for (int i = 0; i < keys.size(); i++) {
			QString key = keys.at(i);
			if (key == "cnName") {
				QJsonValue value = jsonObject.value(key);
				QString cnName = value.toString();
				qDebug() << cnName;

				QTableWidgetItem* nameItem = new QTableWidgetItem;
				nameItem->setText(cnName);
				nameItem->setTextColor("#B8B6B4");
				ui.tableWidget->setItem(objCount, 1, nameItem);
			}
			if (key == "avgDayPrice") {
				QJsonValue value = jsonObject.value(key);
				double valueDouble = value.toDouble();
				QString valueStr = QString::number(valueDouble, 'f', 0);

				QTableWidgetItem* valueItem = new QTableWidgetItem;
				valueItem->setText(valueStr);
				valueItem->setTextColor("#B8B6B4");
				ui.tableWidget->setItem(objCount, 2, valueItem);
			}
			if (key == "wikiIcon") {
				QJsonValue value = jsonObject.value(key);
				req.setUrl(QUrl(value.toString()));
				/*����һ��QNetworkReply������Ϊ�������ݸ�����*/
				QNetworkReply* replyIcon = networkManager->get(req);  //HTML����

				connect(replyIcon, &QNetworkReply::finished, [=]() {
					Enquiry::replyFinishedIcon(objCount, replyIcon);
				});  //���������ʱ��ִ�к�������icon
			}
		}
	}
	replyAPI->deleteLater(); //�û����������ʵ���ʱ��ɾ�� QNetworkreplyAPI ����
}

void Enquiry::replyFinishedIcon(int row, QNetworkReply* replyIcon) {
	/*����Icon*/
	QTableWidgetItem* iconItem = new QTableWidgetItem;
	QPixmap pixmap;
	QIcon icon;

	pixmap.loadFromData(replyIcon->readAll());
	icon.addPixmap(pixmap);
	iconItem->setIcon(icon);
	ui.tableWidget->setItem(row, 0, iconItem);

	replyIcon->deleteLater();
}

void Enquiry::on_lineEdit_returnPressed() {
	QString url = "https://mp.soulofcinder.xyz/eft/api/mainSearch?tag=&page_num=1&page_size=300&item_name=";
	url = url + ui.lineEdit->text();
	req.setUrl(QUrl(url));
	replyAPI = networkManager->get(req);  //HTML����

	connect(replyAPI, &QNetworkReply::finished, this, &Enquiry::replyFinishedAPI);  //���������ʱ��ִ�вۺ���
}

void Enquiry::on_pushButton_clicked() {
	/*debug Here*/
};