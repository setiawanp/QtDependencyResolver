/****************************************************************************
* Copyright (C) 2013 Sergey Pupko <s.d.pupko@gmail.com>
*
* This library is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "DIContainer.h"

#include <QMetaObject>
#include <QMetaMethod>
#include <QDebug>

class CtorArg
{
public:
    CtorArg(QString name, void* data):
        _name(name.toUtf8()),
        _data(data),
        _hasValue(false) {
    }

    CtorArg(QString name, QVariant value):
        _name(name.toUtf8()),
        _value(value),
        _hasValue(true) {
    }

    QGenericArgument toQArg() {
        if (_hasValue) {
            return QGenericArgument(_name.data(), _value.constData());
        }

        return _name == ""?
                    QGenericArgument() :
                    QGenericArgument(_name.data(), &_data);
    }

private:
    QByteArray _name;
    void *_data;
    QVariant _value;
    bool _hasValue;

    Q_DISABLE_COPY(CtorArg)
};
typedef QSharedPointer<CtorArg> CtorArgPtr;

using namespace QtDependencyResolver;

class DIContainer::P : public QObject
{
public:
    explicit P(QObject *parent = 0) :
        QObject(parent) {
    }

    virtual ~P() {
    }

    void RegisterSingletonMetaObject(const QString &typeName, const QMetaObject &metaObject) {
        _singletonMetaObjects.insert(typeName, metaObject);
    }

    bool ContainsSingletonMetaObject(const QString &typeName) {
        return _singletonMetaObjects.contains(typeName);
    }

    QMetaObject GetSingletonMetaObject(const QString &typeName) {
        return ContainsSingletonMetaObject(typeName)?
                    _singletonMetaObjects.value(typeName) :
                    QMetaObject();
    }

    void RegisterSingletonValue(const QString &typeName, QObject* const &value) {
        if (!_singletonObjects.contains(typeName)) {
            _singletonObjects.insert(typeName, value);
        }
    }

    bool ContainsSingletonValue(const QString &typeName) {
        if (!_singletonObjects.contains(typeName)) {
            return false;
        }

        return true;
    }

    QObject* GetSingletonValue(const QString &typeName) {
        return ContainsSingletonValue(typeName)?
                    _singletonObjects[typeName] :
                    NULL;
    }

    void RegisterMetaObject(const QString &typeName, const QMetaObject &metaObject) {
        _metaObjects.insert(typeName, metaObject);
    }

    bool ContainsMetaObject(const QString &typeName) {
        return _metaObjects.contains(typeName);
    }

    QMetaObject GetMetaObject(const QString &typeName) {
        return _metaObjects.value(typeName, QMetaObject());
    }

    void RegisterValue(const QString &typeName, const QString &key, const QVariant &value) {
        if (!_objects.contains(typeName)) {
            _objects.insert(typeName, QHash<QString, QVariant>());
        }

        _objects[typeName].insert(key, value);
    }

    bool ContainsValue(const QString &typeName, const QString &key) {
        if (!_objects.contains(typeName)) {
            return false;
        }

        if (!_objects[typeName].contains(key)) {
            return false;
        }

        return true;
    }

    QVariant GetValue(const QString &typeName, const QString &key) {
        return ContainsValue(typeName, key)?
                    _objects[typeName].value(key) :
                    QVariant();
    }

    QObject* ResolveByName(QString typeName) {
        typeName = typeName.replace("*", "");
        typeName = typeName.replace("&", "");

        DIContainer::Scope scope = DIContainer::NO_SCOPE;
        if (ContainsSingletonMetaObject(typeName)) {
            scope = DIContainer::SINGLETON;
        }
        else if (ContainsMetaObject(typeName)) {
            scope = DIContainer::NO_SCOPE;
        }
        else {
            qDebug("DIContainer: %s is not registered or may be you forgot specify namespace in constructor!", qPrintable(typeName));
            return NULL;
        }

        if (scope == DIContainer::SINGLETON)
            return ResolveSingleton(GetSingletonMetaObject(typeName));
        else
            return ResolveNoScope(GetMetaObject(typeName));

    }

    QObject* ResolveNoScope(const QMetaObject& metaObject) {
        QMetaMethod constructorType = metaObject.constructor(0);
        QList<CtorArgPtr> ctorArguments;

        for (quint8 index = 0; index < 10; index++) {
            if (index >= constructorType.parameterTypes().count()) {
                CtorArgPtr ctorArg(new CtorArg("", 0));
                ctorArguments << ctorArg;
                continue;
            }

            QString argType = constructorType.parameterTypes().at(index);
            QString argName = constructorType.parameterNames().at(index);

            if (argType == "QObject*" && argName == "parent") {
                CtorArgPtr ctorArg(new CtorArg("QObject*", 0));
                ctorArguments << ctorArg;
                continue;
            }

            if (ContainsValue(argType, argName)) {
                CtorArgPtr ctorArg(new CtorArg(argType, GetValue(argType, argName)));
                ctorArguments << (ctorArg);
                continue;
            }

            QObject* argValue = ResolveByName(argType);
            if (argValue == NULL) {
                return NULL;
            }

            CtorArgPtr ctorArg(new CtorArg(argType, static_cast<void *>(argValue)));
            ctorArguments << ctorArg;
        }

        QObject  *instance = metaObject.newInstance(ctorArguments[0]->toQArg(), ctorArguments[1]->toQArg(), ctorArguments[2]->toQArg(), ctorArguments[3]->toQArg(), ctorArguments[4]->toQArg(),
                ctorArguments[5]->toQArg(),ctorArguments[6]->toQArg(),ctorArguments[7]->toQArg(),ctorArguments[8]->toQArg(),ctorArguments[9]->toQArg());

        if (!instance) {
            qDebug("DIContainer: could not create an instance of class %s", metaObject.className());
            return NULL;
        }

        return instance;
    }

    QObject* ResolveSingleton(const QMetaObject& metaObject) {
        QString typeName = metaObject.className();
        if (ContainsSingletonValue(typeName)) {
            //qDebug("Get singleton instance %s", qPrintable(metaObject.className()));
            return GetSingletonValue(typeName);
        }
        else {
            QObject* object = ResolveNoScope(metaObject);
			object->setParent(this);
			
            RegisterSingletonValue(typeName, object);
            return object;
        }
    }

private:
    QHash<QString, QMetaObject> _singletonMetaObjects;
    QHash<QString, QObject*> _singletonObjects;

    QHash<QString, QMetaObject> _metaObjects;
    QHash<QString, QHash<QString, QVariant> > _objects;
};

DIContainer::DIContainer(QObject *parent) :
    QObject(parent),
    _d(QSharedPointer<P>(new P)) {
}

DIContainer::~DIContainer() {
}

QObject *DIContainer::ResolveByClassName(const QString &className) {
    return _d->ResolveByName(className);
}

QObject *DIContainer::ResolveMetaobject(QMetaObject metaObject)
{
    return _d->ResolveByName(metaObject.className());
}

void DIContainer::ClassBind(const QMetaObject &resolvableTypeMeta,
                            const QMetaObject &typeMeta,
                            const DIContainer::Scope &scope) {
    if (scope == DIContainer::NO_SCOPE)
        _d->RegisterMetaObject(resolvableTypeMeta.className(), typeMeta);
    else
        _d->RegisterSingletonMetaObject(resolvableTypeMeta.className(), typeMeta);
}

void DIContainer::ClassBind(const QMetaObject &typeMeta,
                            const DIContainer::Scope &scope) {
    if (scope == DIContainer::NO_SCOPE)
        _d->RegisterMetaObject(typeMeta.className(), typeMeta);
    else
        _d->RegisterSingletonMetaObject(typeMeta.className(), typeMeta);
}

void DIContainer::ValueBind(const QString &key, const QVariant &value) {
    _d->RegisterValue(value.typeName(), key, value);
}

void DIContainer::ValueBind(const QMetaObject &typeMeta, QObject* const &value) {
    _d->RegisterSingletonMetaObject(typeMeta.className(), typeMeta);
    _d->RegisterSingletonValue(typeMeta.className(), value);
}
