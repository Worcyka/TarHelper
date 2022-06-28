#include "enquiry.h"
#include "qdebug.h"
#include "qlabel.h"
#include "qtimer.h"
#include "qbuffer.h"
#include "qscreen.h"
#include "qtextcodec.h"
#include "qjsonarray.h"
#include "qmessagebox.h"
#include "qjsonobject.h"
#include "qtablewidget.h"
#include "qjsondocument.h"
#include "qnetworkreply.h"

Enquiry::Enquiry(QWidget* parent)
	: QWidget(parent), replyAPI(NULL)
{
	ui.setupUi(this);

	ui.tableWidget->setColumnCount(3);
	ui.tableWidget->setColumnWidth(0, 96);

	QTimer* timer = new QTimer(this);
	//timer->start(5000);
	connect(timer, &QTimer::timeout, this, &Enquiry::on_pushButton_clicked);

	networkManager = new QNetworkAccessManager(this);
}

Enquiry::~Enquiry()
{	
	delete networkManager;
}

void Enquiry::replyFinishedAPI(){
	if (replyAPI->error()) {
		qDebug() << replyAPI->errorString();
		replyAPI->deleteLater();
		QMessageBox::critical(this, "Network Error", replyAPI->errorString());
		return;
	}
	int statusCode = replyAPI->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	qDebug() << "statusCode:" << statusCode;
	if (statusCode != 200) {
		QMessageBox::critical(this, "Network Error", static_cast<QString>(statusCode));
		return;
	}
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
		ui.tableWidget->setRowHeight(objCount, 96);
		for (int i = 0; i < keys.size(); i++) {
			QString key = keys.at(i);
			if (key == "cnName") {
				QJsonValue value = jsonObject.value(key);
				QString cnName = value.toString();
				qDebug() << cnName;

				QTableWidgetItem* nameItem = new QTableWidgetItem;
				nameItem->setText(cnName);
				nameItem->setTextColor("#B8B6B4");
				//nameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter); //����
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
				QNetworkRequest req;
				req.setUrl(QUrl(value.toString()));
				/*����һ��QNetworkReply������Ϊ�������ݸ�����*/
				QNetworkReply* replyIcon = networkManager->get(req);  //HTML����

				connect(replyIcon, &QNetworkReply::finished, [=]() {
					Enquiry::replyFinishedIcon(objCount, replyIcon);
				});  //���������ʱ��ִ�к�������icon
			}
		}
	}
	replyAPI->abort();
	replyAPI->deleteLater(); //�û����������ʵ���ʱ��ɾ�� QNetworkreplyAPI ����
}

void Enquiry::replyFinishedIcon(int row, QNetworkReply* replyIcon) {
	/*����Icon*/
	QLabel* icon = new QLabel;
	QPixmap pixmap;

	pixmap.loadFromData(replyIcon->readAll());
	icon->setPixmap(pixmap.scaled(72, 72, Qt::KeepAspectRatio, Qt::SmoothTransformation)); //ͼ������
	icon->setAlignment(Qt::AlignCenter);
	if (row%2){
		icon->setStyleSheet("background: #323130");	//���и���
	}
	ui.tableWidget->setCellWidget(row, 0, icon);
	
	replyIcon->abort();
	replyIcon->deleteLater();
}

void Enquiry::on_lineEdit_returnPressed() {
	QString url = "https://mp.soulofcinder.xyz/eft/api/mainSearch?tag=&page_num=1&page_size=300&item_name=";
	url = url + ui.lineEdit->text();
	QNetworkRequest req;
	req.setUrl(QUrl(url));
	replyAPI = networkManager->get(req);  //HTML����

	connect(replyAPI, &QNetworkReply::finished, this, &Enquiry::replyFinishedAPI);  //���������ʱ��ִ�вۺ���
}

void Enquiry::on_pushButton_clicked() {
	/*debug Here*/
	QPoint	posPoint = QCursor::pos();	//��ȡָ��λ��
	qDebug() << posPoint;
	
	QScreen* screen = QGuiApplication::primaryScreen();
	QPixmap pixmap = screen->grabWindow(0, posPoint.x()+14, posPoint.y()-42,  300, 30);	//����ָ��λ�ý�ͼ
	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "png");
	auto const base64 = buffer.data().toBase64();	//��ͼƬת��Ϊbase64
	//pixmap.save("C:/Users/10637_c4lx35f/Desktop/test.png");
	/*����POST����*/
	QNetworkRequest req;
	QUrl url = "https://aip.baidubce.com/rest/2.0/ocr/v1/general_basic";
	QString access_token = "24.642429a0b17f786950c2c65de79063f2.2592000.1658895930.282335-26556990";
	url = url.toString() + "?access_token=" + access_token;	
	req.setUrl(url);
	req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
	QByteArray data;
	data.append("image=");
	data.append(QUrl::toPercentEncoding(base64));

	QNetworkReply* replyOCR = networkManager->post(req, data);
	connect(replyOCR, &QNetworkReply::finished, this, [=]() {
		Enquiry::replyFinishedOCR(replyOCR);
	});
};

void Enquiry::replyFinishedOCR(QNetworkReply* replyOCR) {
	qDebug() << replyOCR->error();
	QString replyOCRText = replyOCR->readAll();

	qDebug() << "\n";
	qDebug() << replyOCRText;

	replyOCR->abort();
	replyOCR->deleteLater();
}