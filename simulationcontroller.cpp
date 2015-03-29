#include "simulationcontroller.h"

#include <QDebug>
#include <limits>

#include <element/clock.h>

#include <nodes/qneconnection.h>

#include <priorityqueue.h>

SimulationController::SimulationController(QGraphicsScene * scn) : QObject(dynamic_cast<QObject *>(scn)), timer(this) {
  scene = scn;
  timer.start(10);
  connect(&timer,&QTimer::timeout,this,&SimulationController::update);
}

SimulationController::~SimulationController() {

}

int SimulationController::calculatePriority(GraphicElement * elm){
  if(!elm)
    return 0;
  if(elm->beingVisited()){
    elm->setChanged(true);
    qDebug() << elm->objectName() << " being visited";
    return 0;
  }
  if(elm->visited()){
    qDebug() << elm->objectName() << " is visited";
    return elm->priority();
  }
  int max = 0;
  foreach (QNEPort * port, elm->outputs()) {
    foreach (QNEConnection * conn, port->connections()) {
      QNEPort * sucessor = conn->port1();
      if(sucessor == port){
        sucessor = conn->port2();
      }
      if(sucessor)
        max = qMax(calculatePriority(sucessor->graphicElement()),max);
      qDebug() << elm->objectName() << " max = " << max;
    }
  }
  qDebug() << elm->objectName() << " priority set to " << max + 1;
  elm->setPriority(max + 1);
  elm->setBeingVisited(false);
  elm->setVisited(true);
  return( elm->priority() );
}

void SimulationController::update() {
  QList<QGraphicsItem*> items = scene->items();
  QVector<GraphicElement *> elements;
  foreach (QGraphicsItem * item, items) {
    GraphicElement * elm = qgraphicsitem_cast<GraphicElement *>(item);
    if(elm){
      elements.append(elm);
    }
  }
  QVector<GraphicElement*> changed;
  foreach (GraphicElement * elm, elements) {
    if(elm->changed()){
      changed.append(elm);
    }
  }

  if(changed.isEmpty()){
    return;
  }

  foreach (GraphicElement * elm, changed) {
    if(elm)
      calculatePriority(elm);
  }

  PriorityQueue queue (elements);
  while (! queue.isEmpty()) {
    GraphicElement * elm = (GraphicElement *) queue.pop();
    elm->updateLogic();
    elm->setChanged(false);
    elm->setBeingVisited(false);
    elm->setVisited(false);
    qDebug() << elm->objectName() << ", " << elm->priority();
  }
}



