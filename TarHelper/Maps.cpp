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
}

void Maps::on_forest_clicked() {
	anim_slide(3);
	qimg.load(":/Maps/forest");
}

void Maps::on_interchange_clicked() {
	anim_slide(1);
	qimg.load(":/Maps/interchange");
}

void Maps::on_shoreline_clicked() {
	anim_slide(2);
	qimg.load(":/Maps/shoreline");
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
	return true;
}