#include "PointerList.h"

Qtilities::Core::PointerList::PointerList(bool cleanup_when_done, QObject *parent) : PointerListDeleter() {
    Q_UNUSED(parent)

    cleanup_enabled = cleanup_when_done;
}

Qtilities::Core::PointerList::~PointerList() {
    if (cleanup_enabled) {
            list.clear();
    }
}

//! Appends a new instance of T to the PointerList.
void Qtilities::Core::PointerList::append(QObject* object) {
    addThisObject(object);
    list.append(object);
}

void Qtilities::Core::PointerList::deleteAll() {
    int total =list.count();
    for (int i = 0; i < total; i++) {
        delete list.at(0);
    }
    list.clear();
}

int Qtilities::Core::PointerList::count() const {
    return list.count();
}

QObject* Qtilities::Core::PointerList::at(int i) const {
    return list.at(i);
}

void Qtilities::Core::PointerList::removeThisObject(QObject * object) {
    list.removeOne(object);
    emit objectDestroyed(object);
}

void Qtilities::Core::PointerList::removeOne(QObject* obj) {
    list.removeOne(obj);
}

void Qtilities::Core::PointerList::addThisObject(QObject * obj) {
    QObject::connect(obj, SIGNAL(destroyed(QObject *)), this, SLOT(removeSender()));
}

QMutableListIterator<QObject*> Qtilities::Core::PointerList::iterator() {
    QMutableListIterator<QObject*> itr(list);
    return itr;
}
