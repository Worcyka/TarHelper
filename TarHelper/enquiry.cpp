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
	/*清空tableWidget*/
	ui.tableWidget->clear();
	/*将获得的返回值转换为Json并解析*/
	QTextCodec* codec = QTextCodec::codecForName("utf8");
	QString replyAPIText = codec->toUnicode(replyAPI->readAll());
	QJsonDocument replyAPIJson = QJsonDocument::fromJson(replyAPIText.toUtf8());
	/*读取Json中Array的每一个Object*/
	QJsonArray jsonArray = replyAPIJson.array();
	ui.tableWidget->setRowCount(jsonArray.size());
	for (int objCount(0); objCount < jsonArray.size(); ++objCount) {
		QJsonValue jsonValue = jsonArray.at(objCount);
		QJsonObject jsonObject = jsonValue.toObject();
		/*遍历Object的每个key，找到需要的key*/
		QStringList keys = jsonObject.keys();
		for (int i = 0; i < keys.size(); ++i) {
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
				/*创建一个QNetworkReply对象，作为参数传递给函数*/
				QNetworkReply* replyIcon = networkManager->get(req);  //HTML请求

				connect(replyIcon, &QNetworkReply::finished, [=]() {
					Enquiry::replyFinishedIcon(objCount, replyIcon);
				});  //当请求完成时，执行函数配置icon
			}
		}
	}
	replyAPI->deleteLater(); //用户有责任在适当的时候删除 QNetworkreplyAPI 对象
}

void Enquiry::replyFinishedIcon(int row, QNetworkReply* replyIcon) {
	/*配置Icon*/
	QPixmap pixmap;
	pixmap.loadFromData(replyIcon->readAll());
	QIcon icon;
	icon.addPixmap(pixmap);
	QTableWidgetItem* iconItem = new QTableWidgetItem;
	iconItem->setIcon(icon);
	ui.tableWidget->setItem(row, 0, iconItem);

	replyIcon->deleteLater();  //用户有责任在适当的时候删除 QNetworkreplyAPI 对象
}

void Enquiry::on_lineEdit_returnPressed() {
	QString url = "https://mp.soulofcinder.xyz/eft/api/mainSearch?tag=&page_num=1&page_size=300&item_name=";
	url = url + ui.lineEdit->text();
	req.setUrl(QUrl(url));
	replyAPI = networkManager->get(req);  //HTML请求

	connect(replyAPI, &QNetworkReply::finished, this, &Enquiry::replyFinishedAPI);  //当请求完成时，执行槽函数
}