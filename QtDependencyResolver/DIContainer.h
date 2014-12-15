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

#pragma once

#include <QObject>
#include <QMetaType>
#include <QSharedPointer>
#include <QVariant>
#include <typeinfo>
#include <QDebug>

#include "qtdependencyresolver_global.h"

namespace QtDependencyResolver
{
    class QTDEPENDENCYRESOLVERSHARED_EXPORT DIContainer : public QObject
    {
        Q_OBJECT
    public:
        enum Scope {
            NO_SCOPE,
            SINGLETON,
        };

        explicit DIContainer(QObject *parent = 0);
        virtual ~DIContainer();

        template <typename ResolvableType, typename Type>
        void Bind(const DIContainer::Scope& scope = DIContainer::NO_SCOPE)
        {
            QObject* objectFromType = static_cast<Type*>(0); Q_UNUSED(objectFromType);
            QObject* objectFromResolvableType = static_cast<ResolvableType*>(0); Q_UNUSED(objectFromResolvableType);
            ResolvableType* resolvableTypeFromType = static_cast<Type*>(0); Q_UNUSED(resolvableTypeFromType);
            ClassBind(static_cast<ResolvableType*>(0)->staticMetaObject, static_cast<Type*>(0)->staticMetaObject, scope);
        }

        template <typename Type>
        void Bind(const DIContainer::Scope& scope = DIContainer::NO_SCOPE)
        {
            QObject* objectFromType = static_cast<Type*>(0); Q_UNUSED(objectFromType);
            ClassBind(static_cast<Type*>(0)->staticMetaObject, scope);
        }

        template <typename Type>
        void BindSingleton(QObject* const &value)
        {
            ValueBind(static_cast<Type*>(0)->staticMetaObject, value);
        }

        void Bind(const QString &key, const QVariant &value)
        {
            ValueBind(key, value);
        }

        template <typename ResolvableType>
        ResolvableType* Resolve()
        {
            QObject* resolvableTypeToObjectCastCheck = static_cast<ResolvableType*>(0); Q_UNUSED(resolvableTypeToObjectCastCheck);
            return qobject_cast<ResolvableType*>(ResolveMetaobject(static_cast<ResolvableType*>(0)->staticMetaObject));
        }

        QObject* ResolveByClassName(const QString& className);

        QObject* ResolveMetaobject(QMetaObject metaObject);

    private:
        void ClassBind(const QMetaObject &resolvableTypeMeta,
                       const QMetaObject &typeMeta,
                       const DIContainer::Scope &scope);
        void ClassBind(const QMetaObject &typeMeta,
                       const DIContainer::Scope &scope);
        void ValueBind(const QString &key, const QVariant &value);
        void ValueBind(const QMetaObject &typeMeta,
                       QObject* const &value);

    private:
        class P; QSharedPointer<P> _d;
    };

    typedef QSharedPointer<DIContainer> DIContainerPtr;
}

Q_DECLARE_METATYPE(QtDependencyResolver::DIContainerPtr)
