/********************************************************************

    Copyright (c) 2013-2015 - Mogara

    This file is part of QSanguosha.

    This game engine is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3.0
    of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the LICENSE file for more details.

    Mogara
*********************************************************************/

#include "card.h"
#include "serverplayer.h"
#include "datavalue.h"

namespace DataValue
{

    namespace
    {
#define DATAVALUE_METAOBJECT(x) std::make_pair(QString(#x), &x::staticMetaObject)
        QMap<QString, const QMetaObject *> metaObjects {
            DATAVALUE_METAOBJECT(CardMove),
            DATAVALUE_METAOBJECT(CardsMoveValue),
            DATAVALUE_METAOBJECT(PhaseChangeValue),
            DATAVALUE_METAOBJECT(CardUseValue),
            DATAVALUE_METAOBJECT(CardEffectValue),
            DATAVALUE_METAOBJECT(DamageValue),
            DATAVALUE_METAOBJECT(RecoverValue),
            DATAVALUE_METAOBJECT(CardResponseValue),
            DATAVALUE_METAOBJECT(JudgeValue),
            DATAVALUE_METAOBJECT(DeathValue),
            DATAVALUE_METAOBJECT(SkillValue),
            DATAVALUE_METAOBJECT(SkillInvokeValue),
            DATAVALUE_METAOBJECT(IntValue)
        };
#undef DATAVALUE_METAOBJECT
    }

    QObject *newDataValue(const QString &type)
    {
        if (!metaObjects.contains(type))
            return nullptr;

        const QMetaObject *meta = metaObjects.value(type);
        QObject *value = meta->newInstance();
        return value;
    }

    QObject *newDataValue(const QString &, int value)
    {
        return new IntValue(value);
    }

    QObject *newDataValue(const QString &, QObject *value) // we must use a list to deal with this condition!!!
    {
        CardUseValue *use = qobject_cast<CardUseValue *>(value);
        if (use == nullptr)
            return nullptr;
        return new CardEffectValue(*use);
    }

    QObject *newDataValue(const QString &, const QString &value)
    {
        return new JudgeValue(value);
    }

    CardMove::CardMove()
        : fromArea(nullptr), toArea(nullptr), toDirection(CardArea::UndefinedDirection), card(nullptr), isOpen(false)
    {

    }


    CardMove::CardMove(const CardMove &move)
        : fromArea(move.fromArea), toArea(move.toArea), toDirection(move.toDirection), card(move.card), isOpen(move.isOpen)
    {

    }

    bool CardMove::operator ==(const CardMove &move)
    {
        return fromArea == move.fromArea && toArea == move.toArea && toDirection == move.toDirection && card == move.card && isOpen == move.isOpen;
    }

    CardMove &CardMove::operator=(const CardMove &move)
    {
        fromArea = move.fromArea;
        toArea = move.toArea;
        toDirection = move.toDirection;
        card = move.card;
        isOpen = move.isOpen;

        return *this;
    }

    bool CardMove::isRelevant(const Player *player) const
    {
        if (player == nullptr)
            return true;

        if (fromArea != nullptr) {
            if (fromArea->type() != CardArea::Special && player == fromArea->owner())
                return true;
        }

        if (toArea != nullptr) {
            if (toArea->type() != CardArea::Special && player == toArea->owner())
                return true;
        }

        return false;
    }

    QVariant CardMove::toVariant(bool open /*= false*/) const
    {
        QVariantMap map;
        if (fromArea != nullptr)
            map["fromArea"] = fromArea->toVariant();
        if (toArea != nullptr)
            map["toArea"] = toArea->toVariant();
        map["toDirection"] = static_cast<int>(toDirection);
        if (card != nullptr && (isOpen || open))
            map["card"] = card->id();
        map["isOpen"] = isOpen;
        return map;
    }

    CardsMoveValue::CardsMoveValue()
    {

    }


    CardsMoveValue::CardsMoveValue(const CardsMoveValue &move)
        : moves(move.moves)
    {

    }

