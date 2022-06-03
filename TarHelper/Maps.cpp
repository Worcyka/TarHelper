#include "Maps.h"
#include "qimage.h"
#include "qdebug.h"
#include "qpoint.h"
#include "qevent.h"
#include "qthread.h"
#include "qscrollbar.h"

Maps::Maps(QWidget *parent)
	: QWidget(parent), qimg(":/Maps/reserve"), imgSizeDivisor(1)
{	
	ui.setupUi(this);
	
	ui.maps->setPixmap(QPixmap::fromImage(qimg));

	ui.mapArea->installEventFilter(this);	//��װ�¼�������

	animation.setDuration(250);
	animation.setTargetObject(ui.slider);
	animation.setEasingCurve(QEasingCurve::InOutCirc);
}

Maps::~Maps() {

}

void Maps::paintEvent(QPaintEvent *event) {
	QWidget::paintEvent(event);
	static int paint(0);	//����paintEvent�¼�����һ�����봰��ʱ�������ó�ʼλ��
	if (paint <= 2) {		//����ÿ���ػ涼�����paintEvent���������ø���ֵ��Ȼ����Ч����Ч
		ui.slider->move(0, 0);
		++paint;
	}
}

/*ͼƬѡ��*/
void Maps::on_reserve_clicked() {
	anim_slide(0);
	qimg.load(":/Maps/reserve");
	ui.maps->setPixmap(QPixmap::fromImage(qimg));
}

void Maps::on_forest_clicked() {
	anim_slide(3);
	qimg.load(":/Maps/forest");
	ui.maps->setPixmap(QPixmap::fromImage(qimg));
}

void Maps::on_interchange_clicked() {
	anim_slide(1);
	qimg.load(":/Maps/interchange");
	ui.maps->setPixmap(QPixmap::fromImage(qimg));
}

void Maps::on_shoreline_clicked() {
	anim_slide(2);
	qimg.load(":/Maps/shoreline");
	ui.maps->setPixmap(QPixmap::fromImage(qimg));
}

void Maps::anim_slide(int num) {
	animation.setPropertyName("pos");
	animation.setStartValue(ui.slider->pos());
	animation.setEndValue(QPoint(0, 48 * num));
	animation.start();
}

void QAbstractScrollArea::wheelEvent(QWheelEvent* event) {
	return;		//���ع����¼� ������Թ���
}

bool Maps::eventFilter(QObject* watched, QEvent* event) {
	if (watched == ui.mapArea && event->type() == QEvent::Wheel) {
		QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
		QRect tmp = ui.maps->geometry();
		int adjustSize = 500;
		/*ͼ������*/
		if (wheelEvent->delta() > 0) {
			imgSizeDivisor > 1 ? --imgSizeDivisor : NULL;
			ui.maps->setPixmap(QPixmap::fromImage(
				qimg.scaled(qimg.width() / imgSizeDivisor, qimg.height() / imgSizeDivisor,
					Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
		}
		else if (wheelEvent->delta() < 0) {
			++imgSizeDivisor;
			ui.maps->setPixmap(QPixmap::fromImage(
				qimg.scaled(qimg.width() / imgSizeDivisor, qimg.height() / imgSizeDivisor,
					Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
		}
	}
	return true;
}