    QVariant CardsMoveValue::toVariant(bool open /*= false*/) const
    {
        QVariantMap map;
        QVariantList list;
        foreach(const CardMove &move, moves)
            list << move.toVariant(open);
        map["moves"] = list;
        return map;
    }

    QVariant CardsMoveValue::toVariant(const Player *relevantPlayer) const
    {
        QVariantMap map;
        QVariantList list;
        foreach(const CardMove &move, moves)
            list << move.toVariant(move.isRelevant(relevantPlayer));
        map["moves"] = list;
        return map;
    }

    CardsMoveValue &CardsMoveValue::operator=(const CardsMoveValue &move)
    {
        moves = move.moves;
        return *this;
    }

    PhaseChangeValue::PhaseChangeValue()
        : from(Player::InvalidPhase), to(Player::InvalidPhase)
    {

    }

    CardUseValue::CardUseValue()
        : from(nullptr)
        , card(nullptr)
        , target(nullptr)
        , isNullified(false)
        , isOwnerUse(true)
        , addHistory(true)
        , isHandcard(true)
        , reason(PlayReason)
    {
    }

    CardUseValue::CardUseValue(const CardUseValue &arg2)
        : QObject()
    {
        from = arg2.from;
        to = arg2.to;
        card = arg2.card;
        target = arg2.target;
        nullifiedList = arg2.nullifiedList;
        isNullified = arg2.isNullified;
        isOwnerUse = arg2.isOwnerUse;
        addHistory = arg2.addHistory;
        isHandcard = arg2.isHandcard;
        reason = arg2.reason;
        extra = arg2.extra;
    }

    CardUseValue &CardUseValue::operator =(const CardUseValue &arg2)
    {
        from = arg2.from;
        to = arg2.to;
        card = arg2.card;
        target = arg2.target;
        nullifiedList = arg2.nullifiedList;
        isNullified = arg2.isNullified;
        isOwnerUse = arg2.isOwnerUse;
        addHistory = arg2.addHistory;
        isHandcard = arg2.isHandcard;
        reason = arg2.reason;
        extra = arg2.extra;

        return *this;
    }

    CardEffectValue::CardEffectValue(CardUseValue &use)
        : use(use)
        , from(use.from)
        , to(nullptr)
    {
    }

    DamageValue::DamageValue()
        : from(nullptr)
        , to(nullptr)
        , card(nullptr)
        , damage(1)
        , nature(Normal)
        , chain(false)
        , transfer(false)
        , byUser(true)
        , prevented(false)
    {
    }

    RecoverValue::RecoverValue()
        : from(nullptr)
        , to(nullptr)
        , recover(1)
        , card(nullptr)
    {
    }

    CardResponseValue::CardResponseValue()
        : from(nullptr)
        , to(nullptr)
        , card(nullptr)
        , target(nullptr)
        , isRetrial(false)
    {
    }

    JudgeValue::JudgeValue(const QString &pattern)
        : who(nullptr)
        , card(nullptr)
        , matched(false)
        , m_pattern(pattern)
    {
    }

    void JudgeValue::updateResult()
    {
        matched = m_pattern.match(who, card);
    }

    DeathValue::DeathValue()
        : who(nullptr)
        , damage(nullptr)
    {
    }

    SkillValue::SkillValue()
        : owner(nullptr)
        , skill(nullptr)
        , area(Player::UnknownSkillArea)
    {
    }

    SkillInvokeValue::SkillInvokeValue()
        : player(nullptr)
        , skill(nullptr)
    {

    }

    SkillInvokeValue::SkillInvokeValue(const SkillInvokeValue &arg2)
        : QObject()
    {
        player = arg2.player;
        skill = arg2.skill;
        targets = arg2.targets;
        cards = arg2.cards;
    }

    SkillInvokeValue &SkillInvokeValue::operator =(const SkillInvokeValue &arg2)
    {
        player = arg2.player;
        skill = arg2.skill;
        targets = arg2.targets;
        cards = arg2.cards;

        return *this;
    }

    IntValue::IntValue(int value)
        : value(value)
    {

    }
}